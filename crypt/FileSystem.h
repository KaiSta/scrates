#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include <stdint.h>
#include <fstream>
#include "string_helper.h"

#include <cryptopp\cryptlib.h>
#include <cryptopp\files.h>
#include <cryptopp\filters.h>
#include <cryptopp\crc.h>


#ifdef _WIN32
class FileSystem
{
public:

	/**
	  \brief Determines if file exists at the given path.
	  \return true if file exists.
	*/
	static bool file_exists(const std::string& path)
	{
		auto file_attributes = GetFileAttributes(path_to_systemstandard(path).data());
		return (file_attributes != INVALID_FILE_ATTRIBUTES) &&
			(file_attributes != FILE_ATTRIBUTE_DIRECTORY) &&
			(GetLastError() != ERROR_FILE_NOT_FOUND);
	}
	
	/**
	  \brief Returns true if given path points to a directory.
	*/
	static bool is_directory(const std::string& path)
	{
		auto attributes = GetFileAttributes(path_to_systemstandard(path).c_str());
		return (attributes == FILE_ATTRIBUTE_DIRECTORY);
	}

	/**
	  \brief Returns the file size in bytes.
	*/
	static int64_t file_size(const std::string& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA fatt;
		if (!GetFileAttributesEx(path_to_systemstandard(path).c_str(), GetFileExInfoStandard, &fatt))
		{
			return -1;
		}
		LARGE_INTEGER size;
		size.HighPart = fatt.nFileSizeHigh;
		size.LowPart = fatt.nFileSizeLow;
		return size.QuadPart;
	}


	/**
	  \brief creates a directory at the given location.
	  if the given path is invalid because a higher level directory is missing
	  the method will fail and return false.
	  \param dest path ending with the directory that shall be created.
	*/
	static bool create_directory(const std::string& dest)
	{
		if (!CreateDirectory(dest.data(), 0))
		{
			DWORD err = GetLastError();
			if (err == ERROR_ALREADY_EXISTS || err == ERROR_PATH_NOT_FOUND)
			{
				//std::cout << "error directory already exists\n";
				return false;
			}
		}
		return true;
	}

	/**
	  \brief deletes the directory at the given path.
	  \return true if successfull.
	*/
	static bool delete_directory(const std::string& dest)
	{
		if (is_directory(dest))
			return RemoveDirectory(dest.data()) != 0;
		else
			return false;
	}

	/**
	\brief deletes the file at the given path.
	\return true if successfull.
	*/
	static bool delete_file(const std::string& dest)
	{
		if (file_exists(dest))
		{
			return DeleteFile(dest.data()) != 0;
		}
		return false;
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
		using namespace std;

		HANDLE dir;
		WIN32_FIND_DATA file_data;

		if ((dir = FindFirstFile((dest + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
			return false; /* No files found */

		do {
			const string file_name = file_data.cFileName;
			string full_file_name;
			if (dest[dest.size() - 1] == path_separator)
			{
				full_file_name = dest + file_name;
			}
			else
			{
				full_file_name = dest + path_separator + file_name;
			}
			
			const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

			if (file_name[0] == '.')
				continue;

			if (is_directory)
				continue;

			out.push_back(full_file_name);
		} while (FindNextFile(dir, &file_data));

		FindClose(dir);
		return true;
	}

	/**
	  \brief Deletes all files in the given directory.
	*/
	static void delete_all(const std::string& p)
	{
		auto files = list_files(p, true);
		for (auto& f : files)
		{
			delete_file(f);
		}
		auto folders = list_folders(p, true);
		for (auto& d : folders)
		{
			delete_directory(d);
		}
	}

	/**
	  \brief lists all folders in a given directory.
	  \param p path to the root directory.
	  \param recursive true to include subfolders.
	*/
	static std::vector<std::string> list_folders(const std::string& p, bool recursive)
	{
		
		using namespace std;
		std::vector<std::string> folders;
		HANDLE dir;
		WIN32_FIND_DATA file_data;

		if ((dir = FindFirstFile((p + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
			return folders; /* No files found */

		do {
			const string file_name = file_data.cFileName;
			const string full_file_name = p + FileSystem::path_separator + file_name;
			const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

			if (file_name[0] == '.')
				continue;

			if (is_directory)
			{
				if (recursive)
				{
					std::vector<std::string> tmp = list_folders(full_file_name, true);
					folders.insert(folders.end(), tmp.begin(), tmp.end());
				}			
				folders.push_back(full_file_name);
			}
		} while (FindNextFile(dir, &file_data));

		FindClose(dir);
		return folders;
	}

	/**
	  \brief lists all files in a given directory.
	  \param p path to the root directory.
	  \param recursive true to include subfolders.
	*/
	static std::vector<std::string> list_files(const std::string& p, bool recursive)
	{
		std::string pa(p);

		if (pa[pa.size() - 1] != path_separator)
		{
			pa.append(path_separator + "");
		}

		if (!recursive)
		{
			std::vector<std::string> files;
			get_all_files_in_dir(pa, files);
			return std::vector<std::string>(files.begin(), files.end());
		}
		using namespace std;
		std::vector<std::string> files;
		HANDLE dir;
		WIN32_FIND_DATA file_data;

		if ((dir = FindFirstFile((pa + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
			return files; /* No files found */

		do {
			const string file_name = file_data.cFileName;
			const string full_file_name = pa + FileSystem::path_separator + file_name;
			const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

			if (file_name[0] == '.')
				continue;

			if (is_directory)
			{
				std::vector<std::string> tmp = list_files(full_file_name, true);
				files.insert(files.end(), tmp.begin(), tmp.end());
			}
			else
				files.push_back(full_file_name);
		} while (FindNextFile(dir, &file_data));

		FindClose(dir);
		return files;
	}

	/**
	  \brief Opens a file with 'open with' dialog.
	  \param path file location.
	*/
	static void openas_file_external(const std::string& path)
	{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.nShow = SW_SHOWNORMAL;
		sei.lpVerb = "openas";
		sei.lpFile = path.data();
		sei.fMask = 12;
		ShellExecuteEx(&sei);
	}

	/**
	  \brief Opens the file with the default program.
	*/
	static void open_file_default(const std::string& path)
	{
		ShellExecute(0, 0, path.data(), 0, 0, SW_SHOW);
	}

	/**
	  \brief Returns the path to the temp directory.
	*/
	static std::string get_temp_path()
	{
		char path[MAX_PATH + 1];
		if (GetTempPath(MAX_PATH + 1, path) != 0)
		{
			return std::string(path);
		}
		return std::string("");
	}

	/**
	  \brief Copys file from src to dest.
	*/
	static void copy_file(const std::string& src, const std::string& dest)
	{
		CopyFile(src.data(), dest.data(), FALSE);
	}

	/**
	  \brief creates all missing folders in given path.
	*/
	static void make_folders(const std::string& path)
	{
		std::string tmp(FileSystem::path_to_systemstandard(path));
#ifdef _WIN32
		tmp.erase(0, 3);
		std::string current_path(path.begin(), path.begin()+3);	

		for (; tmp.size() > 0;)
		{
			auto n = tmp.find_first_of(FileSystem::path_separator);
			if (n != std::string::npos)
			{
				current_path.append(std::string(tmp.begin(), tmp.begin() + (n + 1)));
				tmp.erase(0, (n + 1));
				if (!is_directory(current_path))
					create_directory(current_path);
				//std::cout << current_path << std::endl;
			}
			else
			{
				current_path.append(tmp);
				if (!is_directory(current_path))
					create_directory(current_path);
				tmp.clear();
			}
		}
#endif
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
#endif