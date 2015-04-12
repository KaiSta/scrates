#pragma once

#include "tempest.h"
#include "MessageTypes.h"
#include "local_file.h"

class ContainerController
{
public:
	typedef std::function<void(container_event)> callback_t;
	typedef local_file::storage_type stor_t;

	ContainerController(callback_t event_callback, const std::string& vhd_path);
	~ContainerController();

	container_event create(const std::string& container_name, const std::string& container_location,
		const std::string& password, const std::string& sync_location, stor_t store_type, size_t store_size);
	bool open(const std::string& containerlocation, const std::string& password, stor_t store_type);
	bool sync_now();
	std::vector<unsigned char> get_pseudo_seed();
	void set_seed(std::vector<unsigned char>& seed);

	void delete_booked_node(container_event& e);
	void refresh_providerlist();
	void add_provider(std::string name_with_sign, std::string location);
	std::vector<std::pair<std::string, std::string> > get_providers();

private:
	callback_t event_callback_;
	local_file container_;
	std::string vhd_path_;

	void send_callback(event_type t, event_message m, std::string data="");
};

