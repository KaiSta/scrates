#include "CloudManager.h"
#include <cryptopp/cryptlib.h>
#include <cryptopp/base64.h>
#include <iostream>
#include <istream>
#include <string>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>

#include "string_helper.h"
#include "FileSystem.h"

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#pragma comment(lib, "Shell32.lib")
#endif

CloudManager& CloudManager::instance()
{
	static CloudManager inst;
	return inst;
}

CloudManager::CloudManager()
{
	autodetect();
}


CloudManager::~CloudManager()
{
}

std::string CloudManager::extent(std::string path)
{
	for (auto& x : provider_locations_)
	{
		replaceAll(path, x.first, x.second);
	}
	/*std::string search = "$Dropbox";
	std::string replacement = "F:\\Dropbox";
	replaceAll(path, search, replacement);*/
	return path;
}

std::string CloudManager::get_location_name(const std::string& path)
{
	auto n = path.find('$');
	auto e = path.find_first_of('/');

	if (e != std::string::npos)
		return std::string(path.begin() + (n + 1), path.begin() + e);
	else
	{
		auto e = path.find_first_of(FileSystem::path_separator);
		return std::string(path.begin() + (n + 1), path.begin() + e);
	}
}

void CloudManager::autodetect()
{
	providerlist_.reset(new pugi::xml_document());
	if (FileSystem::file_exists("providers.xml"))
	{
		providerlist_->load_file("providers.xml");
		pugi::xpath_node_set provider = providerlist_->select_nodes("//provider");
		for (auto it = provider.begin(); it != provider.end(); ++it)
		{
			pugi::xpath_node node = *it;
			std::pair<std::string, std::string> tmp(
				node.node().attribute("placeholder").value(),
				node.node().attribute("location").value());
			provider_locations_.insert(tmp);
		}
	}
	else
	{
		detect_dropbox();
		detect_onedrive();
		detect_googledrive();
		detect_local();

		pugi::xml_node providers = providerlist_->append_child("providers");
		for (auto& x : provider_locations_)
		{
			pugi::xml_node current = providers.append_child("provider");
			current.append_attribute("placeholder") = x.first.data();
			current.append_attribute("location") = x.second.data();
		}
		providerlist_->save_file("providers.xml");
	}

}

void CloudManager::create_providerlist()
{
	if (FileSystem::file_exists("providers.xml"))
	{
		FileSystem::delete_file("providers.xml");
	}
	auto& manager = instance();
	manager.provider_locations_.clear();
	manager.autodetect();
}

void CloudManager::detect_dropbox()
{
#ifdef _WIN32
	size_t required_size;
	char* appdata;
	getenv_s(&required_size, nullptr, 0, "appdata");
	if (required_size == 0) { return; }
	appdata = new char[required_size];
	getenv_s(&required_size, appdata, required_size, "appdata");
	std::string appd(appdata);
	delete[] appdata;
	appd.append("\\Dropbox\\host.db");
	//if (!FileSystem::file_exists(appd)){ return; }

	std::ifstream in(appd, std::ios::in | std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	in.close();
	auto n = content.find_first_of('\n');
	if (n != std::string::npos)
	{
		content.erase(0, n);
	}
	std::string decoded;
	CryptoPP::StringSource(content, true,
		new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

	provider_locations_["$Dropbox"] = decoded;
#endif
}

void CloudManager::detect_onedrive()
{
	/*size_t required_size;
	char* tmp_systemdrive = nullptr;
	getenv_s(&required_size, nullptr, 0, "systemdrive");
	if (required_size == 0) { return; }
	tmp_systemdrive = new char[required_size];
	getenv_s(&required_size, tmp_systemdrive, required_size, "systemdrive");
	std::string systemdrive(tmp_systemdrive);
	delete[] tmp_systemdrive;

	char* tmp_homepath = nullptr;
	getenv_s(&required_size, nullptr, 0, "homepath");
	if (required_size == 0) { return; }
	tmp_homepath = new char[required_size];
	getenv_s(&required_size, tmp_homepath, required_size, "homepath");
	std::string homepath(tmp_homepath);
	delete[] tmp_homepath;*/
#ifdef _WIN32
	PWSTR pa;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SkyDrive, 0, nullptr, &pa)))
	{
		int lenUnicode = lstrlenW(pa);
		int lenNeeded = WideCharToMultiByte(0, 0, pa, lenUnicode, NULL, 0, NULL, NULL);
		char* x = new char[lenNeeded + 1];
		WideCharToMultiByte(0, 0, pa, lenUnicode, x, lenNeeded, NULL, NULL);
		x[lenNeeded] = '\0';

		provider_locations_["$OneDrive"] = std::string(x);
	}
#endif
}

void CloudManager::detect_googledrive()
{
#ifdef _WIN32
	PWSTR pa;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &pa)))
	{
		int lenUnicode = lstrlenW(pa);
		int lenNeeded = WideCharToMultiByte(0, 0, pa, lenUnicode, NULL, 0, NULL, NULL);
		char* x = new char[lenNeeded + 1];
		WideCharToMultiByte(0, 0, pa, lenUnicode, x, lenNeeded, NULL, NULL);
		x[lenNeeded] = '\0';

		std::string appdatalocal(x);
		appdatalocal.append("\\Google\\Drive\\sync_config.db");
		if (!FileSystem::file_exists(appdatalocal)){ return; }

		std::ifstream in(appdatalocal, std::ios::in | std::ios::binary);
		std::string content((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());
		in.close();
		auto n = content.find_first_of("\\?\\");
		if (n != std::string::npos)
		{
			content.erase(0, n+4);
			auto c = content.find_first_of("Drive");
			if (c != std::string::npos)
			{
				content.erase(c + 6, content.size() - (c + 6));
				auto n = content.find_last_of("\\GoogleDrive");
				if (n != std::string::npos)
				{
					provider_locations_["$GoogleDrive"] = content;
				}
			}
		}
	}
#endif
}

void CloudManager::detect_local()
{
#ifdef _WIN32
	PWSTR pa;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &pa)))
	{
		int lenUnicode = lstrlenW(pa);
		int lenNeeded = WideCharToMultiByte(0, 0, pa, lenUnicode, NULL, 0, NULL, NULL);
		char* x = new char[lenNeeded + 1];
		WideCharToMultiByte(0, 0, pa, lenUnicode, x, lenNeeded, NULL, NULL);
		x[lenNeeded] = '\0';

		provider_locations_["$Local"] = std::string(x);
	}
#endif
}

std::vector<std::pair<std::string, std::string> > CloudManager::get_providers()
{
	std::vector<std::pair<std::string, std::string> > tmp;
	for (auto& e : provider_locations_)
	{
		tmp.push_back({ e.first, e.second });
	}
	return tmp;
}

void CloudManager::add_provider(std::string name_with_sign, std::string location)
{
	provider_locations_[name_with_sign] = location;
	pugi::xml_node providers = providerlist_->child("providers");
	pugi::xml_node current = providers.append_child("provider");
	current.append_attribute("placeholder") = name_with_sign.c_str();
	current.append_attribute("location") = location.c_str();
	providerlist_->save_file("providers.xml");
}

void CloudManager::delete_provider(std::string name_with_sign)
{
	pugi::xpath_variable_set vars;
	vars.add("name_with_sign", pugi::xpath_type_string);
	vars.set("name_with_sign", name_with_sign.c_str());

	provider_locations_.erase(name_with_sign);
	pugi::xml_node providers = providerlist_->child("providers");
	pugi::xpath_node temp_node = providers.select_single_node("/providers/provider[@placeholder=$name_with_sign]", &vars);
	providers.remove_child(temp_node.node());
	providerlist_->save_file("providers.xml");
}

bool CloudManager::contains_provider(std::string name_with_sign)
{
	auto it = std::find_if(std::begin(provider_locations_), std::end(provider_locations_), [&](const std::pair<std::string, std::string>& p)
	{
		return p.first == name_with_sign;
	});

	return it != std::end(provider_locations_);
}
