#pragma once
#include "tempest.h"
#include "FileSystem.h"
#include "pugixml.hpp"
#include "Container.h"
#include "Path.h"
#include "VirtualDisk_Impl.h"

#include "Storage.h"
#include "VirtualDisk_Impl.h"
#include "Folder_Impl.h"

class local_file
{
	typedef std::function<void(container_event)> callback_t;
public:
	enum storage_type
	{
		VHD,
		FOLDER
	};

	local_file();
	~local_file();

	void create(const std::string& containername,
		const std::string& passphrase,
		path& p, std::vector<std::pair<std::string, size_t>> locations,
		path& vhdpath, storage_type stor_type, callback_t c);
	void open(path& p, const std::string& passphrase,
		path& vhdpath, storage_type stor_type, callback_t c);

	CryptoPP::SecByteBlock get_seed();

	void manual_sync();

	path get_cloudpath();

	void close();

	bool is_open()
	{
		return is_open_;
	}

private:
	pugi::xml_document file_;
	path location_;
	std::string containername_;
	std::vector<byte> pw_;
	std::string cloudloc_;

	VirtualDisk_Impl::volume_handle vhd_;
	Container container_;
	CryptoPP::SecByteBlock random_num_;
	std::string passphrase_;
	std::shared_ptr<Storage> storage_;

	bool is_open_;

};

