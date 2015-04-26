#include "container_handle.h"
#include "FileSystem.h"
#include "number_helper.h"

#include <sstream>
#include <algorithm>
#include <mtl/fromtorange.h>


container_handle::container_handle(const std::string& content)
{
	if (content != "")
	{
		container_file_.load(content.data());
	}
}

container_handle::container_handle(container_handle& other)
{
	location_ = other.location_;
	open(other.str());
}

void container_handle::create(container_node& node)
{
	auto root_ = container_file_.append_child("container");
	root_.append_attribute("name") = node.name.data();
	root_.append_attribute("version") = node.version.data();
	root_.append_attribute("compressed") = node.compressed ? "true" : "false";
}
void container_handle::create(container_node& container_node, std::vector<location_node>& loc_nodes)
{
	create(container_node);
	add_locations_node(loc_nodes);
}
void container_handle::create(container_node& container_node, std::vector<std::pair<std::string, size_t>> locations)
{
	create(container_node);
	add_locations_node(locations);
}
void container_handle::open(const std::string& content)
{
	if (content != "")
	{
		container_file_.load(content.data());
	}
}

bool container_handle::is_compressed()
{
	std::lock_guard<std::mutex> guard(mtx_);
	return std::string(container_file_.child("container").attribute("compressed").value()) == "true";
}
std::string container_handle::get_version()
{
	std::lock_guard<std::mutex> guard(mtx_);
	return container_file_.child("container").attribute("version").value();
}
std::string container_handle::get_containername()
{
	std::lock_guard<std::mutex> guard(mtx_);
	return container_file_.child("container").attribute("name").value();
}

container_handle::~container_handle()
{

}

container_handle::file_node container_handle::get_filenode(const path& relative_path)
{
	std::lock_guard<std::mutex> guard(mtx_);

	std::string filename = relative_path.get_filename();
	path folder(relative_path.get_folderpath_unsecure());

	std::string xpath_query("/container/file[@filename = \"" + filename + "\"]");
	pugi::xpath_node_set possible = container_file_.select_nodes(xpath_query.data());

	file_node node;
	node.size = 0;

	for (auto& e : possible)
	{
		path p(e.node().child("path").child_value());
		if (p == folder)
		{
			
			node.filename = e.node().attribute("filename").value();
			node.size = s_to_int64(e.node().child("size").child_value());//std::stoi(e.node().child("size").child_value());
			node.p = p;
			node.crc = e.node().child("crc").child_value();
			node.rev = s_to_int64(e.node().child("rev").child_value());

			auto blocksnode = e.node().child("blocks");
			for (pugi::xml_node block = blocksnode.child("block"); block; 
				block = block.next_sibling("block"))
			{
				block_node blocknode;
				blocknode.id = std::stoi(block.attribute("id").value());
				blocknode.location = block.attribute("location").value();
				blocknode.crc = block.attribute("crc").value();
				blocknode.filename = block.attribute("filename").value();
				node.blocks.push_back(blocknode);
			}
			if (node.size >= 0)
				break;
			else
				node = file_node{};
		}
	}

	return node;
}
std::string container_handle::get_filename(const path& relative_path)
{
	return get_filenode(relative_path).filename;
}
int64_t container_handle::get_size(const path& relative_path)
{
	return get_filenode(relative_path).size;
}
path container_handle::get_path(const path& relative_path)
{
	return get_filenode(relative_path).p;
}
std::string container_handle::get_crc(const path& relative_path) 
{
	return get_filenode(relative_path).crc;
}

size_t container_handle::get_block_count(const path& relative_path)
{
	return get_filenode(relative_path).blocks.size();
}
std::vector<container_handle::block_node> container_handle::get_blocks(const path& relative_path)
{
	return get_filenode(relative_path).blocks;
}
container_handle::block_node container_handle::get_block(const path& relative_path, size_t block_id)
{
	return get_filenode(relative_path).blocks[block_id-1];
}
std::string container_handle::get_block_location(const path& relative_path, size_t block_id)
{
	return get_filenode(relative_path).blocks[block_id-1].location;
}
std::string container_handle::get_block_crc(const path& relative_path, size_t block_id)
{
	return get_filenode(relative_path).blocks[block_id-1].crc;
}
std::string container_handle::get_block_filename(const path& relative_path, size_t block_id)
{
	return get_filenode(relative_path).blocks[block_id-1].filename;
}

path container_handle::get_location_path(const std::string& location)
{
	std::lock_guard<std::mutex> guard(mtx_);
	std::string query("/container/locations/location[@name = \"" + location + "\" ]");
	pugi::xpath_node_set nodelocation = container_file_.select_nodes(query.data());
	if (nodelocation.empty()) { return path(""); }
	return path((*nodelocation.begin()).node().attribute("path").value());
}

void container_handle::add_file_node(const file_node& node)
{
	
	std::lock_guard<std::mutex> guard(mtx_);

	std::string xpath_q("/container/file[@filename = \"" + node.filename + "\" and path = \"" + node.p.raw_std_str() + "\" ]");
	pugi::xpath_node_set nodes_found = container_file_.select_nodes(xpath_q.data());

	auto root = container_file_.child("container");
	std::string rev_to_add("0");

	if (nodes_found.size() > 0)
	{
		std::string revstr(nodes_found[0].node().child("rev").child_value());
		auto rev = s_to_int64(revstr) + 1;
		std::stringstream ss;
		ss << rev;
		rev_to_add = ss.str();
		root.remove_child(nodes_found[0].node());
	}

	auto filenode = root.append_child("file");
	std::stringstream ss;

	filenode.append_attribute("filename") = node.filename.data();
	ss << node.size;
	filenode.append_child("size").append_child(
		pugi::node_pcdata).set_value(ss.str().data());
	ss.str(std::string());
	filenode.append_child("path").append_child(
		pugi::node_pcdata).set_value(node.p.std_str().data());
	filenode.append_child("crc").append_child(
		pugi::node_pcdata).set_value(node.crc.data());
	filenode.append_child("rev").append_child(
		pugi::node_pcdata).set_value(rev_to_add.c_str());

	auto blocksnode = filenode.append_child("blocks");
	for (auto& e : node.blocks)
	{
		auto blocknode = blocksnode.append_child("block");
		ss << e.id;
		blocknode.append_attribute("id") = ss.str().data();
		ss.str(std::string());
		blocknode.append_attribute("location") = e.location.data();
		blocknode.append_attribute("crc") = e.crc.data();
		blocknode.append_attribute("filename") = e.filename.data();
	}		
}
void container_handle::add_block_node(block_node& blocknode, file_node& filenode)
{
	std::lock_guard<std::mutex> guard(mtx_);
	filenode.blocks.push_back(blocknode);
}

void container_handle::update_file_node(const file_node& node)
{
	std::string filename = node.filename;
	path folder = node.p;

	path relative_path(node.p);
	relative_path = relative_path.append_filename(node.filename);
	file_node old_node = get_filenode(relative_path);

	std::lock_guard<std::mutex> guard(mtx_);

	std::string xpath_query("/container/file[@filename = \"" + filename + "\"]");
	pugi::xpath_node_set possible = container_file_.select_nodes(xpath_query.data());

	std::stringstream ss;

	for (auto& e : possible)
	{
		path p(e.node().child("path").child_value());
		if (p == folder)
		{
			if (node.size != old_node.size/* && old_node.size != 0 && old_node.size != -1*/)
			{
				ss << node.size;
				e.node().remove_child("size");
				auto size_node = e.node().insert_child_before("size",
					e.node().child("path"));
				size_node.append_child(pugi::node_pcdata).set_value(ss.str().data());
				ss.str(std::string());
			}

			if (node.crc != old_node.crc)
			{
				e.node().remove_child("crc");
				auto crc_node = e.node().insert_child_after("crc",
					e.node().child("path"));
				crc_node.append_child(pugi::node_pcdata).set_value(node.crc.data());

				//update is sure so update rev
				auto revnum = node.rev + 1;
				std::stringstream ssrev;
				ssrev << revnum;
				e.node().remove_child("rev");
				auto rev_node = e.node().insert_child_after("rev", e.node().child("crc"));
				rev_node.append_child(pugi::node_pcdata).set_value(ssrev.str().data());
			}

			for (auto& x : node.blocks)
			{
				auto it = std::find_if(old_node.blocks.begin(), old_node.blocks.end(),
					[&](block_node& old_block)
				{
					return old_block.filename == x.filename;
				});

				auto iti = std::end(node.blocks);
				
				if (it != std::end(old_node.blocks) && (*it).crc != x.crc)
				{
					auto blocks_node = e.node().child("blocks");
					std::string query("./block[@filename = \"" + x.filename
						+ "\"]");
					pugi::xpath_node block = blocks_node.select_single_node(query.data());
					block.node().remove_attribute("crc");
					block.node().insert_attribute_after("crc",
						block.node().attribute("location")) = x.crc.data();
				}
			}
		}
	}
}
void container_handle::update_file_size(int64_t size, const path& relative_path)
{
	file_node node = get_filenode(relative_path);
	node.size = size;
	update_file_node(node);
}
void container_handle::update_file_path(const path& location, const path& relative_path)
{
	file_node node = get_filenode(relative_path);
	node.p = location;
	update_file_node(node);
}
void container_handle::update_file_crc(const std::string& crc, const path& relative_path)
{
	file_node node = get_filenode(relative_path);
	node.crc = crc;
	update_file_node(node);
}

void container_handle::update_block_crc(const std::string& crc, const path& relative_path, int64_t block_id)
{
	file_node node = get_filenode(relative_path);
	node.blocks[block_id-1].crc = crc;
	update_file_node(node);
}

std::string container_handle::str()
{
	std::lock_guard<std::mutex> guard(mtx_);
	std::stringstream ss;
	container_file_.save(ss);
	std::string xmlcode = ss.str();
	return xmlcode;
}

pugi::xml_document& container_handle::data()
{
	return container_file_;
}

void container_handle::add_locations_node(std::vector<std::pair<std::string, size_t>> locations)
{
	std::lock_guard<std::mutex> guard(mtx_);
	auto root_ = container_file_.child("container");

	auto locationsnode = root_.append_child("locations");
	for (auto& e : locations)
	{
		auto locationnode = locationsnode.append_child("location");
		auto s = e.first.find("$");
		std::string name("");
		for (auto i = s; i < e.first.size(); ++i)
		{
			if (e.first[i] == '$')
				continue;
			else if (e.first[i] == FileSystem::path_separator)
				break;
			else
				name += e.first[i];
		}

		path location_path(e.first);
		locationnode.append_attribute("name") = name.data();
//		location_path.create_folderpath();
		FileSystem::make_folders(location_path.str());
		locationnode.append_attribute("path") = FileSystem::standardize_path(location_path.raw_str()).data();
		std::stringstream ss;
		ss << e.second;
		locationnode.append_attribute("quota") = ss.str().data();
		locationnode.append_attribute("used") = "0";
	}
}
void container_handle::add_locations_node(std::vector<location_node>& loc_nodes)
{
	std::lock_guard<std::mutex> guard(mtx_);

	auto root_ = container_file_.child("container");

	auto locationsnode = root_.append_child("locations");

	for (auto& e : loc_nodes)
	{
		auto locationnode = locationsnode.append_child("location");
		locationnode.append_attribute("name") = e.name.data();
		e.location.create_folderpath();
		locationnode.append_attribute("path") = e.location.raw_std_str().data();
		std::stringstream ss;
		ss << e.quota;
		locationnode.append_attribute("quota") = ss.str().data();
		ss.str(std::string());
		ss << e.used;
		locationsnode.append_attribute("used") = ss.str().data();
	}
}
bool container_handle::update_memusage_location(const std::string& locationname, int64_t size, bool is_locked)
{	
	if (has_space_location(locationname, size))
	{
		std::string xpath_q("/container/locations/location[@name = \"" + locationname + "\" ]");
		pugi::xpath_node_set locations = container_file_.select_nodes(xpath_q.data());
		int64_t used = s_to_int64((*locations.begin()).node().attribute("used").value());
		int64_t available = s_to_int64((*locations.begin()).node().attribute("quota").value());

		used += size;
		((*locations.begin()).node().remove_attribute("used"));
		std::stringstream ss;
		ss << used;
		(*locations.begin()).node().append_attribute("used") = ss.str().data();
	}

	return true;
}
bool container_handle::has_space_location(const std::string& locationname, int64_t size)
{
	//std::lock_guard<std::mutex> guard(mtx_);
	std::string xpath_q("/container/locations/location[@name = \"" + locationname + "\" ]");
	pugi::xpath_node_set locations = container_file_.select_nodes(xpath_q.data());

	if (locations.size() == 1)
	{
		int64_t used = s_to_int64((*locations.begin()).node().attribute("used").value());
		int64_t available = s_to_int64((*locations.begin()).node().attribute("quota").value());
		
		return (available > (used + size));
	}
	return false;
}

size_t container_handle::count_locations()
{
	//std::lock_guard<std::mutex> guard(mtx_);
	auto locs = container_file_.select_nodes("/container/locations/location");
	return locs.size();
}

std::vector<container_handle::location_node> container_handle::get_locations()
{
	std::lock_guard<std::mutex> guard(mtx_);
	auto locs = container_file_.select_nodes("/container/locations/location");
#ifdef TDEBUG
	container_file_.save_file("C:\\Users\\Kai\\Desktop\\containerfiledump.xml");
#endif
	std::vector<location_node> tmp(locs.size());
	for (size_t idx = 0; idx < locs.size(); ++idx)
	{
		tmp[idx].location = locs[idx].node().attribute("path").value();
		tmp[idx].name = locs[idx].node().attribute("name").value();
		std::string s(locs[idx].node().attribute("quota").value());
		tmp[idx].quota = s_to_int64(s);//atol(locs[idx].node().attribute("quota").value());
		std::string s2(locs[idx].node().attribute("used").value());
		tmp[idx].used = s_to_int64(s2);//atol(locs[idx].node().attribute("used").value());
	}
	return tmp;
}

bool container_handle::where_to_store(int64_t size, path& store_path)
{
	std::vector<container_handle::location_node> locations = get_locations();

	std::lock_guard<std::mutex> guard(mtx_);

	static std::vector<size_t> visited;
	if (visited.size() == count_locations()) { visited.clear(); }

	for (auto idx = 0; idx < locations.size(); ++idx)
	{
		if (std::find(visited.begin(), visited.end(), idx) != visited.end()) { continue; }

		if ((locations[idx].used + size) < locations[idx].quota)
		{
			update_memusage_location(locations[idx].name, size, true);
			store_path = locations[idx].location;
			visited.push_back(idx);
			return true;
		}
	}
	return false;
}

void container_handle::dump(const path& dest)
{
	std::lock_guard<std::mutex> guard(mtx_);
	container_file_.save_file(dest.str().data());
}

void container_handle::delete_filenode(const path& relative_path)
{
	auto node = get_filenode(relative_path);
	auto rev = node.rev + 1;

	std::stringstream ssrev;
	ssrev << rev;

	std::string xpath_q("/container/file[@filename = \"" + node.filename + "\" and path = \"" + node.p.raw_std_str() + "\" ]");
	pugi::xpath_node_set nodes_found = container_file_.select_nodes(xpath_q.data());

	if (nodes_found.size() > 0)
	{
		nodes_found[0].node().remove_child("rev");
		auto revnode = nodes_found[0].node().insert_child_after("rev", nodes_found[0].node().child("crc"));
		revnode.append_child(pugi::node_pcdata).set_value(ssrev.str().c_str());
	}

	auto locations = get_locations();
	auto locationname = locations[0].name;
	auto s = node.size;
	update_file_size(-1, relative_path);
	update_memusage_location(locationname, -s);

	/*{
		std::lock_guard<std::mutex> guard(mtx_);
		std::string filename(relative_path.get_filename());
		std::string folder(relative_path.get_folderpath_unsecure());

		std::string xpath_query("/container/file[@filename = \"" + filename + "\"]");
		pugi::xpath_node_set possible = container_file_.select_nodes(xpath_query.data());

		for (auto& e : possible)
		{
			std::string p(e.node().child("path").child_value());
			if (p == folder)
			{
				auto blocknode = e.node().child("blocks").child("block");
				std::string hashname(blocknode.attribute("filename").value());
				std::string location(blocknode.attribute("location").value());

				path location_path(location, container_file_, e.node());

				if (location_path.valid())
				{
					path store_path(location_path.str() + FileSystem::path_separator + hashname);
					store_path.delete_file();

					int64_t size = std::stoi(e.node().child("size").child_value());
					update_memusage_location(location, -size, true);

					container_file_.child("container").remove_child(e.node());
					break;
				}
			}
		}
	}*/
	
}

std::vector<container_handle::file_node> container_handle::get_filenodes()
{
	pugi::xpath_node_set list_of_files;
	
	{
		std::lock_guard<std::mutex> guard(mtx_);
		list_of_files = container_file_.select_nodes("/container/file[size != -1]");
	}
	//std::vector<file_node> files(list_of_files.size());
	std::vector<file_node> files;

	for (auto idx : mtl::count_until(list_of_files.size()))
	{
		std::string filename = list_of_files[idx].node().attribute("filename").value();
		path location(list_of_files[idx].node().child("path").child_value());
		location = location.append_filename(filename);
		auto node = get_filenode(location);
		if (node.size > 0)
			files.push_back(node);
	}

	/*for (size_t idx = 0; idx < files.size(); ++idx)
	{
		std::string filename = list_of_files[idx].node().attribute("filename").value();
		path location(list_of_files[idx].node().child("path").child_value());
		location = location.append_filename(filename);
		files[idx] = get_filenode(location);
	}*/
	return files;
}

std::vector<container_handle::file_node> container_handle::get_deleted_filenodes()
{
	auto all_nodes = get_all_filenodes();
	std::vector<file_node> files;
	for (auto n : all_nodes)
	{
		if (n.size == -1)
			files.push_back(n);
	}
	return files;
}

std::vector<container_handle::file_node> container_handle::get_all_filenodes()
{
	std::lock_guard<std::mutex> guard(mtx_);
	pugi::xpath_node_set list_of_files;
	list_of_files = container_file_.select_nodes("/container/file");
	std::vector<file_node> files;

	for (auto idx : mtl::count_until(list_of_files.size()))
	{
		std::string filename2 = list_of_files[idx].node().attribute("filename").value();
		path location(list_of_files[idx].node().child("path").child_value());
		location = location.append_filename(filename2);

		std::string filename = location.get_filename();
		path folder(location.get_folderpath_unsecure());

		std::string xpath_query("/container/file[@filename = \"" + filename + "\"]");
		pugi::xpath_node_set possible = container_file_.select_nodes(xpath_query.data());

		file_node node;
		node.size = 0;

		for (auto& e : possible)
		{
			path p(e.node().child("path").child_value());
			if (p == folder)
			{

				node.filename = e.node().attribute("filename").value();
				node.size = s_to_int64(e.node().child("size").child_value());//std::stoi(e.node().child("size").child_value());
				node.p = p;
				node.crc = e.node().child("crc").child_value();
				node.rev = s_to_int64(e.node().child("rev").child_value());

				auto blocksnode = e.node().child("blocks");
				for (pugi::xml_node block = blocksnode.child("block"); block;
					block = block.next_sibling("block"))
				{
					block_node blocknode;
					blocknode.id = std::stoi(block.attribute("id").value());
					blocknode.location = block.attribute("location").value();
					blocknode.crc = block.attribute("crc").value();
					blocknode.filename = block.attribute("filename").value();
					node.blocks.push_back(blocknode);
				}
				break;
			}
		}
		files.push_back(node);
	}
	return files;
}