#include "ContainerController.h"
#include "Path.h"
#include <cryptopp\secblock.h>
#include <cryptopp\osrng.h>

ContainerController::ContainerController(callback_t event_callback, const std::string& vhd_path) : 
event_callback_(event_callback), vhd_path_(vhd_path)
{
}


ContainerController::~ContainerController()
{
}

container_event ContainerController::create(const std::string& container_name, const std::string& container_location,
	const std::string& password, const std::string& sync_location, stor_t store_type, size_t store_size)
{
	container_event ev;
	path filepath = path(container_location).append_filename(container_name + ".cco");
	try
	{
		container_.create(container_name, password, filepath, { std::pair < std::string, size_t >(sync_location, store_size) }, path(vhd_path_), store_type);
		ev.ev_type = event_type::INFORMATION;
		ev.inf = information::SUCC;
	}
	catch (std::invalid_argument e)
	{
		ev.ev_type = event_type::CONFLICT;
		ev.c = conflict::WRONG_ARGUMENTS;
	}
	return ev;
}

bool ContainerController::open(const std::string& container_location, const std::string& password, stor_t store_type)
{
	container_.open(path(container_location), password, path(vhd_path_), store_type);
}

bool ContainerController::sync_now()
{
	container_.manual_sync();
}

std::vector<unsigned char> ContainerController::get_pseudo_seed()
{
	using namespace CryptoPP;
	SecByteBlock b(32);
	AutoSeededRandomPool prng;
	prng.GenerateBlock(b, b.size);
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