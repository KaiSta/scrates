#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <stdint.h>
#include "FileSystem.h"


class Storage
{
public:

	struct volume_handle
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
	};

	virtual ~Storage(){}

	virtual bool mount_drive(const std::string& path, volume_handle& out) = 0;
	virtual bool create(const std::string& path, int64_t disk_size_mb, volume_handle& out) = 0;
	virtual void dismount_drive(volume_handle& in) = 0;

};