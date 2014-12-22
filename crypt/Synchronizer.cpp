#include "Synchronizer.h"

#include "helper_files.h"
#include <stdexcept>
#include <cryptopp\cryptlib.h>
#include <cryptopp\files.h>
#include <cryptopp\filters.h>
#include <cryptopp\crc.h>
#include <cryptopp\hex.h>
#include <thread>
#include <chrono>
#include "FileSystem.h"

Synchronizer::Synchronizer()
{
}

Synchronizer::Synchronizer(const std::string& vhd_location, const std::string& cloud_location) :
	vhd_location_(vhd_location), cloud_location_(cloud_location),
	vhdwatch_(vhd_location), cloudwatch_(cloud_location), stopped_(false)
{
	vhdwatch_.set_filter(folder_watcher::filter::WRITES_ONLY);
	vhdwatch_.set_callback(std::bind(&Synchronizer::update_at_vhd, this));

	cloudwatch_.set_filter(folder_watcher::filter::WRITES_ONLY);
	cloudwatch_.set_callback(std::bind(&Synchronizer::update_at_cloud, this));
}

void Synchronizer::init()
{
	using namespace CryptoPP;

	auto cloudfiles = FileSystem::list_files(cloud_location_, true);

//#pragma omp parallel for
	for (int i = 0; i < cloudfiles.size(); ++i)
	{
		std::pair<std::string, std::string> tmp;
		tmp.first = cloudfiles[i];

		std::string crc;
		if (!secure_crc(cloudfiles[i], crc))
		{
			continue;
		}

		tmp.second = crc;

		cloud_list_.insert(tmp);
	}

	auto vhdfiles = FileSystem::list_files(vhd_location_, true);

//#pragma omp parallel for
	for (int i = 0; i < vhdfiles.size(); ++i)
	{
		std::pair<std::string, std::string> tmp;
		tmp.first = vhdfiles[i];

		std::string crc;
		if (!secure_crc(vhdfiles[i], crc))
		{
			continue;
		}

		tmp.second = crc;

		vhd_list_.insert(tmp);
	}

	//vhdwatch_.run();
	//cloudwatch_.run();

	timer_sync();
}

void Synchronizer::set_vhdlocation(const std::string& location)
{
	vhd_location_ = location;
	vhdwatch_.set_directory(vhd_location_);
	
	vhdwatch_.set_filter(folder_watcher::filter::WRITES_ONLY);
	vhdwatch_.set_callback(std::bind(&Synchronizer::update_at_vhd, this));
}

void Synchronizer::set_cloudlocation(const std::string& location)
{
	cloud_location_ = location;
	cloudwatch_.set_directory(cloud_location_);
	cloudwatch_.set_filter(folder_watcher::filter::WRITES_ONLY);
	cloudwatch_.set_callback(std::bind(&Synchronizer::update_at_cloud, this));
}

void Synchronizer::add_listener(func_t func, listen_to location)
{
	switch (location)
	{
	case listen_to::VHD:
		vhdlistener_.push_back(func);
		break;
	case listen_to::CLOUD:
		cloudlistener_.push_back(func);
		break;
	default:
		throw std::invalid_argument("invalid listen location");
	}
}

Synchronizer::~Synchronizer()
{
	stop();
	/*vhdwatch_.stop();
	cloudwatch_.stop();*/
}

void Synchronizer::update_at_cloud()
{
	using namespace CryptoPP;

	std::lock_guard<std::mutex> guard(vhd_mtx_);

	auto files = FileSystem::list_files(cloud_location_, true);
	std::vector<file_item> update;

	#pragma omp parallel for
	for (int i = 0; i < files.size(); ++i)
	{

		file_item item;

		auto it = cloud_list_.find(files[i]);
		item.ev = (it == cloud_list_.end()) ? watch_event::NEW : watch_event::UPDATE;
		item.location = files[i];

		std::string crc;
		if (!secure_crc(files[i], crc))
		{
			continue;
		}
		

		if (item.ev == NEW)
		{
			std::pair<std::string, std::string> tmp;
			tmp.first = item.location;
			tmp.second = crc;
			cloud_list_.insert(tmp);

			#pragma omp critical
			{
				update.push_back(item);
			}
		}
		else
		{
			std::string old_crc = cloud_list_.at(item.location);
			if (old_crc != crc)
			{
				cloud_list_[item.location] = crc;

				#pragma omp critical
				{
					update.push_back(item);
				}
			}
		}
	}

	for (auto& e : cloud_list_)
	{
		auto it = std::find_if(files.begin(), files.end(), 
			[&](const std::string& str)
		{
			return str == e.first;
		});

		if (it == files.end())
		{
			//removed file found
			file_item item;
			item.ev = REMOVED;
			item.location = e.first;
			update.push_back(item);
			cloud_list_.unsafe_erase(e.first);
		}
	}

	for (auto& e : cloudlistener_)
	{
		e(update);
	}
}

void Synchronizer::update_at_vhd()
{
	using namespace CryptoPP;

	std::lock_guard<std::mutex> guard(vhd_mtx_);

	auto files = FileSystem::list_files(vhd_location_, true);
	std::vector<file_item> update;

	#pragma omp parallel for
	for (int i = 0; i < files.size(); ++i)
	{
		file_item item;

		auto it = vhd_list_.find(files[i]);
		item.ev = (it == vhd_list_.end()) ? watch_event::NEW : watch_event::UPDATE;
		item.location = files[i];

		std::string crc;
		if (!secure_crc(files[i], crc))
		{
			continue;
		}

		if (item.ev == NEW)
		{
			std::pair<std::string, std::string> tmp;
			tmp.first = item.location;
			tmp.second = crc;
			vhd_list_.insert(tmp);

			#pragma omp critical
			{
				update.push_back(item);
			}
		}
		else
		{
			std::string old_crc = vhd_list_.at(item.location);
			if (old_crc != crc)
			{
				vhd_list_[item.location] = crc;

				#pragma omp critical
				{
					update.push_back(item);
				}
			}
		}
	}

	for (auto& e : vhd_list_)
	{
		auto it = std::find_if(files.begin(), files.end(),
			[&](const std::string& str)
		{
			return str == e.first;
		});

		if (it == files.end())
		{
			//removed file found
			file_item item;
			item.ev = REMOVED;
			item.location = e.first;
			update.push_back(item);
			vhd_list_.unsafe_erase(e.first);
		}
	}

	for (auto& e : vhdlistener_)
	{
		e(update);
	}
}

void Synchronizer::timer_sync()
{
	std::thread t([&]() {
		while (!stopped_)
		{
			std::this_thread::sleep_for(std::chrono::seconds(30));
			std::vector<file_item> update;
			for (auto& e : cloudlistener_)
			{
				e(update);
			}
			for (auto& e : vhdlistener_)
			{
				e(update);
			}			
		}
	});
	t.detach();
}

void Synchronizer::stop()
{
	stopped_ = true;
	//cloudwatch_.stop();
	//vhdwatch_.stop();
}