#pragma once
#include "tempest.h"

class CloudManager
{
public:
	static CloudManager& instance();
	~CloudManager();

	/**
	  \brief replaces the placeholder inside the given path.
	  \param path with placeholders.
	  \return path with all placeholders replaced if possible.
	*/
	std::string extent(std::string path);

	/**
	  \brief gives the location name to a given path.
	  Given $Dropbox\a\b... it will return Dropbox as location name.
	  \param path with the unknown location name.
	  \reutrn location name.
	*/
	std::string get_location_name(const std::string& path);

	void create_providerlist();
	std::vector<std::pair<std::string,std::string> > get_providers();
	void add_provider(std::string name_with_sign, std::string location);
	void delete_provider(std::string name_with_sign);
	bool contains_provider(std::string name_with_sign);
private:
	CloudManager();
	CloudManager(const CloudManager&);
	std::unordered_map<std::string, std::string> provider_locations_;
	std::unique_ptr<pugi::xml_document> providerlist_;

	void autodetect();
	void detect_dropbox();
	void detect_onedrive();
	void detect_googledrive();
	void detect_local();
};
