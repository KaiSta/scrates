#include "Storage.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <string>
#include <stdint.h>
#include "FileSystem.h"

class Folder_Impl : public Storage
{
public:
	Folder_Impl() : Storage{}
	{}
	virtual ~Folder_Impl()
	{}

	virtual bool mount_drive(const std::string& path, volume_handle& out)
	{
		return create(path, 0, out);
	}

	virtual bool create(const std::string& path, int64_t disk_size_mb, volume_handle& out)
	{
		FileSystem::make_folders(path);

		if (path[path.size()] != FileSystem::path_separator)
			out.drive_letter = out.path = path + FileSystem::path_separator;
		else
			out.drive_letter = out.path = path;

		out.is_open = true;
#ifdef _WIN32
		out.handle = nullptr;
#endif
		return true;
	}
	virtual void dismount_drive(volume_handle& in)
	{
		FileSystem::delete_directory(in.path);
	}
private:
};