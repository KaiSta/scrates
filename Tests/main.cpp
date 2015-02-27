#include <iostream>
#include <Container.h>
#include <CloudManager.h>
#include <string>
#include <vector>
#include <chrono>
#include <cryptopp\cryptlib.h>
#include <cryptopp\sha.h>
#include <cryptopp\files.h>
#include <cryptopp\filters.h>
#include <cryptopp\base64.h>
#include <cryptopp\serpent.h>
#include <cryptopp\aes.h>
#include <cryptopp\twofish.h>
#include <Windows.h>

#include <istream>
#include <FileSystem.h>

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

int main()
{
	using namespace CryptoPP;

	local_file f;
	
	path vhdp("C:\\tmp\\local\\");
	path folderp("C:\\tmp\\folder");
	
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
				f.create(containername, password, filepath, { std::pair<std::string, size_t>(/*"$Local\\tempestTests"*/ synclocation, 53687091200) }, vhdp, local_file::storage_type::VHD);
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