#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <mutex>
//#include "../lib/pugixml/src/pugixml.hpp"
#include "pugixml.hpp"
#include "Path.h"

class container_handle
{
public:
	struct block_node
	{
		size_t id;
		std::string location;
		std::string crc;
		std::string filename;
	};

	struct file_node
	{
		file_node() : filename(""), p(""), size(0), crc(""), rev(0)
		{}
		std::string filename;
		path p;//ath;
		int64_t size;
		std::string crc;	
		std::vector<block_node> blocks;
		int64_t rev;
	};

	struct location_node
	{
		std::string name;
		path location;
		int64_t quota;
		int64_t used;
	};

	struct container_node
	{
		std::string name;
		std::string version;
		bool compressed;
	};

	container_handle(const std::string& content = "");
	container_handle(container_handle& other);
	~container_handle();

	void create(container_node& node);
	void create(container_node& container_node, std::vector<location_node>& loc_node);
	void create(container_node& container_node, std::vector<std::pair<std::string, size_t>> locations);
	void open(const std::string& content);

	bool is_compressed();
	std::string get_version();
	std::string get_containername();

	file_node get_filenode(const path& relative_path);
	std::vector<file_node> get_filenodes();
	std::string get_filename(const path& relative_path);
	int64_t get_size(const path& relative_path);
	path get_path(const path& relative_path);
	std::string get_crc(const path& relative_path);

	size_t get_block_count(const path& relative_path);
	std::vector<block_node> get_blocks(const path& relative_path);
	block_node get_block(const path& relative_path, size_t block_id);
	std::string get_block_location(const path& relative_path, size_t block_id);
	std::string get_block_crc(const path& relative_path, size_t block_id);
	std::string get_block_filename(const path& relative_path, size_t block_id);

	path get_location_path(const std::string& location);

	void add_file_node(const file_node& node);
	void add_block_node(block_node& blocknode, file_node& filenode);
	void add_locations_node(std::vector<std::pair<std::string, size_t>> locations);
	void add_locations_node(std::vector<location_node>& loc_node);
	bool update_memusage_location(const std::string& locationname, int64_t size, bool is_locked = false);
	bool has_space_location(const std::string& locationname, int64_t size);
	size_t count_locations();
	std::vector<location_node> get_locations();
	bool where_to_store(int64_t size, path& store_path);

	void update_file_node(const file_node& node);
	void update_file_size(int64_t size, const path& relative_path);
	void update_file_path(const path& location, const path& relative_path);
	void update_file_crc(const std::string& crc, const path& relative_path);

	void update_block_crc(const std::string& crc, const path& relative_path, int64_t block_id);
	void update_block_filename(const std::string& filename, const path& relative_path, int64_t block_id);

	std::string str();
	pugi::xml_document& data();
	void dump(const path& dest);

	void delete_filenode(const path& relative_path);
	std::vector<file_node> get_deleted_filenodes();
	std::vector<file_node> get_all_filenodes();

private:
	pugi::xml_document container_file_;
	path location_;

	std::mutex mtx_;
};

