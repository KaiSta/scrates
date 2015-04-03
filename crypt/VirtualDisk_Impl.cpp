#include "VirtualDisk_Impl.h"

#ifdef _WIN32
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <initguid.h>
#include <virtdisk.h>
#include <rpc.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <ShlObj.h>


#define PHYS_PATH_LEN 1024+1

#define ARRAY_SIZE(a)                               \
	((sizeof(a) / sizeof(*(a))) / \
	static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))


VirtualDisk_Impl::VirtualDisk_Impl() : Storage{}
{
}


VirtualDisk_Impl::~VirtualDisk_Impl()
{
}

bool VirtualDisk_Impl::mount_drive(const std::string& path, volume_handle& out)
{
	bool res = false;
	if (exists(path))
	{
		TCHAR buffer[100];
		GetLogicalDriveStrings(100, buffer);
		std::vector<std::string> drives_old;

		for (int i = 0; i < 100; i += 4)
		{
			std::string tmp(buffer + i);
			if (tmp.empty())
				break;
			drives_old.push_back(tmp);
		}

		WCHAR widestring_path[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, widestring_path,
			sizeof(widestring_path));
		auto result = open_disk(widestring_path, &out.handle);
	
		if (result == ERROR_SUCCESS)
		{
			result = attach_disk(widestring_path, &out.handle);
			if (result == ERROR_SUCCESS)
			{
				out.path = path;

				Sleep(2500);//HACK to ensure VHD is attached
				TCHAR buffer[100];
				GetLogicalDriveStrings(100, buffer);
				std::vector<std::string> drives_new;

				for (int i = 0; i < 100; i += 4)
				{
					std::string tmp(buffer + i);
					if (tmp.empty())
						break;
					drives_new.push_back(tmp);
				}

				for (auto& e : drives_old)
				{
					auto it = std::find(drives_new.begin(),
						drives_new.end(), e);
					drives_new.erase(it);
				}
				if (drives_new.size() == 1)
				{
					out.drive_letter = drives_new[0];
					out.is_open = true;
					res = true;
				}		
				else
				{
					dismount_drive(out);
				}
				
			}
		}
	}
	return res;
}

bool VirtualDisk_Impl::create(const std::string& path, int64_t disk_size_mb, volume_handle& out)
{
	WCHAR widestring_path[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1,
		widestring_path, sizeof(widestring_path));
	out.path = path;

	//create, open and attach disk
	auto result = create_disk(widestring_path, &out.handle, disk_size_mb);
	if (result == ERROR_FILE_EXISTS)
	{
		result = open_disk(widestring_path, &out.handle);
		if (result != ERROR_SUCCESS)
		{
			CloseHandle(out.handle);
			return false;
		}
	}
	else if (result != ERROR_SUCCESS)
	{
		CloseHandle(out.handle);
		return false;
	}
	result = attach_disk(widestring_path, &out.handle);
	if (result != ERROR_SUCCESS)
	{
		CloseHandle(out.handle);
		return false;
	}
	out.is_open = true;

	//get physicalpath for further work
	ULONG sizePhysicalDisk = ((MAX_PATH * sizeof(wchar_t)) * 256);
	wchar_t pszPhysicalDiskPath[MAX_PATH * 256];
	memset(pszPhysicalDiskPath, 0, sizeof(wchar_t) * MAX_PATH);
	result = GetVirtualDiskPhysicalPath(out.handle, &sizePhysicalDisk, 
		pszPhysicalDiskPath);
	if (result != ERROR_SUCCESS)
	{
		cleanup(out);
		return false;
	}
	std::wstring phydiskpath(pszPhysicalDiskPath);
	
	Sleep(2500);//HACK to ensure VHD is attached

	//get devicehandle
	int size = WideCharToMultiByte(CP_UTF8, 0, phydiskpath.c_str(),
		-1, NULL, 0, 0, NULL);
	std::vector<char> buf(size);
	WideCharToMultiByte(CP_UTF8, 0, phydiskpath.c_str(), -1, &buf[0], size,
		0, NULL);
	std::string phydiskpath_utf8(&buf[0]);

	HANDLE vol_handle = CreateFile(
		phydiskpath_utf8.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (vol_handle == INVALID_HANDLE_VALUE)
	{
		cleanup(out);
		return false;
	}

	//intialize disk with GPT
	GUID u_id;
	if (UuidCreate((UUID*)&u_id) != RPC_S_OK)
	{
		cleanup(out);
		return false;
	}
	CREATE_DISK cdisk;
	cdisk.PartitionStyle = PARTITION_STYLE_GPT;
	cdisk.Gpt.MaxPartitionCount = 128;
	cdisk.Gpt.DiskId = u_id;
	DWORD junk;
	BOOL res = DeviceIoControl(
		vol_handle,
		IOCTL_DISK_CREATE_DISK,
		&cdisk,
		sizeof(cdisk),
		NULL,
		0,
		&junk,
		NULL);
	if (res != TRUE)
	{
		dismount_drive(out);
		CloseHandle(out.handle);
		return false;
	}

	//get drive letters from currently mounted drives
	TCHAR buffer[100];
	GetLogicalDriveStrings(100, buffer);
	std::vector<std::string> drives_old;

	for (int i = 0; i < 100; i += 4)
	{
		std::string tmp(buffer + i);
		if (tmp.empty())
			break;
		drives_old.push_back(tmp);
	}
	
	//partition drive
	DRIVE_LAYOUT_INFORMATION drive_layout = { 0 };
	drive_layout.PartitionCount = 1;
	drive_layout.PartitionEntry[0].StartingOffset.QuadPart = 1048576;
	drive_layout.PartitionEntry[0].PartitionLength.QuadPart = (disk_size_mb * 1024 * 1024) - 1048576;
	drive_layout.PartitionEntry[0].HiddenSectors = 0;
	drive_layout.PartitionEntry[0].PartitionNumber = 1;
	drive_layout.PartitionEntry[0].PartitionType = PARTITION_IFS;
	drive_layout.PartitionEntry[0].BootIndicator = FALSE;
	drive_layout.PartitionEntry[0].RecognizedPartition = TRUE;
	drive_layout.PartitionEntry[0].RewritePartition = TRUE;
	res = DeviceIoControl(
		vol_handle,
		IOCTL_DISK_SET_DRIVE_LAYOUT,
		&drive_layout,
		sizeof(drive_layout),
		NULL,
		0,
		&junk,
		NULL
		);
	if (res == FALSE)
	{
		cleanup(out);
		return false;
	}

	Sleep(2500); // HACK to ensure drive has been partitioned

	//get VHDs drive letter
	TCHAR buffer2[100];
	GetLogicalDriveStrings(100, buffer2);
	std::vector<std::string> drives_new;
	for (int i = 0; i < 100; i += 4)
	{
		std::string tmp(buffer2 + i);
		if (tmp.empty())
			break;
		drives_new.push_back(tmp);
	}
	for (auto& e : drives_old)
	{
		auto it = std::find(drives_new.begin(),
			drives_new.end(), e);
		drives_new.erase(it);
	}
	if (drives_new.size() > 1)
	{
		cleanup(out);
		return false;
	}
	std::string letter(drives_new[0]);
	out.drive_letter = letter;
	
	//format VHD
	format_disk(letter);
	return true;
}

unsigned long VirtualDisk_Impl::create_disk(PCWSTR diskfilepath, HANDLE* handle, int64_t disk_size_mb)
{
	GUID unique_id;
	DWORD status;

	if (UuidCreate((UUID*)&unique_id) != RPC_S_OK)
	{
		status = ERROR_NOT_ENOUGH_MEMORY;
		return status;
	}

	VIRTUAL_STORAGE_TYPE storageType = {
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	int64_t max_size = disk_size_mb * int64_t(1024) * int64_t(1024);
	CREATE_VIRTUAL_DISK_PARAMETERS parameters = {};
	parameters.Version = CREATE_VIRTUAL_DISK_VERSION_1;
	parameters.Version1.MaximumSize = max_size;
	parameters.Version1.BlockSizeInBytes = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_BLOCK_SIZE;
	parameters.Version1.SectorSizeInBytes = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_SECTOR_SIZE;
	parameters.Version1.SourcePath = NULL;
	parameters.Version1.UniqueId = unique_id;

	status = CreateVirtualDisk(
		&storageType,
		diskfilepath,
		VIRTUAL_DISK_ACCESS_ALL,
		NULL,
		CREATE_VIRTUAL_DISK_FLAG_NONE,
		0,
		&parameters,
		NULL,
		handle);

	return status;
}
unsigned long VirtualDisk_Impl::open_disk(PCWSTR diskfilepath, HANDLE* handle)
{
	VIRTUAL_STORAGE_TYPE storageType =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	OPEN_VIRTUAL_DISK_PARAMETERS parameters =
	{
		OPEN_VIRTUAL_DISK_VERSION_1
	};

	parameters.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;//1024;

	return OpenVirtualDisk(
		&storageType,
		diskfilepath,
		VIRTUAL_DISK_ACCESS_ALL,
		OPEN_VIRTUAL_DISK_FLAG_NONE,
		&parameters,
		handle);
}
unsigned long VirtualDisk_Impl::attach_disk(PCWSTR diskfilepath, HANDLE* handle)
{
	ATTACH_VIRTUAL_DISK_PARAMETERS parameters = {};
	parameters.Version = ATTACH_VIRTUAL_DISK_VERSION_1;
	return AttachVirtualDisk(
		*handle,
		NULL,
		ATTACH_VIRTUAL_DISK_FLAG_NONE,
		//ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME,
		0,
		&parameters,
		NULL);
}
void VirtualDisk_Impl::format_disk(const std::string& letter)
{
	if (letter[0] <= 'A' || letter[0] >= 'Z')
		return;

	int drive_number = letter[0] - 'A';
	auto res = SHFormatDrive(NULL, drive_number, SHFMT_ID_DEFAULT, SHFMT_OPT_FULL);

	switch (res)
	{
	case SHFMT_ERROR:
	case SHFMT_NOFORMAT:
		return;
	case SHFMT_CANCEL:
		format_disk(letter);
		break;
	}
}

void VirtualDisk_Impl::dismount_drive(volume_handle& in)
{
	if (in.is_open)
	{
		DetachVirtualDisk(in.handle, DETACH_VIRTUAL_DISK_FLAG_NONE, 0);
		CloseHandle(in.handle);
		in.is_open = false;
		Sleep(1000);
	}
}

void VirtualDisk_Impl::cleanup(volume_handle& in)
{
	dismount_drive(in);
	DeleteFile(in.path.c_str());
	//FileSystem::delete_file(in.path);
}

bool VirtualDisk_Impl::exists(const std::string& p)
{
	auto file_attributes = GetFileAttributes(path_to_systemstandard_(p).data());
	return (file_attributes != INVALID_FILE_ATTRIBUTES) &&
		(file_attributes != FILE_ATTRIBUTE_DIRECTORY) &&
		(GetLastError() != ERROR_FILE_NOT_FOUND);
}
#endif