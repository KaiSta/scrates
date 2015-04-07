#pragma once

#include "FileSystem.h"
#include "CloudManager.h"
//#include "../lib/pugixml/src/pugixml.hpp"
#include "pugixml.hpp"
#include <string>
#include <vector>

/**
  \brief Abstraction for path handling.
*/
class path
{
public:
	path(std::string p = "") : c_manager_(CloudManager::instance())
	{
		path_ = p;
	}

	/**
	  \brief DEPRECATED
	*/
	path(std::string location_name, const pugi::xml_document& container_file, const pugi::xml_node& filenode) : c_manager_(CloudManager::instance())
	{
		auto blocknode = filenode.child("blocks").child("block");
		std::string query("/container/locations/location[@name = \"" + location_name + "\" ]");
		pugi::xpath_node_set nodelocation = container_file.select_nodes(query.data());
		if (nodelocation.empty()) { path_ = ""; return; }
		path_ = FileSystem::path_to_systemstandard(c_manager_.extent(std::string((*nodelocation.begin()).node().attribute("path").value())));
	}

	path(const path& p) : c_manager_(CloudManager::instance())
	{
		path_ = p.path_;
	}

	path& operator=(const path& p)
	{
		path_ = p.path_;
		return *this;
	}

	path& operator=(const std::string& p)
	{
		path_ = p;
		return *this;
	}

	bool operator==(const path& other)
	{
		return std_str() == other.std_str();
	}

	/**
	  \brief Returns the stored path in system specific format with expansion.
	*/
	std::string str() const
	{
		return FileSystem::path_to_systemstandard(c_manager_.extent(path_));
	}

	/**
	  \brief Returns the stored path in unix standard with expansion.
	*/
	std::string std_str() const
	{
		return FileSystem::standardize_path(c_manager_.extent(path_));
	}

	/**
	  \brief Returns the path as stored.
	*/
	std::string raw_str() const
	{
		return path_;
	}

	/**
	  \brief Returns stored path standardized.
	*/
	std::string raw_std_str() const
	{
		return FileSystem::standardize_path(raw_str());
	}

	std::string get_filename() const
	{
		return FileSystem::name_from_path(str());
	}

	std::string get_folderpath() const
	{
		return FileSystem::get_folderpath(str());
	}

	std::string get_folderpath_unsecure() const
	{
		return FileSystem::get_folderpath_unsecure(str());
	}

	void create_folderpath() const
	{
		FileSystem::make_folders(get_folderpath());
	}

	void delete_file() const
	{
		FileSystem::delete_file(str());
	}

	void delete_directory() const
	{
		FileSystem::delete_directory(str());
	}

	path append_filename(const std::string& filename) const
	{
		return FileSystem::join_path_filename(str(), filename);
	}

	bool valid() const
	{
		return path_ != "";
	}

private:
	std::string path_;
	CloudManager& c_manager_;
};