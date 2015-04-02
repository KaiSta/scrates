#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <stdint.h>
//#include "FileSystem.h"


class Storage
{
public:
	enum store_t
	{
		VHD,
		FOLDER
	};
	struct volume_handle
	{
		
		std::string path;
		std::string drive_letter;
		bool is_open;
		store_t type;

#ifdef _WIN32
		HANDLE handle;
		void close()
		{
			CloseHandle(handle);
		}

		volume_handle() : handle(INVALID_HANDLE_VALUE), path(),
			drive_letter(), is_open(false)
		{
		}
#else
		void close()
		{

		}
		volume_handle() : path(), drive_letter(), is_open(false)
		{
		}
#endif
	};

	virtual ~Storage(){}

	virtual bool mount_drive(const std::string& path, volume_handle& out) = 0;
	virtual bool create(const std::string& path, int64_t disk_size_mb, volume_handle& out) = 0;
	virtual void dismount_drive(volume_handle& in) = 0;

};