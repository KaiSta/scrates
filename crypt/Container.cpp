#include "Container.h"
#include <cryptopp\cryptlib.h>
#include <cryptopp\sha.h>
#include <cryptopp\files.h>
#include <cryptopp\filters.h>
#include <cryptopp\pwdbased.h>
#include <cryptopp\modes.h>
#include <cryptopp\crc.h>
#include "FileSystem.h"
#include <sstream>
#include "CloudManager.h"
#include <cryptopp\hex.h>
#include "helper_files.h"
#include <mtl/fromtorange.h>

Container::Container(encryption_algorithm algo) : algo_(algo), volume_(nullptr), seed_(16), is_syncing(false), container_open_(false)
{

}

Container::Container(VirtualDisk_Impl::volume_handle* volume, encryption_algorithm algo) : algo_(algo),
volume_(volume), seed_(16), is_syncing(false), container_open_(false)
{
}

Container::Container(const path& location, const std::string& pw, VirtualDisk_Impl::volume_handle* volume, encryption_algorithm algo) : volume_(volume), 
passphrase_(pw.begin(), pw.end()), seed_(16), is_syncing(false), container_open_(false)
{
	using namespace CryptoPP;
	StringSource(pw.data(), true, new HashFilter(SHA256(), new StringSink(pw_)));
	
	container_name_ = location.get_filename();
	container_name_.erase(container_name_.find_first_of('.'), container_name_.size());
	path_ = location;
	location.create_folderpath();
	algo_ = algo;
}

void Container::set_vhd(VirtualDisk_Impl::volume_handle* v)
{
	volume_ = v;
}

void Container::set_seed(CryptoPP::SecByteBlock seed)
{
	seed_ = seed;
	CryptoPP::AutoSeededRandomPool prng;
	CryptoPP::SecByteBlock t(seed.size());
	prng.GenerateBlock(t, t.size());
	for (auto p : mtl::count_until(t.size()))
	{
		seed_[p] ^= t[p];
	}
	prng_.IncorporateEntropy(seed_, seed_.size());
}

Container::~Container()
{
	close();
}

void Container::init(const path& location, const std::string& pw, encryption_algorithm algo)
{
	using namespace CryptoPP;
	StringSource(pw.data(), true, new HashFilter(SHA256(), new StringSink(pw_)));
	passphrase_.assign(pw.begin(), pw.end());
	container_name_ = location.get_filename();
	container_name_.erase(container_name_.find_first_of('.'), container_name_.size());
	path_ = location;
	location.create_folderpath();
	algo_ = algo;
}

void Container::update_containerCrC()
{
	path clouddbloc(handle_.get_locations()[0].location.str());
	clouddbloc = clouddbloc.append_filename(container_name_ + ".db");
	containercrc_ = "";
	secure_crc(clouddbloc.str(), containercrc_);
}

void Container::create(std::vector<std::pair<std::string, size_t>> locations)
{
	if (!FileSystem::file_exists(path_.str()))
	{
		switch (algo_)
		{
		case AES:
			helper_create<CryptoPP::AES>();
			break;
		case SERPENT:
			helper_create<CryptoPP::Serpent>();
			break;
		case TWOFISH:
			helper_create<CryptoPP::Twofish>();
			break;
		default:
			return;
		}

		handle_.add_locations_node(locations);
		path p(locations[0].first);

		syncer.set_vhdlocation((*volume_).drive_letter + handle_.get_containername());
		syncer.set_cloudlocation(handle_.get_locations()[0].location.str());
		syncer.add_listener([&](std::vector<Synchronizer::file_item>)
		{
			sync();
		}, Synchronizer::CLOUD);
		syncer.add_listener([&](std::vector<Synchronizer::file_item>)
		{
			manual_sync();
		}, Synchronizer::VHD);
		syncer.init();
		
		update_containerCrC();
		container_open_ = true;
#ifdef TDEBUG
//		handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
	}
	else
		open();
}

void Container::open()
{
	switch (algo_)
	{
	case AES:
		helper_open<CryptoPP::AES>();
		break;
	case SERPENT:
		helper_open<CryptoPP::Serpent>();
		break;
	case TWOFISH:
		helper_open<CryptoPP::Twofish>();
		break;
	default:
		return;
	}

	handle_.open(container_raw_);
	if (!validate())
	{
		throw(std::logic_error("severe: synchronization with cloud incomplete!"));
	}
	auto tmp = handle_.get_locations();
	path p(tmp[0].location);

	extract_all();

	syncer.set_vhdlocation((*volume_).drive_letter + handle_.get_containername());
	syncer.set_cloudlocation(handle_.get_locations()[0].location.str());
	syncer.add_listener([&](std::vector<Synchronizer::file_item>)
	{
		sync();
	}, Synchronizer::CLOUD);
	syncer.add_listener([&](std::vector<Synchronizer::file_item>)
	{
		manual_sync();
	}, Synchronizer::VHD);
	syncer.init();

	update_containerCrC();
	container_open_ = true;
#ifdef TDEBUG
	handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
}

void Container::save()
{
	std::lock_guard<std::mutex> guard(wr_mtx_);

	if (!container_open_)
		return;
	std::string oldcrc;
	CryptoPP::StringSource(container_raw_, true, new CryptoPP::HashFilter(CryptoPP::CRC32(),
		new CryptoPP::HexEncoder(new CryptoPP::StringSink(oldcrc))));
	std::string ncrc;
	CryptoPP::StringSource(handle_.str(), true, new CryptoPP::HashFilter(CryptoPP::CRC32(),
		new CryptoPP::HexEncoder(new CryptoPP::StringSink(ncrc))));

	if (oldcrc == ncrc)
	{
		return;
	}

	switch (algo_)
	{
	case AES:
		helper_save<CryptoPP::AES>();
		break;
	case SERPENT:
		helper_save<CryptoPP::Serpent>();
		break;
	case TWOFISH:
		helper_save<CryptoPP::Twofish>();
		break;
	default:
		return;
	}
	update_containerCrC();
	container_raw_ = handle_.str();

#ifdef TDEBUG
	handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
}
//vhd update
void Container::manual_sync()
{
	std::lock_guard<std::mutex> syncguard(sync_lock_);

	if (!validate())
	{
		//throw(std::logic_error("severe: synchronization with cloud incomplete!"));
		std::cout << "severe: synchronization with cloud incomplete!\n";
		return;
	}

	std::vector<std::string> tmp;
	std::vector<container_handle::file_node> fnodes;
	{
		std::lock_guard<std::mutex> guard(wr_mtx_);

		std::string strpath((*volume_).drive_letter + handle_.get_containername());
		path p(strpath);
		tmp = FileSystem::list_files(p.str(), true);
		fnodes = handle_.get_filenodes();
	}

/*	for (auto fnode : fnodes)
	{
<<<<<<< HEAD
		if (FileSystem::file_exists(f) && FileSystem::file_size(f) > 0)
			add_file(f, handle_.get_containername());
	}
=======
		path p(fnode.path);
		p = p.append_filename(fnode.filename);
		path containerpath((*volume_).drive_letter + handle_.get_containername());
		std::string filepath(containerpath.str() + FileSystem::path_separator + p.str());
		
		if (!FileSystem::file_exists(filepath))
		{
			//quick fix container_handle needs a way to give store path for existing files!!!
			path tmp;
			handle_.where_to_store(0, tmp);
			tmp = tmp.append_filename(fnode.blocks[0].filename);
			if(FileSystem::delete_file(tmp.str()))
				handle_.update_file_size(-1, p);
		}
	}
	
>>>>>>> origin/master*/
	//#pragma omp parallel for
	for (int i = 0; i < tmp.size(); ++i)
	{
		add_file(tmp[i], handle_.get_containername());
	}

	{
		std::lock_guard<std::mutex> guard2(wr_mtx_);
		update_containerCrC();
#ifdef TDEBUG
		handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
	}
	//}
	save();
}
//cloud update
void Container::sync()
{ //needs merge of old and new handle
	std::lock_guard<std::mutex> syncguard(sync_lock_);
	{
		std::lock_guard<std::mutex> guard(wr_mtx_);

		//check if external sync is needed
		std::string oldcrc = containercrc_;
		update_containerCrC();
		if (oldcrc == containercrc_)
			return;

		auto oldhandle = handle_;
		container_raw_ = "";
		switch (algo_)
		{
		case AES:
			helper_open<CryptoPP::AES>();
			break;
		case SERPENT:
			helper_open<CryptoPP::Serpent>();
			break;
		case TWOFISH:
			helper_open<CryptoPP::Twofish>();
			break;
		default:
			return;
		}
		handle_.open(container_raw_);

		auto oldnodes = oldhandle.get_filenodes();

		for (auto& node : oldnodes)
		{
			path tmp(node.path);
			tmp = tmp.append_filename(node.filename);
			auto xnode = handle_.get_filenode(tmp);

			if (xnode.size == 0) // old handle contains nodes that aren't in the new one.
			{
				handle_.add_file_node(node);
				//files should be already locally extracted, no further steps?
			}
			else
			{
				if (node.crc != xnode.crc) // both contain a conflicting node
				{
					//merge newer one into new handle.
					if (node.rev > xnode.rev)
						handle_.update_file_node(node);					
				}
			}
		}

		auto newnodes = handle_.get_filenodes();

		for (auto& node : newnodes)
		{
			path tmp(node.path);
			tmp = tmp.append_filename(node.filename);

			auto oldnode = oldhandle.get_filenode(tmp);
			if (node.crc != oldnode.crc)
			{
				extract_file(tmp);
			}
		}
		update_containerCrC();
#ifdef TDEBUG
		handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
	}
	//save();
	
}
void Container::close()
{
	if (!container_open_)
		return;
	syncer.stop();
	sync();
	save();
	std::string p((*volume_).drive_letter);
	p.append(handle_.get_containername());
	FileSystem::delete_all(p);
}

void Container::add_file(const path& src, const std::string& rootfolder)
{
	if (!validate())
	{
		//throw(std::logic_error("severe: synchronization with cloud incomplete!"));
		std::cout << "severe: synchronization with cloud incomplete!\n";
		return;
	}
	std::string location = src.str();
	std::string file_name = src.get_filename();

	using namespace CryptoPP;
	std::string crc;
	if (!secure_crc(location, crc))
	{
		return;
	}

	auto folderstart = location.find(rootfolder);
	auto folderend = location.find(src.get_filename());
	path relativepath(std::string(location.begin() + folderstart, location.begin() + (folderend - 1)));
	path tmp = relativepath.append_filename(file_name);

	container_handle::file_node fnode = handle_.get_filenode(tmp);
	if (fnode.path == relativepath)
	{
		update_file(src.get_filename(), relativepath, src);
		return;
	}

	if (FileSystem::file_exists(location))
	{		
		auto size = FileSystem::file_size(location);
		if (size >= 0)
		{
			path store_path;
			if (!handle_.where_to_store(size, store_path)) { return; }
			CloudManager& manager = CloudManager::instance();
			std::string location_name = manager.get_location_name(store_path.raw_str());
			container_handle::file_node file;
			file.filename = src.get_filename();
			file.size = size;
			file.path = relativepath.std_str();

			
			//FileSource(location.data(), true, new HashFilter(CRC32(), new HexEncoder(new StringSink(crc))));
			file.crc = crc;

			std::string key;
			std::string iv;
			std::string hashname;
			bool ret = false;

			//file verschluesseln hier
			switch (algo_)
			{
			case AES:
				ret = helper_add_file<CryptoPP::AES>(src, store_path, iv, hashname);
				break;
			case SERPENT:
				ret = helper_add_file<CryptoPP::Serpent>(src, store_path, iv, hashname);
				break;
			case TWOFISH:
				ret = helper_add_file<CryptoPP::Twofish>(src, store_path, iv, hashname);
				break;
			default:
				return;
			}

			if (!ret)
			{
				handle_.update_memusage_location(location_name, -size);
				return;
			}

			//file.key = key;
			//file.iv = iv;
			container_handle::block_node block_node;
			block_node.id = 1;
			block_node.location = location_name;
			block_node.crc = crc;
			block_node.filename = hashname;
			file.blocks.push_back(block_node);
			handle_.add_file_node(file);

			/*list_of_files::item new_item;
			new_item.name = hashname;
			new_item.crc = crc;
			new_item.size = size;
			new_item.location = location_name;
			new_item.relative_path = tmp;
			lof_.add_file(new_item);*/

			save();
		}

	}
#ifdef TDEBUG
	handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
	//lof_.dump();
#endif
}
void Container::open_file(const std::string& relative_path)
{

}

void Container::delete_file(const path& relative_path)
{
	if (!validate())
	{
		//throw(std::logic_error("severe: synchronization with cloud incomplete!"));
		std::cout << "severe: synchronization with cloud incomplete!\n";
		return;
	}
	auto x = handle_.get_filenode(relative_path);
	list_of_files::item remove_item;
	remove_item.name = x.blocks[0].filename;
	//lof_.delete_file(remove_item);
	handle_.delete_filenode(relative_path);
	save();
#ifdef TDEBUG
	handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
}

void Container::extract_file(const path& relative_path/*, const path& dest*/)
{
	if (file_exists(relative_path.str()))
	{
		std::string location(relative_path.str());
		std::string filename(relative_path.get_filename());
		std::string folder(relative_path.get_folderpath_unsecure());

		container_handle::file_node fnode = handle_.get_filenode(relative_path);

		if (fnode.path == folder)
		{
			switch (algo_)
			{
			case AES:
				helper_extract_file<CryptoPP::AES>(fnode, (*volume_).drive_letter);
				break;
			case SERPENT:
				helper_extract_file<CryptoPP::Serpent>(fnode, (*volume_).drive_letter);
				break;
			case TWOFISH:
				helper_extract_file<CryptoPP::Twofish>(fnode, (*volume_).drive_letter);
				break;
			}
		}

	}
	//THROW
}

void Container::update_file(const std::string& filename, const path& relative_path, const path& src)
{
	{
		std::lock_guard<std::mutex> guard(wr_mtx_);

		if (!validate())
		{
			//throw(std::logic_error("severe: synchronization with cloud incomplete!"));
			std::cout << "severe: synchronization with cloud incomplete!\n";
			return;
		}

		path location = relative_path.append_filename(filename);
		std::string file_name = location.get_filename();
		container_handle::file_node fnode = handle_.get_filenode(location);

		if (!FileSystem::file_exists(src.str()))
		{
			return;
		}

		using namespace CryptoPP;
		std::string crc;
		if (!secure_crc(src.str(), crc))
		{
			return;
		}

		if (fnode.path == relative_path)
		{
			container_handle::block_node block_node = fnode.blocks[0];
			std::string hashname = block_node.filename;
			std::string old_crc = block_node.crc;
			
			//std::string crc;
			//FileSource(src.str().data(), true, new HashFilter(CRC32(), new HexEncoder(new StringSink(crc))));
			if (old_crc == crc) { return; }

			std::string location_name = block_node.location;

			int64_t size = FileSystem::file_size(src.str());
			int64_t old_size = fnode.size;
			if (!handle_.update_memusage_location(location_name, (size - old_size)))
				return;

			path store_path(handle_.get_location_path(location_name));

			if (!store_path.valid())
			{
				//THROW
				return;
			}

			std::string iv;
			bool ret = false;
			switch (algo_)
			{
			case AES:
				ret =helper_update_file<CryptoPP::AES>(fnode, src, store_path, iv);
				break;
			case SERPENT:
				ret = helper_update_file<CryptoPP::Serpent>(fnode, src, store_path, iv);
				break;
			case TWOFISH:
				ret = helper_update_file<CryptoPP::Twofish>(fnode, src, store_path, iv);
				break;
			default:
				return;
			}

			if (!ret)
			{
				handle_.update_memusage_location(location_name, old_size - size);
				return;
			}

			handle_.update_file_crc(crc, location);
			handle_.update_block_crc(crc, location, 1);
			handle_.update_file_size(size, location);
			/*auto file = lof_.get_file(hashname);
			file.crc = crc;
			file.size = size;
			lof_.update_file(file);*/
		}
	}
	save();

#ifdef TDEBUG
	handle_.dump(std::string("C:\\Users\\Kai\\Desktop\\new_test.xml"));
#endif
}

void Container::list_files(std::vector<path>& files)
{
	auto file_nodes = handle_.get_filenodes();
	files.resize(file_nodes.size());
	for (size_t idx = 0; idx < files.size(); ++idx)
	{
		path tmp = file_nodes[idx].path;
		tmp = tmp.append_filename(file_nodes[idx].filename);
		files[idx] = tmp;
	}
}

bool Container::file_exists(const path& relative_path)
{
	std::string folder = relative_path.get_folderpath_unsecure();
	auto fnode = handle_.get_filenode(relative_path);

	return fnode.path == folder;
}

void Container::extract_all()
{
	auto tmp = handle_.get_filenodes();
	std::string dest((*volume_).drive_letter + handle_.get_containername());

	/*for (auto& x : tmp)
	{
		path loc = x.path;
		loc = loc.append_filename(x.filename);
		extract_file(loc);
	}*/
#pragma omp parallel for
	for (int i = 0; i < tmp.size(); ++i)
	{
		path loc = tmp[i].path;
		loc = loc.append_filename(tmp[i].filename);
		extract_file(loc);
	}
}

bool Container::validate()
{
	auto cloudloc = handle_.get_locations()[0].location;
	auto storedblocks = FileSystem::list_files(cloudloc.str(), false);
	auto filenodes = handle_.get_filenodes();

	for (auto& node : filenodes)
	{
		for (auto& b : node.blocks)
		{
			bool found = false;
			auto blockname = b.filename;
			for (size_t idx = 0; idx < storedblocks.size() && !found; ++idx)
			{
				path x(storedblocks[idx]);
				auto fname = x.get_filename();
				if (fname == blockname)
				{
					found = true;
				}
			}
			if (!found)
				return false;
		}
	}
	return true;
}