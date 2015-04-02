#pragma once

#include <string>
#include <vector>

//#include <Windows.h>
#include <stdint.h>
#include <fstream>
#include "string_helper.h"

#include <cryptopp\cryptlib.h>
#include <cryptopp\files.h>
#include <cryptopp\filters.h>
#include <cryptopp\crc.h>

#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/DirectoryIterator.h>

//#ifdef _WIN32
class FileSystem
{
public:

	static bool file_exists(const std::string& path)
	{
		try
		{
			Poco::File f(path);
			return f.exists() && !f.isDirectory();
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: file_exists" << std::endl;
			return false;
		}
		//	auto file_attributes = GetFileAttributes(path_to_systemstandard(path).data());
		//	return (file_attributes != INVALID_FILE_ATTRIBUTES) &&
		//		(file_attributes != FILE_ATTRIBUTE_DIRECTORY) &&
		//		(GetLastError() != ERROR_FILE_NOT_FOUND);
	}
	
	/**
	  \brief Returns true if given path points to a directory.
	*/
	static bool is_directory(const std::string& path)
	{
		try
		{
			Poco::File f(path);
			return f.exists() && f.isDirectory();
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: is_directory" << std::endl;
		}
		return false;
	/*	auto attributes = GetFileAttributes(path_to_systemstandard(path).c_str());
		return (attributes == FILE_ATTRIBUTE_DIRECTORY);*/
	}

	/**
	  \brief Returns the file size in bytes.
	*/
	static int64_t file_size(const std::string& path)
	{
		try
		{
			Poco::File f(path);
			if (f.exists())
				return f.getSize();
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: file_size" << std::endl;
		}
		return -1;
		/*WIN32_FILE_ATTRIBUTE_DATA fatt;
		if (!GetFileAttributesEx(path_to_systemstandard(path).c_str(), GetFileExInfoStandard, &fatt))
		{
			return -1;
		}
		LARGE_INTEGER size;
		size.HighPart = fatt.nFileSizeHigh;
		size.LowPart = fatt.nFileSizeLow;
		return size.QuadPart;*/
	}


	/**
	  \brief creates a directory at the given location.
	  if the given path is invalid because a higher level directory is missing
	  the method will fail and return false.
	  \param dest path ending with the directory that shall be created.
	*/
	static bool create_directory(const std::string& dest)
	{
		try
		{
		Poco::File f(dest);
		return f.createDirectory();
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: create_directory" << std::endl;
		}
		//CreateDirectory(dest.data(), 0);
		//if (!CreateDirectory(dest.data(), 0))
		//{
		//	DWORD err = GetLastError();
		//	if (err == ERROR_ALREADY_EXISTS || err == ERROR_PATH_NOT_FOUND)
		//	{
		//		//std::cout << "error directory already exists\n";
		//		return false;
		//	}
		//}
		//return true;
	}

	/**
	  \brief deletes the directory at the given path.
	  \return true if successfull.
	*/
	static bool delete_directory(const std::string& dest)
	{
		try
		{
			Poco::File f(dest);
			if (f.exists())
			{
				f.remove(true);
				return true;
			}
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: delete_directory" << std::endl;
			std::cout << dest << std::endl;
		}
		return false;
		/*if (is_directory(dest))
			return RemoveDirectory(dest.data()) != 0;
		else
			return false;*/
	}
	
	/**
	\brief deletes the file at the given path.
	\return true if successfull.
	*/
	static bool delete_file(const std::string& dest)
	{
		try
		{
			Poco::File f(dest);
			if (f.exists())
			{
				f.remove();
				return true;
			}
			
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: delete_file" << std::endl;
		}
		return false;
		/*if (file_exists(dest))
		{
			return DeleteFile(dest.data()) != 0;
		}
		return false;*/
	}

	/**
	  \brief DEPRECATED. Gives a list of all files in given directory.
	  Doesn't include sub directories.
	  \param dest path to the directory.
	  \param out vector<string> with the paths to the found files.
	  \return true if successfull.
	*/
	static bool get_all_files_in_dir(const std::string& dest, std::vector<std::string>& out)
	{
		try
		{
			Poco::File f(dest);
			if (!f.exists())
				return false;

			std::vector<std::string> files;
			f.list(files);

			for (auto it = files.begin(); it != files.end(); ++it)
			{
				Poco::File tmp(Poco::Path(dest, (*it)));
				if (tmp.isDirectory())
				{
					it = files.erase(it);
				}
			}

			for (auto& e : files)
			{
				out.push_back(Poco::Path(dest, e).toString());
			}
			return true;
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: get_all_files_in_dir" << std::endl;
		}
	}

	/**
	  \brief Deletes all files in the given directory.
	*/
	static void delete_all(const std::string& p)
	{
		try
		{
			Poco::DirectoryIterator end;
			for (Poco::DirectoryIterator it(p); it != end; ++it)
			{
				it->remove(true);
			}
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: delete_all" << std::endl;
		}

		/*auto files = list_files(p, true);
		for (auto& f : files)
		{
			delete_file(f);
		}
		auto folders = list_folders(p, true);
		for (auto& d : folders)
		{
			delete_directory(d);
		}*/
	}

	/**
	  \brief lists all folders in a given directory.
	  \param p path to the root directory.
	  \param recursive true to include subfolders.
	*/
	static std::vector<std::string> list_folders(const std::string& p, bool recursive)
	{
		try
		{
			Poco::File f(p);
			if (!f.exists())
				return{};

			std::vector<std::string> folders;
			f.list(folders);

			std::vector<std::string> found;

			for (auto it = folders.begin(); it != folders.end(); ++it)
			{
				Poco::File tmp(Poco::Path(f.path(), (*it)));

				if (tmp.isDirectory())
				{
					found.push_back(tmp.path());

					if (recursive)
					{
						auto v = list_folders(tmp.path(), true);
						found.insert(found.begin(), v.begin(), v.end());
					}
				}
			}

			return found;
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: list_folders" << std::endl;
		}
	}

	/**
	  \brief lists all files in a given directory.
	  \param p path to the root directory.
	  \param recursive true to include subfolders.
	*/
	static std::vector<std::string> list_files(const std::string& p, bool recursive)
	{
		try
		{
			Poco::File f(p);
			if (!f.exists())
				return{};

			std::vector<std::string> folders;
			f.list(folders);

			std::vector<std::string> found;

			for (auto it = folders.begin(); it != folders.end(); ++it)
			{
				Poco::File tmp(Poco::Path(f.path(), (*it)));

				if (tmp.isFile())
				{
					found.push_back(tmp.path());
				}
				else if (tmp.isDirectory() && recursive)
				{
					auto v = list_files(tmp.path(), true);
					found.insert(found.begin(), v.begin(), v.end());
				}
			}

			return found;
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: list_files" << std::endl;
		}
		return{};
	}

	/**
	  \brief Opens a file with 'open with' dialog.
	  \param path file location.
	*/
	//static void openas_file_external(const std::string& path)
	//{
	//	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	//	SHELLEXECUTEINFO sei = { sizeof(sei) };
	//	sei.nShow = SW_SHOWNORMAL;
	//	sei.lpVerb = "openas";
	//	sei.lpFile = path.data();
	//	sei.fMask = 12;
	//	ShellExecuteEx(&sei);
	//}

	///**
	//  \brief Opens the file with the default program.
	//*/
	//static void open_file_default(const std::string& path)
	//{
	//	ShellExecute(0, 0, path.data(), 0, 0, SW_SHOW);
	//}

	/**
	  \brief Returns the path to the temp directory.
	*/
	static std::string get_temp_path()
	{
		return Poco::Path::temp();
	}

	/**
	  \brief Copys file from src to dest.
	*/
	static void copy_file(const std::string& src, const std::string& dest)
	{
		try
		{
			Poco::File s(src);
			if (s.exists())
				s.copyTo(dest);
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: copy_file" << std::endl;
		}
	}

	/**
	  \brief creates all missing folders in given path.
	*/
	static void make_folders(const std::string& path)
	{
		try
		{
			Poco::File f(path);
			f.createDirectories();
		}
		catch (std::exception& e)
		{
			std::cout << "what: " << e.what() << " where: make_folder" << std::endl;
		}
//#ifdef _WIN32
//		tmp.erase(0, 3);
//		std::string current_path(path.begin(), path.begin()+3);	
//std::string tmp(FileSystem::path_to_systemstandard(path));
//		for (; tmp.size() > 0;)
//		{
//			auto n = tmp.find_first_of(FileSystem::path_separator);
//			if (n != std::string::npos)
//			{
//				current_path.append(std::string(tmp.begin(), tmp.begin() + (n + 1)));
//				tmp.erase(0, (n + 1));
//				if (!is_directory(current_path))
//					create_directory(current_path);
//				//std::cout << current_path << std::endl;
//			}
//			else
//			{
//				current_path.append(tmp);
//				if (!is_directory(current_path))
//					create_directory(current_path);
//				tmp.clear();
//			}
//		}
//#endif
	}

	/**
	  \brief Returns filename.
	*/
	static std::string name_from_path(const std::string& path)
	{
		std::string s(FileSystem::path_to_systemstandard(path));
		size_t last_slash = path.find_last_of(FileSystem::path_separator);
		last_slash = (last_slash != std::string::npos) ? last_slash : 0;
		return std::string(path.begin() + last_slash + 1, path.end());
	}

	/**
	  \brief Appends filename to path in platform dependend format.
	*/
	static std::string join_path_filename(const std::string& path, const std::string& filename)
	{
		std::string p = path_to_systemstandard(path);
		if (p[p.size() - 1] == path_separator)
			p.append(filename);
		else
			p += (path_separator + filename);
		
		return p;
	}

	/**
	  \brief Returns path without the filename.
	*/
	static std::string get_folderpath(const std::string& path)
	{
		std::string tmp(path);
		if (!is_directory(tmp))
		{
			std::string name = name_from_path(tmp);
			tmp.erase(tmp.size() - name.size(), name.size());
		}
		return tmp;
	}

	/**
	  \brief Returns path without the filename.
	  Works with relative paths but the returned path is still invalid.
	*/
	static std::string get_folderpath_unsecure(const std::string& path)
	{
		std::string tmp(path);
		std::string name = name_from_path(tmp);
		tmp.erase(tmp.size() - name.size(), name.size());
		
		auto n = tmp.find_last_of(path_separator);
		if (n != std::string::npos)
		{
			tmp.erase(n, tmp.size() - n);
		}

		return tmp;
	}

	/**
	  \brief Replaces all '\' with '/'.
	*/
	static std::string standardize_path(const std::string& path)
	{
		std::string tmp(path);
		replaceAll(tmp, std::string("\\"), std::string("/"));
		return tmp;
	}

	/**
	  \brief Returns given path in system specific format.
	*/
	static std::string path_to_systemstandard(const std::string& path)
	{
		std::string tmp(path);
#ifdef _WIN32
		replaceAll(tmp, "/", std::string("\\"));
#endif
		return tmp;
	}

#ifdef _WIN32
	static const char path_separator = '\\';
#else
	static const char path_separator = '/';
#endif
	static const char standard_separator = '/';
};
//#endif