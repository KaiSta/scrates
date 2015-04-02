#pragma once
#include <string>
#include "../lib/pugixml/src/pugixml.hpp"
#include <cryptopp\serpent.h>
#include <cryptopp\twofish.h>
#include <cryptopp\aes.h>
#include <cryptopp\osrng.h>
#include <cryptopp\zlib.h>
#include <cryptopp\secblock.h>
#include <vector>
#include "Path.h"
#include "container_handle.h"
#include "list_of_files.h"
#include "VirtualDisk_Impl.h"
#include "Storage.h"
#include "Synchronizer.h"
#include <atomic>

#include <mutex>

class Container
{
public:
	enum encryption_algorithm
	{
		AES = 0,
		SERPENT = 1,
		TWOFISH = 2,
		INVALID = 42
	};

	//Container(const std::string& path, const std::string& pw, encryption_algorithm algo = SERPENT);

	Container(encryption_algorithm algo = SERPENT);
	Container(VirtualDisk_Impl::volume_handle*, encryption_algorithm algo = SERPENT);
	Container(const path& location, const std::string& pw, VirtualDisk_Impl::volume_handle*, encryption_algorithm algo = SERPENT);
	~Container();

	void set_vhd(VirtualDisk_Impl::volume_handle*);
	void set_seed(CryptoPP::SecByteBlock seed);
	
	/**
	  \brief First step after creating a container instance.
	  It's important to call this function before using the container instance any further.
	  \param location where to store the container file
	  \param pw passphrase used for encryption
	  \param algo the encryption algorithm that should be used.
	*/
	void init(const path& location, const std::string& pw, encryption_algorithm algo = SERPENT);
	
	/**
	  \brief Second init step for containers. Adds cloud locations.
	  After init create has to be called to create the file and add the cloud locations. Calls open
	  if the container already exists.
	  \see init()
	  \param locations vector of std::pair<std::string, size_t>. first is the path to the cloud folder,
	  second is the cloud quota.
	*/
	void create(std::vector<std::pair<std::string, size_t>> locations);

	/**
	  \brief opens a existing container file. 
	*/
	void open();

	/**
	  \brief saves the container file.
	*/
	void save();

	/**
	  \brief sync the container file with the current state of cloud files.
	  Needed for updates that are made in cloud storage. The state of the cloud storage
	  is used to update the container file.
	*/
	void sync(bool ignore_container_state = false);

	/**
	  \brief updates the container file with the current state of local files.
	  Needed for updates inside the Virtual Hard Drive. Updates the lof inside
	  cloud storage.
	*/
	void manual_sync(bool ignore_container_state = false);

	/**
	  \brief Will sync the local files and the cloud files and closes the container.
	  After successfully syncing the local files will be deleted and the container closed.
	*/
	void close();

	/**
	  \brief Adds a new file to the container.
	  Calls update_file if file already exists inside the container.
	  \param src location of the file that will be added to the container.
	  \param rootfolder working directory from where the sub folders shall be created.
	*/
	void add_file(const path& src, const std::string& rootfolder);

	/**
	  \brief unused.
	  \param relative_path path beginning with the specified root folder and filename.
	*/
	void open_file(const std::string& relative_path);

	/**
	  \brief Removes a file from container.
	  \param relative_path path beginning with the specified root folder and filename.
	*/
	void delete_file(const path& relative_path);

	/**
	  \brief Will copy the specified file from container into the VHD.
	  \param relative_path path beginning with the specified root folder and filename.
	*/
	void extract_file(const path& relative_path/*, const path& dest*/);

	/**
	  \brief extracts all files from container to vhd.
	*/
	void extract_all();

	/**
	  \brief updates an existing file inside the container.
	  \param filename name of the file that will be updated.
	  \param relative_path path beginning with the specified root folder and filename.
	  \param src location of the file that will be used to update the file inside the container.
	*/
	void update_file(const std::string& filename, const path& relative_path, const path& src);

	/**
	  \brief list of all files inside the container.
	*/
	void list_files(std::vector<path>& files);

	void update_containerCrC();
	bool validate();
	
private:
	std::string container_name_;
	//std::string path_;
	path path_;
	std::string blocklocation_;
	std::string pw_;
	std::vector<byte> passphrase_;
	std::vector<byte> enhanced_passphrase_;
	encryption_algorithm algo_;
	
	std::string container_raw_;
	container_handle handle_;
	
	//VirtualDisk_Impl::volume_handle* volume_;
	Storage::volume_handle* volume_;
	CryptoPP::SecByteBlock seed_;
	CryptoPP::RandomPool prng_;

	Synchronizer syncer;
	std::atomic<bool> is_syncing;

	std::mutex wr_mtx_;
	std::mutex sync_lock_;
	bool container_open_;
	std::string containercrc_;

	bool file_exists(const path& relative_path);

	template <typename T>
	void helper_open()
	{
		using namespace CryptoPP;
		std::string recovered_front;
		FileSource source(path_.str().data(), false, new StringSink(recovered_front));
		source.Pump(salt_length);

		std::string recovered_salt(recovered_front.begin(), 
			recovered_front.begin() + salt_length);
		
		SecByteBlock recovered_derived_key(T::MAX_KEYLENGTH);
		SecByteBlock recovered_iv(T::BLOCKSIZE);
		PKCS5_PBKDF2_HMAC<SHA512> pwbase;
		pwbase.DeriveKey(recovered_derived_key, recovered_derived_key.size(),
			pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			reinterpret_cast<const byte*>(recovered_salt.data()), salt_length,
			derivation_iterations);
		pwbase.DeriveKey(recovered_iv, recovered_iv.size(),
			pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			reinterpret_cast<const byte*>(recovered_salt.data()), salt_length,
			1000);
		typename CFB_Mode<T>::Decryption decrypt;
		decrypt.SetKeyWithIV(recovered_derived_key, recovered_derived_key.size(), recovered_iv);
		
		source.Detach(new CryptoPP::StreamTransformationFilter(decrypt,
			new StringSink(container_raw_)));
		source.PumpAll();
	}

	template <typename T>
	void helper_create()
	{
		using namespace CryptoPP;
		
		char tmp_key[T::MAX_KEYLENGTH];
		std::string key;
		prng_.GenerateBlock(reinterpret_cast<byte*>(&tmp_key), T::MAX_KEYLENGTH);
		StringSource(reinterpret_cast<const byte*>(&tmp_key), T::MAX_KEYLENGTH, true, new HexEncoder(new StringSink(key)));

		container_handle::container_node cnode;
		cnode.name = container_name_;
		cnode.compressed = true;
		cnode.version = "1.0";
		handle_.create(cnode);

		//create XML part
		std::string xmlcode = handle_.str();

		//create SALT
		SecByteBlock salt(salt_length);
		prng_.GenerateBlock(salt, salt.size());

		//create File and add salt
		std::string file;
		StringSink* strsink = new StringSink(file);
		strsink->Put(salt, salt.size());
		//init encryption
		PKCS5_PBKDF2_HMAC<SHA512> pwbase;
		SecByteBlock derived_key(T::MAX_KEYLENGTH);
		SecByteBlock iv(T::BLOCKSIZE);
		pwbase.DeriveKey(derived_key, derived_key.size(), pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			derivation_iterations);
		pwbase.DeriveKey(iv, iv.size(), pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			1000);
		typename CFB_Mode<T>::Encryption encrypt;
		encrypt.SetKeyWithIV(derived_key, derived_key.size(), iv);
		//add teststring	
		//StringSource(std::string("TRUE"), true, new StreamTransformationFilter(encrypt, strsink));
		//add xml
		//strsink = new StringSink(file);
		StringSource(xmlcode, true, new StreamTransformationFilter(encrypt, strsink));
		//write to file
		StringSource(file, true, new FileSink(path_.str().data()));
	}

	template <typename T>
	void helper_save()
	{
		using namespace CryptoPP;
		std::string recovered_front;
		FileSource source(path_.str().data(), false, new StringSink(recovered_front));
		source.Pump(salt_length + 4);

		std::string recovered_salt(recovered_front.begin(),
			recovered_front.begin() + salt_length);
		std::string recovered_teststring(recovered_front.begin() + salt_length,
			recovered_front.end());

		SecByteBlock salt(salt_length);
		prng_.GenerateBlock(salt, salt.size());

		SecByteBlock recovered_derived_key(T::MAX_KEYLENGTH);
		SecByteBlock recovered_iv(T::BLOCKSIZE);
		PKCS5_PBKDF2_HMAC<SHA512> pwbase;
		pwbase.DeriveKey(recovered_derived_key, recovered_derived_key.size(),
			pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			derivation_iterations);
		pwbase.DeriveKey(recovered_iv, recovered_iv.size(),
			pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			1000);
		typename CFB_Mode<T>::Encryption encrypt;
		encrypt.SetKeyWithIV(recovered_derived_key, recovered_derived_key.size(), recovered_iv);
		
		//create File and add salt
		std::string file;
		StringSink* strsink = new StringSink(file);
		strsink->Put(salt, salt.size());

		std::string xmlcode = handle_.str();

		//add teststring	
		StringSource(std::string("TRUE"), true, new StreamTransformationFilter(encrypt, strsink));
		//add xml
		strsink = new StringSink(file);
		StringSource(xmlcode, true, new StreamTransformationFilter(encrypt, strsink));
		//write to file
		StringSource(file, true, new FileSink(path_.str().data()));
	}
	

	template <typename T>
	bool helper_add_file(const path& src, const path& store_path, std::string& ivi, std::string& hashname)
	{
		using namespace CryptoPP;	

		SecByteBlock salt(salt_length);
		prng_.GenerateBlock(salt, salt_length);

		PKCS5_PBKDF2_HMAC<SHA512> pwbase;
		SecByteBlock derived_key(T::MAX_KEYLENGTH);
		SecByteBlock iv(T::BLOCKSIZE);
		pwbase.DeriveKey(derived_key, derived_key.size(), pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			derivation_iterations);
		pwbase.DeriveKey(iv, iv.size(), pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			1000);

		char tmp_hashname[16];
		prng_.GenerateBlock(reinterpret_cast<byte*>(&tmp_hashname), 16);
		StringSource(reinterpret_cast<const byte*>(&tmp_hashname), 16, true, new HexEncoder(new StringSink(hashname)));

		typename CFB_Mode<T>::Encryption encrypt;
		encrypt.SetKeyWithIV(derived_key, T::MAX_KEYLENGTH, iv);
		
		path location(store_path.str() + FileSystem::path_separator + hashname);

		FileSink* fsink = new FileSink(location.str().data());
		fsink->Put(salt, salt.size());

		bool compress = handle_.is_compressed();
		
		bool ret = secure_encrypt(src.str(), compress, new StreamTransformationFilter(encrypt, fsink));
		return ret;
	}

	template <typename T>
	bool helper_update_file(const container_handle::file_node& node, const path& src, const path& dest, std::string& out_iv)
	{
		using namespace CryptoPP;

		SecByteBlock salt(salt_length);
		prng_.GenerateBlock(salt, salt_length);

		PKCS5_PBKDF2_HMAC<SHA512> pwbase;
		SecByteBlock derived_key(T::MAX_KEYLENGTH);
		SecByteBlock iv(T::BLOCKSIZE);
		pwbase.DeriveKey(derived_key, derived_key.size(), pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			derivation_iterations);
		pwbase.DeriveKey(iv, iv.size(), pwbase.UsesPurposeByte(),
			&enhanced_passphrase_[0], enhanced_passphrase_.size(),
			salt, salt.size(),
			1000);
		

		path relative_path(node.path);
		relative_path = relative_path.append_filename(node.filename);
		std::string hashname(handle_.get_block_filename(relative_path, 1));
		path location(dest.str() + FileSystem::path_separator + hashname);

		typename CFB_Mode<T>::Encryption encrypt;
		encrypt.SetKeyWithIV(derived_key, T::MAX_KEYLENGTH, iv);

		FileSink* fsink = new FileSink(location.str().data());
		fsink->Put(salt, salt.size());

		bool compress = handle_.is_compressed();
		bool ret = secure_encrypt(src.str(), compress, new StreamTransformationFilter(encrypt, fsink));
		return ret;
	}

	template <typename T>
	void helper_extract_file(const container_handle::file_node& node, const std::string& dest)
	{
		using namespace CryptoPP;
		bool succ = false;
		while (!succ)
		{
			try
			{
				std::string hashname(node.blocks[0].filename);
				std::string locationname(node.blocks[0].location);
				std::string filename(node.filename);
				path extract_to(dest + node.path.str());
				FileSystem::make_folders(extract_to.str());
				extract_to = extract_to.append_filename(filename);

				path src(handle_.get_location_path(locationname));
				src = src.append_filename(hashname);

				std::vector<byte> recovered_salt(salt_length);
				FileSource source(src.str().data(), false, new ArraySink(&recovered_salt[0], salt_length));
				source.Pump(salt_length);

				PKCS5_PBKDF2_HMAC<SHA512> pwbase;
				SecByteBlock derived_key(T::MAX_KEYLENGTH);
				SecByteBlock iv(T::BLOCKSIZE);
				pwbase.DeriveKey(derived_key, derived_key.size(), pwbase.UsesPurposeByte(),
					&enhanced_passphrase_[0], enhanced_passphrase_.size(),
					&recovered_salt[0], salt_length,
					derivation_iterations);
				pwbase.DeriveKey(iv, iv.size(), pwbase.UsesPurposeByte(),
					&enhanced_passphrase_[0], enhanced_passphrase_.size(),
					&recovered_salt[0], salt_length,
					1000);

				typename CFB_Mode<T>::Decryption decrypt;
				decrypt.SetKeyWithIV(derived_key, T::MAX_KEYLENGTH, iv);

				bool ret = false;
				bool compress = handle_.is_compressed();
				if (compress)
				{
					source.Detach(new StreamTransformationFilter(decrypt, new ZlibDecompressor(new FileSink(extract_to.str().data()))));
					source.PumpAll();
					succ = true;
				}
				else
				{
					source.Detach(new StreamTransformationFilter(
						decrypt, new FileSink(extract_to.str().data())));
					source.PumpAll();
					succ = true;
				}
			}
			catch (std::exception e)
			{
				std::cout << e.what() << std::endl;
				Sleep(200);
			}
		}
	}

	const int salt_length = 32;
	const int derivation_iterations = 16384;
};

