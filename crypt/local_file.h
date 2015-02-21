#pragma once

#include <string>
#include <sstream>
#include <vector>
#include "FileSystem.h"
#include "../lib/pugixml/src/pugixml.hpp"
#include "Container.h"
#include "Path.h"
#include "VirtualDisk_Impl.h"
#include <cryptopp\cryptlib.h>
#include <cryptopp\serpent.h>
#include <cryptopp\aes.h>
#include <cryptopp\osrng.h>
#include <cryptopp\hex.h>
#include <cryptopp\sha.h>
#include <cryptopp\files.h>
#include <cryptopp\filters.h>
#include <cryptopp\pwdbased.h>
#include <cryptopp\modes.h>

#include <mutex>
#include <memory>

#include "Storage.h"
#include "VirtualDisk_Impl.h"
#include "Folder_Impl.h"

class local_file
{
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
		path& vhdpath, storage_type stor_type);
	void open(path& p, const std::string& passphrase,
		path& vhdpath, storage_type stor_type);

	CryptoPP::SecByteBlock get_seed();

	void manual_sync();

	path get_cloudpath();

	void close();

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

};

