#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <stdint.h>
//#include "FileSystem.h"
#include "Storage.h"
#include "string_helper.h"

class VirtualDisk_Impl : public Storage
{
public:
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

	bool exists(const std::string& p);
	static std::string path_to_systemstandard_(const std::string& path)
	{
		std::string tmp(path);
#ifdef _WIN32
		replaceAll(tmp, "/", std::string("\\"));
#else
		replaceAll(tmp, std::string("\\"), "/");
#endif
		return tmp;
	}
};

