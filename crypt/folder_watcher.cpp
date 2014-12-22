//#include <cloudcrypt\folder_watcher.h>
#include "folder_watcher.h"
#include <functional>

#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#endif

#define WAIT_TIME 10000

folder_watcher::folder_watcher(const std::string& dir) : runs_(false), dir_(dir), filter_(FILE_NOTIFY_CHANGE_FILE_NAME)
{
}


folder_watcher::~folder_watcher()
{
	force_stop();
}

static void watch_directory(folder_watcher* pfw)
{
	DWORD dwWaitStatus;
	HANDLE dwChangeHandles[2];
	TCHAR lpDrive[4];
	TCHAR lpFile[_MAX_FNAME];
	TCHAR lpExt[_MAX_EXT];

	_tsplitpath_s(LPTSTR(pfw->get_directory().data()), lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

	lpDrive[2] = (TCHAR)'\\';
	lpDrive[3] = (TCHAR)'\0';

	// Watch the directory for file creation and deletion. 

	dwChangeHandles[0] = FindFirstChangeNotification(
		LPTSTR(pfw->get_directory().data()), // directory to watch 
		TRUE,                         // do not watch subtree 
		pfw->get_filter()); // watch file name changes 

	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE)
	{
		printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		ExitProcess(GetLastError());
	}

	// Watch the subtree for directory creation and deletion. 
	
	dwChangeHandles[1] = FindFirstChangeNotification(
		lpDrive,                       // directory to watch 
		TRUE,                          // watch the subtree 
		FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 

	if (dwChangeHandles[1] == INVALID_HANDLE_VALUE)
	{
		printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		ExitProcess(GetLastError());
	}


	// Make a final validation check on our handles.

	if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
	{
		printf("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
		ExitProcess(GetLastError());
	}

	// Change notification is set. Now wait on both notification 
	// handles and refresh accordingly. 

	while (true)
	{
		// Wait for notification.

		//printf("\nWaiting for notification...\n");

		dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles, FALSE, WAIT_TIME);

		if (!pfw->is_running())
		{
			break;
		}

		switch (dwWaitStatus)
		{
		case WAIT_OBJECT_0:

			// A file was created, renamed, or deleted in the directory.
			// Refresh this directory and restart the notification.
			//printf("fileupdate\n");
			pfw->call_callback();
			if (FindNextChangeNotification(dwChangeHandles[0]) == FALSE)
			{
				printf("\n ERROR: FindNextChangeNotification function failed.\n");
				ExitProcess(GetLastError());
			}
			
			break;

		case WAIT_OBJECT_0 + 1:

			// A directory was created, renamed, or deleted.
			// Refresh the tree and restart the notification.
			//printf("folderupdate\n");
			pfw->call_callback();
			if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE)
			{
				printf("\n ERROR: FindNextChangeNotification function failed.\n");
				ExitProcess(GetLastError());
			}
			break;

		case WAIT_TIMEOUT:

			// A timeout occurred, this would happen if some value other 
			// than INFINITE is used in the Wait call and no changes occur.
			// In a single-threaded environment you might not want an
			// INFINITE wait.

			//printf("\nNo changes in the timeout period.\n");
			break;

		default:
			printf("\n ERROR: Unhandled dwWaitStatus.\n");
			ExitProcess(GetLastError());
			break;
		}
	}
}

static DWORD WINAPI tfunc(LPVOID lpParam)
{
	folder_watcher* s = (folder_watcher*)lpParam;
	watch_directory(s);
	return 0;
}

bool folder_watcher::is_running()
{
	return runs_;
}

void folder_watcher::run()
{
	runs_ = true;
	thread_ = CreateThread(0, 0, tfunc, LPVOID(this), 0, 0);
	SetThreadPriority(thread_, THREAD_PRIORITY_LOWEST);
}

void folder_watcher::stop()
{
	runs_ = false;
}

void folder_watcher::force_stop()
{
	runs_ = false;
	TerminateThread(thread_, 0);
}

void folder_watcher::set_directory(const std::string& dir)
{
	dir_ = dir;
}

void folder_watcher::set_callback(std::function<void()> callback)
{
	callback_ = callback;
}

std::string folder_watcher::get_directory()
{
	return dir_;
}

void folder_watcher::call_callback()
{
	static bool first_call = true;
	if (first_call)
	{
		first_call = false;
		callback_();
	}
	else
		first_call = true;
}

void folder_watcher::set_filter(filter f)
{
	if (f == WRITES_ONLY)
	{
		filter_ = FILE_NOTIFY_CHANGE_LAST_WRITE;
	}
	else
	{
		filter_ = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;
	}
}

DWORD folder_watcher::get_filter()
{
	return filter_;
}