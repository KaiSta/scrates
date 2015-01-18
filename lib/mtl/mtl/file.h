#ifndef MTL_FILE_H_
#define MTL_FILE_H_

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdint.h>
#include <string>

class File
{
public:
	/**
	\brief Determines if file exists at the given path.
	\return true if file exists.
	*/
	static bool file_exists(const std::string& path)
	{
#ifdef _WIN32
		auto file_attributes = GetFileAttributes(path_to_systemstandard(path).data());
		return (file_attributes != INVALID_FILE_ATTRIBUTES) &&
			(file_attributes != FILE_ATTRIBUTE_DIRECTORY) &&
			(GetLastError() != ERROR_FILE_NOT_FOUND);
#endif
	}

	/**
	\brief Returns the file size in bytes.
	*/
	static int64_t file_size(const std::string& path)
	{
#ifdef _WIN32
		WIN32_FILE_ATTRIBUTE_DATA fatt;
		if (!GetFileAttributesEx(path_to_systemstandard(path).c_str(), GetFileExInfoStandard, &fatt))
		{
			return -1;
		}
		LARGE_INTEGER size;
		size.HighPart = fatt.nFileSizeHigh;
		size.LowPart = fatt.nFileSizeLow;
		return size.QuadPart;
#endif
	}

	/**
	\brief deletes the file at the given path.
	\return true if successfull.
	*/
	static bool delete_file(const std::string& dest)
	{
#ifdef _WIN32
		if (file_exists(dest))
		{
			return DeleteFile(dest.data()) != 0;
		}
		return false;
#endif
	}

	/**
	\brief Opens a file with 'open with' dialog.
	\param path file location.
	*/
	static void openas_file_external(const std::string& path)
	{
#ifdef _WIN32
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.nShow = SW_SHOWNORMAL;
		sei.lpVerb = "openas";
		sei.lpFile = path.data();
		sei.fMask = 12;
		ShellExecuteEx(&sei);
#endif
	}

	/**
	\brief Opens the file with the default program.
	*/
	static void open_file_default(const std::string& path)
	{
#ifdef _WIN32
		ShellExecute(0, 0, path.data(), 0, 0, SW_SHOW);
#endif
	}

	/**
	\brief Copys file from src to dest.
	*/
	static void copy_file(const std::string& src, const std::string& dest)
	{
#ifdef _WIN32
		CopyFile(src.data(), dest.data(), FALSE);
#endif
	}

private:

};

#endif