#include <iostream>
#include <Container.h>
#include <CloudManager.h>
#include <string>
#include <vector>
#include <chrono>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/serpent.h>
#include <cryptopp/aes.h>
#include <cryptopp/twofish.h>

#include <istream>
#include <FileSystem.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#pragma comment(lib, "Shell32.lib")
#endif

#include <VirtualDisk_Impl.h>
#include <Path.h>

#include <fstream>

#include <local_file.h>
#include <uarng.h>

#include <MessageTypes.h>
#include <mutex>
#include <fstream>

void sometest()
{
	using namespace CryptoPP;
	AutoSeededRandomPool prng;
	SecByteBlock key_serpent(Serpent::MAX_KEYLENGTH);
	SecByteBlock iv_serpent(Serpent::BLOCKSIZE);
	SecByteBlock key_aes(AES::MAX_KEYLENGTH);
	SecByteBlock iv_aes(AES::BLOCKSIZE);
	SecByteBlock key_twofish(Twofish::MAX_KEYLENGTH);
	SecByteBlock iv_twofish(Twofish::BLOCKSIZE);

	prng.GenerateBlock(key_serpent, key_serpent.size());
	prng.GenerateBlock(iv_serpent, iv_serpent.size());
	prng.GenerateBlock(key_aes, key_aes.size());
	prng.GenerateBlock(iv_aes, iv_aes.size());
	prng.GenerateBlock(key_twofish, key_twofish.size());
	prng.GenerateBlock(iv_twofish, iv_twofish.size());

	CFB_Mode<Serpent>::Encryption en_serpent(key_serpent, key_serpent.size(), iv_serpent);
	CFB_Mode<Serpent>::Encryption en_aes(key_aes, key_aes.size(), iv_aes);
	CFB_Mode<Serpent>::Encryption en_twofish(key_twofish, key_twofish.size(), iv_twofish);

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point stop;

	start = std::chrono::high_resolution_clock::now();
	FileSource("C:\\Users\\Kai\\Desktop\\testfile.7z", true, new StreamTransformationFilter(en_serpent, new FileSink("C:\\Users\\Kai\\Desktop\\test\\test1.crypt")));
	stop = std::chrono::high_resolution_clock::now();
	std::cout << "Serpent encryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
	start = std::chrono::high_resolution_clock::now();
	FileSource("C:\\Users\\Kai\\Desktop\\testfile.7z", true, new StreamTransformationFilter(en_aes, new FileSink("C:\\Users\\Kai\\Desktop\\test\\test2.crypt")));
	stop = std::chrono::high_resolution_clock::now();
	std::cout << "AES encryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
	start = std::chrono::high_resolution_clock::now();
	FileSource("C:\\Users\\Kai\\Desktop\\testfile.7z", true, new StreamTransformationFilter(en_twofish, new FileSink("C:\\Users\\Kai\\Desktop\\test\\test3.crypt")));
	stop = std::chrono::high_resolution_clock::now();
	std::cout << "Twofish encryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;

	CFB_Mode<Serpent>::Decryption de_serpent(key_serpent, key_serpent.size(), iv_serpent);
	CFB_Mode<Serpent>::Decryption de_aes(key_aes, key_aes.size(), iv_aes);
	CFB_Mode<Serpent>::Decryption de_twofish(key_twofish, key_twofish.size(), iv_twofish);

	start = std::chrono::high_resolution_clock::now();
	FileSource("C:\\Users\\Kai\\Desktop\\test\\test1.crypt", true, new StreamTransformationFilter(en_serpent, new FileSink("C:\\Users\\Kai\\Desktop\\test\\testfile1.7z")));
	stop = std::chrono::high_resolution_clock::now();
	std::cout << "Serpent decryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
	start = std::chrono::high_resolution_clock::now();
	FileSource("C:\\Users\\Kai\\Desktop\\test\\test2.crypt", true, new StreamTransformationFilter(en_aes, new FileSink("C:\\Users\\Kai\\Desktop\\test\\testfile2.7z")));
	stop = std::chrono::high_resolution_clock::now();
	std::cout << "AES decryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
	start = std::chrono::high_resolution_clock::now();
	FileSource("C:\\Users\\Kai\\Desktop\\test\\test3.crypt", true, new StreamTransformationFilter(en_twofish, new FileSink("C:\\Users\\Kai\\Desktop\\test\\testfile3.7z")));
	stop = std::chrono::high_resolution_clock::now();
	std::cout << "Twofish decryption: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;

	system("pause");

	return;
}

static std::vector<std::string> list_folders(const std::string& p, bool recursive)
{
	Poco::File f(p);
	std::vector<std::string> folders;
	f.list(folders);

	std::vector<std::string> found;

	for (auto it = folders.begin(); it != folders.end(); ++it)
	{
		Poco::File tmp(Poco::Path(f.path(), (*it)));
		
		if (tmp.isDirectory())
		{
			found.push_back(tmp.path());

			if (recursive)
			{
				auto v = list_folders(tmp.path(), true);
				found.insert(found.begin(), v.begin(), v.end());
			}
		}		
	}

	return found;
}

static std::vector<std::string> list_files(const std::string& p, bool recursive)
{
	Poco::File f(p);
	std::vector<std::string> folders;
	f.list(folders);

	std::vector<std::string> found;

	for (auto it = folders.begin(); it != folders.end(); ++it)
	{
		Poco::File tmp(Poco::Path(f.path(), (*it)));

		if (tmp.isFile())
		{
			found.push_back(tmp.path());
		}
		else if (tmp.isDirectory())
		{
			if (recursive)
			{
				auto v = list_files(tmp.path(), true);
				found.insert(found.begin(), v.begin(), v.end());
			}
		}
	}

	return found;
}

void filesys_replace()
{
	/*Poco::Path p("C:\\Users\\Kai\\Desktop\\testpoco\\test");
	Poco::File tmp(p);
	tmp.createDirectories();
	FileSystem::make_folders("C:\\Users\\Kai\\Desktop\\testpoco\\test");
	*/
	Poco::Path p("C:\\Users\\Kai\\Desktop\\testpoco\\test");
	Poco::File tmp(p);
	tmp.createDirectories();
	Poco::File tmpfile(Poco::Path(p, "samplefile.dat"));
	tmpfile.createFile();
	tmpfile.setSize(1024);
	Poco::File tmpfile2(Poco::Path(p, "samplefile2.dat"));
	tmpfile.copyTo(tmpfile2.path());

	std::vector<std::string> files;
	Poco::File diri(p);
	if (diri.isDirectory())
	{
		diri.list(files);
		for (auto& e : files)
		{
			Poco::File f(Poco::Path(p,e));
			if (f.isDirectory())
				std::cout << "dir " << e << std::endl;
			else
				std::cout << e << std::endl;
		}
	}
	std::cout << "-------------------------------\n";
	std::vector<std::string> files2;
	FileSystem::get_all_files_in_dir(p.toString(), files2);
	for (auto& e : files2)
	{
		std::cout << e << std::endl;
	}
	std::cout << "-------------------------------\n";
	{
		Poco::File f(p.toString());
		std::vector<std::string> files;
		std::vector<std::string> out;
		f.list(files);

		for (auto it = files.begin(); it != files.end(); ++it)
		{
			Poco::File tmp(Poco::Path(p, (*it)));
			if (tmp.isDirectory())
			{
				it = files.erase(it);
			}
		}

		for (auto& e : files)
		{
			//out.push_back(p.toString() + "\\" + e);
			out.push_back(Poco::Path(p, e).toString());
		}
		for (auto& x : out)
		{
			std::cout << x << std::endl;
		}
	}
	std::cout << "-------------------------------\n";
	FileSystem::list_folders("C:\\Users\\Kai\\Desktop\\testpoco", true);
	std::cout << "-------------------------------\n";
	{
		Poco::File f("C:\\Users\\Kai\\Desktop\\testpoco");
		auto x = list_folders("C:\\Users\\Kai\\Desktop\\testpoco", true);
		for (auto& e : x)
		{
			std::cout << e << std::endl;
		}
	}
	std::cout << "-------------------------------\n";
	auto x = list_files("C:\\Users\\Kai\\Desktop\\testpoco", true);
	for (auto& e : x)
	{
		std::cout << e << std::endl;
	}

}

static std::ofstream logfile("log.txt");
static std::mutex m;

void callback_func(container_event e)
{
	std::lock_guard<std::mutex> guard(m);
	std::string type;
	std::string message;
	switch (e.type)
	{
	case INFORMATION:
		type = "information";
		break;
	case CONFLICT:
		type = "error";
		break;
	case WARNING:
		type = "warning";
		break;
	case VERBOSE:
		type = "verbose";
		break;
	}
	
	switch (e.message)
	{
	case NONE:
		message = "none";
		break;
	case CTR_FILE_NOT_FOUND:
		message = "container file not found";
		break;
	case WRONG_PASSWORD:
		message = "wrong password";
		break;
	case MISSING_ENC_FILES:
		message = "missing encrypted file in cloud";
		break;
	case DECRYPTION_ERROR:
		message = "decryption error";
		break;
	case WRONG_ARGUMENTS:
		message = "wrong arguments";
		break;
	case SUCC:
		message = "succ";
		break;
	case SYNCHRONIZING:
		message = "sync";
		break;
	case FINISHED_SYNCHRONIZING:
		message = "finished sync";
		break;
	case NO_WARNING:
		message = "no warning";
		break;
	case CLOSING:
		message = "closing container";
		break;
	case ADD_FILE:
		message = "adding file " + std::string(e._data_.data());
		break;
	case UPDATE_FILE:
		message = "updating file " + std::string(e._data_.data());
		break;
	case DELETE_FILE:
		message = "delete file " + std::string(e._data_.data());
		break;
	case EXTRACT_FILE:
		message = "extract file " + std::string(e._data_.data());
		break;
	case EXTRACT_FILES:
		message = "extract files ";
		break;
	case INIT_CONTAINER:
		message = "initializing container";
		break;
	}
	logfile << type << " - " << message << std::endl;
}

int main()
{
	/*filesys_replace();
	system("pause");
	return 0;*/

	using namespace CryptoPP;

	local_file f;
	
	path vhdp("C:\\tmp\\local\\");
#ifdef _WIN32
	path folderp("C:\\tmp\\folder");
#else
	path folderp("/home/kai/Documents/folder");
#endif
	
	while (true)
	{
		std::cout << ">";
		std::string cmd;
		std::cin >> cmd;

		if (cmd == "exit")
		{
			f.close();
			break;
		}
		else if (cmd == "create")
		{
			std::cout << "container name:\n";
			std::string containername;
			std::cin >> containername;
			std::cout << "container location:\n";
			std::string location;
			std::cin >> location;
			std::cout << "password:\n";
			std::string password;
			std::cin >> password;
			std::cout << "sync location\n";
			std::string synclocation;
			std::cin >> synclocation;

			path filepath(location);
			filepath = filepath.append_filename(containername + ".cco");
			//path filepath("C:\\Users\\Kai\\Desktop\\test\\" + containername +".cco");

			std::cout << "Initialization..." << std::endl;
			try
			{
				f.create(containername, password, filepath, 
				{ std::pair<std::string, size_t>(/*"$Local\\tempestTests"*/ synclocation, 53687091200) }, 
				folderp, local_file::storage_type::FOLDER, callback_func);
			}
			catch (std::invalid_argument e)
			{
				std::cout << e.what() << std::endl;
			}
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else if (cmd == "sync")
		{
			f.manual_sync();
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		cmd.clear();
	}
	system("pause");

	return 0;
}
