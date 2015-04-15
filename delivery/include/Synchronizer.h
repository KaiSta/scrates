#pragma once

#include "tempest.h"

//#include "folder_watcher.h"

class Synchronizer
{
public:
	enum listen_to
	{
		VHD,
		CLOUD
	};

	enum watch_event
	{
		UPDATE,
		NEW,
		REMOVED
	};

	struct file_item
	{
		std::string location;
		watch_event ev;
	};

	typedef std::function<void(std::vector<file_item>) > func_t;

	Synchronizer();
	Synchronizer(const std::string& vhd_location, 
		const std::string& cloud_location);

	void init();

	void set_vhdlocation(const std::string& location);
	void set_cloudlocation(const std::string& location);
	void add_listener(func_t func, listen_to location);

	void stop();

	~Synchronizer();

private:
	std::string vhd_location_;
	std::string cloud_location_;

	//folder_watcher cloudwatch_;
	//folder_watcher vhdwatch_;

	//typedef concurrency::concurrent_unordered_map<std::string, std::string> map_type;
	typedef std::unordered_map<std::string, std::string> map_type;
	map_type cloud_list_;
	map_type vhd_list_;

	std::vector < func_t > cloudlistener_;
	std::vector < func_t > vhdlistener_;
	std::mutex cld_mtx_;
	std::mutex vhd_mtx_;
	bool stopped_;

	void update_at_cloud();
	void update_at_vhd();
	void timer_sync();
};

