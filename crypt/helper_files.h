#pragma once

#include <string>
#include "FileSystem.h"
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/crc.h>
#include <cryptopp/hex.h>
#include <cryptopp/serpent.h>
#include <cryptopp/twofish.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/zlib.h>
#include <cryptopp/secblock.h>
#include <thread>

static bool secure_crc(const std::string& path, std::string& crc)
{
	using namespace CryptoPP;

	if (!FileSystem::file_exists(path) || FileSystem::file_size(path) == 0)
	{
		return false;
	}

	bool ret = false;

	while (!ret)
	{
		try
		{
		        CRC32 crcfunc;
			FileSource(path.data(), true, new HashFilter(crcfunc,
				new HexEncoder(new StringSink(crc))));
			ret = true;
		}
		catch (CryptoPP::FileStore::OpenErr e)
		{
			crc = "";
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			//Sleep(200);
		}
		catch (Exception e)
		{
			break;
		}
	}
	return ret;
}


static bool secure_encrypt(const std::string& src, bool compressed, CryptoPP::StreamTransformationFilter* filter)
{
	using namespace CryptoPP;

	if (!FileSystem::file_exists(src) || FileSystem::file_size(src) == 0)
	{
		return false;
	}

	bool ret = false;

	while (!ret)
	{
		try
		{
			if (compressed)
			{
				FileSource(src.data(), true, new ZlibCompressor(filter,
					ZlibCompressor::MAX_DEFLATE_LEVEL));
			}
			else
			{
				FileSource(src.data(), true, filter);
			}
			ret = true;
		}
		catch (CryptoPP::FileStore::OpenErr e)
		{
		    std::this_thread::sleep_for(std::chrono::milliseconds(200));
			//Sleep(200);
		}
		catch (Exception e)
		{
			break;
		}
	}
	
	return ret;
}