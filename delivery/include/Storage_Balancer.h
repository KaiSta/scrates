#pragma once

#include <pugixml.hpp>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <stdint.h>

#ifdef _WIN32
#include <time.h>
#endif

class storage_balancer
{
public:
	static storage_balancer& instance()
	{
		static storage_balancer inst;
		return inst;
	}


	//deprecated
	bool has_space(pugi::xml_document& container_file, const std::string& location, int64_t size)
	{
		std::string xpath_q("/container/locations/location[@name = \"" + location + "\" ]");
		pugi::xpath_node_set locations = container_file.select_nodes(xpath_q.data());

		if (locations.size() == 1)
		{
			size_t used = std::stoi((*locations.begin()).node().attribute("used").value());
			size_t available = std::stoi((*locations.begin()).node().attribute("quota").value());

			return (available > (used + size));
		}
		return false;
	}


	//deprecated
	bool update_location(pugi::xml_document& container_file, const std::string& location, int64_t size)
	{
		if (has_space(container_file, location, size))
		{
			std::string xpath_q("/container/locations/location[@name = \"" + location + "\" ]");
			pugi::xpath_node_set locations = container_file.select_nodes(xpath_q.data());
			size_t used = std::stoi((*locations.begin()).node().attribute("used").value());
			size_t available = std::stoi((*locations.begin()).node().attribute("quota").value());

			used += size;
			((*locations.begin()).node().remove_attribute("used"));
			std::stringstream ss;
			ss << used;
			(*locations.begin()).node().append_attribute("used") = ss.str().data();
			return true;
		}
		return true;
	}

private:
	storage_balancer()
	{
	}
	storage_balancer(const storage_balancer&);
	storage_balancer& operator=(const storage_balancer&);
};