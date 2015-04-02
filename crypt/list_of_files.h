#pragma once

#include <string>
#include <sstream>
#include <vector>
#include "FileSystem.h"
#include "../lib/pugixml/src/pugixml.hpp"
#include "Path.h"
#include <cryptopp/cryptlib.h>
#include <cryptopp/serpent.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/modes.h>

#include <mutex>

class list_of_files
{
public:
	struct item
	{
		std::string name;
		std::string crc;
		size_t size;
		std::string location;
		path relative_path;

		std::string size_to_string()
		{
			std::stringstream ss;
			ss << size;
			return ss.str();
		}
	};

	typedef std::vector<item> list_type;
	typedef item item_type;

	void create(path location, const std::vector<byte>& pw)
	{
		location_ = location.append_filename("lof.db");
		
		if (FileSystem::file_exists(location_.str()))
		{
			open(location, pw);
			return;
		}

		file_.append_child("files");

		std::stringstream ss;
		file_.save(ss);
		std::string xmlcode(ss.str());

		using namespace CryptoPP;

		AutoSeededX917RNG<Serpent> prng;

		SecByteBlock salt(16);
		prng.GenerateBlock(salt, salt.size());

		PKCS5_PBKDF2_HMAC<SHA512> base;
		SecByteBlock iv(Serpent::BLOCKSIZE);
		SecByteBlock key(Serpent::MAX_KEYLENGTH);
		base.DeriveKey(iv, iv.size(), base.UsesPurposeByte(),
			&pw[0], pw.size(), salt, salt.size(), 1989);
		base.DeriveKey(key, key.size(), base.UsesPurposeByte(),
			&pw[0], pw.size(), salt, salt.size(), 3103);

		std::string file;
		StringSink* strsink = new StringSink(file);
		strsink->Put(salt, salt.size());

		CFB_Mode<Serpent>::Encryption encrypt;
		encrypt.SetKeyWithIV(key, Serpent::MAX_KEYLENGTH, iv);
		StringSource(xmlcode, true, new StreamTransformationFilter(encrypt, strsink));

		StringSource(file, true, new FileSink(location_.str().data()));
		//file_.save_file(location_.str().data());

		pw_ = pw;
	}

	void open(path location, const std::vector<byte>& pw)
	{
		std::lock_guard<std::mutex> guard(mtx_);

		location_ = location.append_filename("lof.db");
		
		using namespace CryptoPP;
		
		std::vector<byte> salt(16);
		FileSource source(location_.str().data(), false, new ArraySink(&salt[0], 16));
		source.Pump(16);

		PKCS5_PBKDF2_HMAC<SHA512> base;
		SecByteBlock iv(Serpent::BLOCKSIZE);
		SecByteBlock key(Serpent::MAX_KEYLENGTH);
		base.DeriveKey(iv, iv.size(), base.UsesPurposeByte(),
			pw.data(), pw.size(),
			salt.data(), salt.size(), 1989);
		base.DeriveKey(key, key.size(), base.UsesPurposeByte(),
			&pw[0], pw.size(), &salt[0], salt.size(), 3103);

		CFB_Mode<Serpent>::Decryption decrypt;
		decrypt.SetKeyWithIV(key, Serpent::MAX_KEYLENGTH, iv);
		std::string xmlcode;
		source.Detach(new StreamTransformationFilter(decrypt, 
			new StringSink(xmlcode)));
		source.PumpAll();

		file_.load(xmlcode.data());

		pw_ = pw;
		//file_.load_file(location_.str().data());
	}

	void reload()
	{
		open(location_.get_folderpath(), pw_);
	}

	void save()
	{
		std::lock_guard<std::mutex> guard(mtx_);

		std::stringstream ss;
		file_.save(ss);
		std::string xmlcode(ss.str());

		using namespace CryptoPP;

		AutoSeededX917RNG<Serpent> prng;

		SecByteBlock salt(16);
		prng.GenerateBlock(salt, salt.size());

		PKCS5_PBKDF2_HMAC<SHA512> base;
		SecByteBlock iv(Serpent::BLOCKSIZE);
		SecByteBlock key(Serpent::MAX_KEYLENGTH);
		base.DeriveKey(iv, iv.size(), base.UsesPurposeByte(),
			pw_.data(), pw_.size(),
			salt, salt.size(), 1989);
		base.DeriveKey(key, key.size(), base.UsesPurposeByte(),
			pw_.data(), pw_.size(), &salt[0], salt.size(), 3103);

		std::string file;
		StringSink* strsink = new StringSink(file);
		strsink->Put(salt, salt.size());

		CFB_Mode<Serpent>::Encryption encrypt;
		encrypt.SetKeyWithIV(key, Serpent::MAX_KEYLENGTH, iv);
		StringSource(xmlcode, true, new StreamTransformationFilter(encrypt, strsink));

		StringSource(file, true, new FileSink(location_.str().data()));
		//file_.save_file(location_.str().data());
		dump();
	}

	list_type get_file_list()
	{
		std::lock_guard<std::mutex> guard(mtx_);

		list_type list;
		auto filesnode = file_.child("files");
		for (pugi::xml_node file = filesnode.child("file");
			file;
			file = file.next_sibling("file"))
		{
			item_type tmp;
			tmp.name = file.attribute("name").value();
			tmp.crc = file.attribute("crc").value();
			tmp.size = std::stoi(file.attribute("size").value());
			tmp.location = file.attribute("location").value();
			tmp.relative_path = file.attribute("path").value();
			list.push_back(tmp);
		}

		return list;
	}

	void add_file(item_type file)
	{
		{
			std::lock_guard<std::mutex> guard(mtx_);
			auto filesnode = file_.child("files");
			auto newnode = filesnode.append_child("file");
			newnode.append_attribute("name") = file.name.data();
			newnode.append_attribute("crc") = file.crc.data();
			newnode.append_attribute("size") = file.size_to_string().data();
			newnode.append_attribute("location") = file.location.data();
			newnode.append_attribute("path") = file.relative_path.raw_std_str().data();
		}
		save();
	}

	item_type get_file(std::string name)
	{
		std::lock_guard<std::mutex> guard(mtx_);

		auto filesnode = file_.child("files");
		for (pugi::xml_node file_node = filesnode.child("file");
			file_node;
			file_node = file_node.next_sibling("file"))
		{
			item_type tmp;
			tmp.name = file_node.attribute("name").value();
			if (tmp.name == name)
			{
				tmp.crc = file_node.attribute("crc").value();
				tmp.size = std::stoi(file_node.attribute("size").value());
				tmp.location = file_node.attribute("location").value();
				tmp.relative_path = file_node.attribute("path").value();
				return tmp;
			}
		}

		return{ "", "", 0, "", "" };
	}

	void update_file(item_type file)
	{
		delete_file(file);
		add_file(file);
	}

	bool contains(item_type file)
	{
		std::lock_guard<std::mutex> guard(mtx_);

		auto filesnode = file_.child("files");
		for (pugi::xml_node file_node = filesnode.child("file");
			file_node;
			file_node = file_node.next_sibling("file"))
		{
			item_type tmp;
			std::string name(file_node.attribute("name").value());
			if (name == file.name)
			{
				return true;
			}
		}
	}

	void delete_file(item_type file)
	{
		{
			std::lock_guard<std::mutex> guard(mtx_);
			auto filesnode = file_.child("files");
			for (pugi::xml_node file_node = filesnode.child("file");
				file_node;
				file_node = file_node.next_sibling("file"))
			{
				item_type tmp;
				std::string name(file_node.attribute("name").value());
				if (name == file.name)
				{
					filesnode.remove_child(file_node);
					break;
				}
			}
		}
		
		save();
	}

	void dump()
	{
		file_.save_file("C:\\Users\\Kai\\Desktop\\lofdump.xml");
	}

private:
	pugi::xml_document file_;
	path location_;

	std::vector<byte> pw_;

	std::mutex mtx_;
};