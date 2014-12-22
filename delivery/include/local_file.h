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

class local_file
{
public:
	local_file();
	~local_file();

	void create(const std::string& containername,
		const std::string& passphrase,
		path& p, std::vector<std::pair<std::string, size_t>> locations,
		path& vhdpath);
	void open(path& p, const std::string& passphrase,
		path& vhdpath);

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

};

