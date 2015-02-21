#include "local_file.h"
#include "Container.h"
#include "number_helper.h"
#include "uarng.h"

local_file::local_file() : random_num_(32)
{
}

void local_file::create(const std::string& containername, 
	const std::string& passphrase,
	path& p, std::vector<std::pair<std::string, size_t>> locations,
	path& vhdpath, storage_type stor_type)
{
	using namespace CryptoPP;
	passphrase_ = passphrase;
	location_ = p;
	containername_ = containername;

	//check if file already exist, if true call open instead
	if (FileSystem::file_exists(p.str()))
	{
		open(p, passphrase, vhdpath, stor_type);
		return;
	}

	switch (stor_type)
	{
	case storage_type::VHD:
	{
		storage_ = std::make_shared<VirtualDisk_Impl>();
	}
		break;
	case storage_type::FOLDER:
	{
		storage_ = std::make_shared<Folder_Impl>();
	}
		break;
	}

	auto containernode = file_.append_child("container");
	containernode.append_attribute("name") = containername.c_str();
	containernode.append_attribute("version") = "1.00";

	//create random number for further encryptions
	UARNG uarng;
	uarng.generate(random_num_);

	auto seednode = containernode.append_child("seed");
	
	std::string seed;
	HexEncoder encoder;
	encoder.Put(random_num_, random_num_.size());
	encoder.MessageEnd();
	auto s = encoder.MaxRetrievable();
	if (s)
	{
		seed.resize(s);
		encoder.Get((byte*)seed.data(), seed.size());
	}
	seednode.append_child(pugi::node_pcdata).set_value(seed.data());

	//add locations
	auto locationsnode = containernode.append_child("locations");
	for (auto& e : locations)
	{
		auto locationnode = locationsnode.append_child("location");
		auto s = e.first.find("$");
		std::string name{};
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
		FileSystem::make_folders(location_path.str());
		locationnode.append_attribute("path") = location_path.raw_std_str().data();
		std::stringstream ss;
		ss << e.second;
		locationnode.append_attribute("quota") = ss.str().data();
		//locationnode.append_attribute("used") = "0";
	}
	file_.save_file("C:\\Users\\kaivm\\Desktop\\dumb.xml");
	//create and open vhd
	if (stor_type == storage_type::VHD)
	{
		std::string vhd_name(containername + ".vhd");
		vhdpath = vhdpath.append_filename(vhd_name);
		uint64_t vhdsize{ 0 };
		for (auto& e : locations)
		{
			vhdsize += e.second;
		}

		if (!FileSystem::file_exists(vhdpath.str()))
		{
			//VirtualDisk_Impl::create(vhdpath.str(), vhdsize /1024/1024, vhd_);
			storage_->create(vhdpath.str(), vhdsize / 1024 / 1024, vhd_);
		}
	}
	else if (stor_type == storage_type::FOLDER)
	{
		storage_->create(vhdpath.str(), 0, vhd_);
	}

	//create cloud db
	container_.set_vhd(&vhd_);
	container_.set_seed(random_num_);
	path cloudfile_location(locations[0].first);
	cloudfile_location = cloudfile_location.append_filename(containername + ".db");

	FileSystem::make_folders(vhd_.drive_letter + containername);
	container_.init(cloudfile_location, passphrase);
	container_.create(locations);
	

	//encrypt local file
	std::stringstream ss;
	file_.save(ss);
	std::string xmlcode(ss.str());

	//AutoSeededX917RNG<Serpent> prng;
	RandomPool prng;
	prng.IncorporateEntropy(random_num_, random_num_.size());

	SecByteBlock salt(32);
	prng.GenerateBlock(salt, salt.size());

	PKCS5_PBKDF2_HMAC<SHA512> base;
	SecByteBlock key(Serpent::MAX_KEYLENGTH);
	SecByteBlock iv(Serpent::BLOCKSIZE);
	base.DeriveKey(key, key.size(), base.UsesPurposeByte(),
		reinterpret_cast<const byte*>(passphrase.data()), passphrase.size(),
		salt, salt.size(), 19890);
	base.DeriveKey(iv, iv.size(), base.UsesPurposeByte(),
		reinterpret_cast<const byte*>(passphrase.data()), passphrase.size(),
		salt, salt.size(), 3103);

	std::string file;
	StringSink* strsink = new StringSink(file);
	strsink->Put(salt, salt.size());

	CFB_Mode<Serpent>::Encryption encrypt;
	encrypt.SetKeyWithIV(key, Serpent::MAX_KEYLENGTH, iv);
	//add teststring	
	StringSource(std::string("TRUE"), true, new StreamTransformationFilter(encrypt, strsink));
	//add xml code
	StringSource(xmlcode, true, new StreamTransformationFilter(
		encrypt, new StringSink(file)/*strsink*/));
	StringSource(file, true, new FileSink(p.str().data()));
}

void local_file::open(path& p, const std::string& passphrase,
	path& vhdpath, storage_type stor_type)
{
	location_ = p;
	passphrase_ = passphrase;

	switch (stor_type)
	{
	case storage_type::VHD:
	{
		storage_ = std::make_shared<VirtualDisk_Impl>();
	}
		break;
	case storage_type::FOLDER:
	{
		storage_ = std::make_shared<Folder_Impl>();
	}
		break;
	}

	using namespace CryptoPP;
	//decrypt local file
	std::vector<byte> salt(32);
	std::string teststring;
	FileSource source(p.str().data(), false, new ArraySink(&salt[0], 32));
	source.Pump(32);

	PKCS5_PBKDF2_HMAC<SHA512> base;
	SecByteBlock key(Serpent::MAX_KEYLENGTH);
	SecByteBlock iv(Serpent::BLOCKSIZE);
	base.DeriveKey(key, key.size(), base.UsesPurposeByte(),
		reinterpret_cast<const byte*>(passphrase.data()), passphrase.size(),
		&salt[0], salt.size(), 19890);
	base.DeriveKey(iv, iv.size(), base.UsesPurposeByte(),
		reinterpret_cast<const byte*>(passphrase.data()), passphrase.size(),
		salt.data(), salt.size(), 3103);

	CFB_Mode<Serpent>::Decryption decrypt;
	decrypt.SetKeyWithIV(key, Serpent::MAX_KEYLENGTH, iv);
	source.Detach(new StreamTransformationFilter(decrypt, new StringSink(teststring)));
	source.Pump(4);
	if (teststring != "TRUE")
	{
		throw(std::invalid_argument("wrong password"));
		return;
	}
	std::string xmlcode;
	source.Detach(new StreamTransformationFilter(decrypt, new StringSink(xmlcode)));
	source.PumpAll();
	file_.load(xmlcode.data());
	
	//get containername
	std::string containername(file_.child("container").attribute("name").value());

	//get seed
	auto seednode = file_.child("container").child("seed");
	std::string encoded_seed(seednode.child_value());
	StringSource(encoded_seed.data(), true, new HexDecoder(new ArraySink(random_num_, 32)));

	//get cloud location
	auto locationnode = file_.child("container").child("locations").child("location");
	cloudloc_ = locationnode.attribute("path").value();
	path cl(cloudloc_);
	cl = cl.append_filename(containername + ".db");

	//create and open vhd
	if (stor_type == storage_type::VHD)
	{
		std::string vhd_name(containername + ".vhd");
		vhdpath = vhdpath.append_filename(vhd_name);

		std::string si(locationnode.attribute("quota").value());

		uint64_t vhdsize = s_to_int64(si);

		if (!FileSystem::file_exists(vhdpath.str()))
		{
			//VirtualDisk_Impl::create(vhdpath.str(), vhdsize / 1024 / 1024, vhd_);
			storage_->create(vhdpath.str(), vhdsize / 1024 / 1024, vhd_);
		}
		else
		{
			//VirtualDisk_Impl::mount_drive(vhdpath.str(), vhd_);
			storage_->mount_drive(vhdpath.str(), vhd_);
		}
	}
	else if (stor_type == storage_type::FOLDER)
	{
		storage_->create(vhdpath.str(), 0, vhd_);
	}

	container_.set_seed(random_num_);
	container_.set_vhd(&vhd_);
	container_.init(cl.str(), passphrase);
	container_.open();
}

CryptoPP::SecByteBlock local_file::get_seed()
{
	return random_num_;
}

void local_file::manual_sync()
{
	container_.sync();
	container_.manual_sync();
}

void local_file::close()
{

}

local_file::~local_file()
{
	container_.close();
	storage_->dismount_drive(vhd_);
	//VirtualDisk_Impl::dismount_drive(vhd_);
}
