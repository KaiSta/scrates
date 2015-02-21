#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <stdint.h>
#include "FileSystem.h"
#include "Storage.h"

class VirtualDisk_Impl : public Storage
{
public:
	/*struct volume_handle
	{
		HANDLE handle;
		std::string path;
		std::string drive_letter;
		bool is_open;

		void close()
		{
			CloseHandle(handle);
		}

		volume_handle() : handle(INVALID_HANDLE_VALUE), path(),
			drive_letter(), is_open(false)
		{
		}
	};*/
	VirtualDisk_Impl();
	virtual ~VirtualDisk_Impl();

	virtual bool mount_drive(const std::string& path, volume_handle& out);
	virtual bool create(const std::string& path, int64_t disk_size_mb, volume_handle& out);
	virtual void dismount_drive(volume_handle& in);

private:
	VirtualDisk_Impl(const VirtualDisk_Impl&);
	VirtualDisk_Impl& operator=(const VirtualDisk_Impl&);

	/*static*/ unsigned long create_disk(PCWSTR diskfilepath, HANDLE* handle, int64_t disk_size_mb);
	/*static*/ unsigned long open_disk(PCWSTR diskfilepath, HANDLE* handle);
	/*static*/ unsigned long attach_disk(PCWSTR diskfilepath, HANDLE* handle);
	/*static*/ void format_disk(const std::string& letter);
	/*static*/ void cleanup(volume_handle& in);
};

