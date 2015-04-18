#include "ContainerController.h"
#include "Path.h"
#include <cryptopp/secblock.h>
#include <cryptopp/osrng.h>
#include <string>
#include <limits>
#include "CloudManager.h"

ContainerController::ContainerController(callback_t event_callback, const std::string& vhd_path) :
event_callback_(event_callback), vhd_path_(vhd_path)
{
}


ContainerController::~ContainerController()
{
  container_.close();
}

container_event ContainerController::create(const std::string& container_name, const std::string& container_location,
	const std::string& password, const std::string& sync_location, stor_t store_type, size_t store_size)
{
	container_event ev;
	path filepath = path(container_location).append_filename(container_name + ".cco");

	if(store_size == 0)
	  store_size = std::numeric_limits< int >::max();

	try
	{
	    path v(vhd_path_);
		container_.create(container_name, password, filepath, { std::pair < std::string, size_t >(sync_location, store_size) }, v, store_type, event_callback_);
		send_callback(INFORMATION, SUCC);
	}
	catch (std::invalid_argument e)
	{
		send_callback(CONFLICT, WRONG_ARGUMENTS);
	}
	return ev;
}

bool ContainerController::open(const std::string& container_location, const std::string& password, stor_t store_type)
{
	path v(vhd_path_);
	path cl(container_location);
	container_event ev;
	try
	{
		container_.open(cl, password, v, store_type, event_callback_);
		send_callback(INFORMATION, SUCC);
		return true;
	}
	catch (const std::invalid_argument& e)
	{
		std::string what = e.what();
		send_callback(CONFLICT, WRONG_PASSWORD, what);
		return false;
	}

}

bool ContainerController::sync_now()
{
	send_callback(INFORMATION, SYNCHRONIZING);

	container_.manual_sync();

	send_callback(INFORMATION, FINISHED_SYNCHRONIZING);
	return true;
}

std::vector<unsigned char> ContainerController::get_pseudo_seed()
{
	using namespace CryptoPP;
	SecByteBlock b(32);
	AutoSeededRandomPool prng;
	prng.GenerateBlock(b, b.size());
	return{ std::begin(b), std::end(b) };
}

void ContainerController::set_seed(std::vector<unsigned char>& seed)
{
	//here comes something nice
}

void ContainerController::delete_booked_node(container_event& e)
{
	//some other nice things here
}

void ContainerController::send_callback(event_type t, event_message m, std::string data)
{
	container_event ev;
	ev.type = t;
	ev.message = m;
	ev._data_.insert(ev._data_.begin(), data.begin(), data.end());
	std::thread th(event_callback_, ev);
	th.detach();
}

void ContainerController::refresh_providerlist()
{
	auto& manager = CloudManager::instance();
	manager.create_providerlist();
}

void ContainerController::add_provider(std::string name_with_sign, std::string location)
{
	auto& manager = CloudManager::instance();
	manager.add_provider(name_with_sign, location);
}

void ContainerController::delete_provider(std::string name_with_sign)
{
  auto& manager = CloudManager::instance();
  manager.delete_provider(name_with_sign);
}

bool ContainerController::contains_provider(std::string name_with_sign)
{
  auto& manager = CloudManager::instance();
  return manager.contains_provider(name_with_sign);
}

std::vector<std::pair<std::string, std::string> > ContainerController::get_providers()
{
	auto& manager = CloudManager::instance();
	return manager.get_providers();
}

void ContainerController::close()
{
  container_.close();
}

bool ContainerController::is_open()
{
	return container_.is_open();
}
