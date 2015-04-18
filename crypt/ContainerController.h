#pragma once

#include "tempest.h"
#include "MessageTypes.h"
#include "local_file.h"

class ContainerController
{
public:
	typedef std::function<void(container_event)> callback_t;
	typedef local_file::storage_type stor_t;

	/**
	 * @param event_callback function to call for status informations. Must have the signature described in callback_t.
	 * @param vhd_path if a vhd is used, the vhd file will be stored in the given path, otherwise a folder will be created in this location where the mounted files are stored.
	 */
	ContainerController(callback_t event_callback, const std::string& vhd_path);
	~ContainerController();

	/**
	 * \brief creates a new container
	 * @param container_name only the name of the container
	 * @param container_location path to the location where the local file will be stored
	 * @param password used passphrase
	 * @param sync_location path where the encrypted files shall be stored (the cloud folder mostly)
	 * @param store_type can be either FOLDER or VHD.
	 * @param store_size if store_type is VHD, the store size must be set (bytes), for folders use 0.
	 */
	container_event create(const std::string& container_name, const std::string& container_location,
		const std::string& password, const std::string& sync_location, stor_t store_type, size_t store_size);

	/**
	 * \brief opens the container in the given location.
	 * @param containerlocation path to the local container file.
	 * @param password passphrase to open it.
	 * @param store_type can be either FOLDER or VHD
	 */
	bool open(const std::string& containerlocation, const std::string& password, stor_t store_type);

	/**
	 * \brief forces sync between mount location and cloud location.
	 */
	bool sync_now();

	/**
	 * \brief returns a pre initialized cryptographic seed with 32 bytes.
	 */
	std::vector<unsigned char> get_pseudo_seed();

	/**
	 * Incorporates the given seed when a new container is created.
	 */
	void set_seed(std::vector<unsigned char>& seed);

	/**
	 * not implemented!
	 */
	void delete_booked_node(container_event& e);

	/**
	 * \brief Deletes the old providerlist and tries to detect them again.
	 * providers.xml will be deleted and recreated! Ensure that open handles are closed and reopened!
	 */
	void refresh_providerlist();

	/**
	 * \brief adds a new storage location with a defined shortcut name
	 *
	 * @param name_with_sign is the shortcut name, beginning with $.
	 * @param location is the path to the location that is used to replace the shortcutname
	 */
	void add_provider(std::string name_with_sign, std::string location);

	/**
	* \brief removes a provider.
	*
	* @param name_with_sign is the shortcut name, beginning with $.
	*/
	void delete_provider(std::string name_with_sign);

	/**
	* \brief checks if the providerlist contains a given provider.
	*
	* @param name_with_sign is the shortcut name, beginning with $.
	*/
	bool contains_provider(std::string name_with_sign);

	/**
	 * Returns all providers, first string contains the shortcutname beginning with $
	 * second string contains the path
	 */
	std::vector<std::pair<std::string, std::string> > get_providers();

  void close();

	/**
	 * Returns whether the container is mounted or not.
	 */
	bool is_open();

private:
	callback_t event_callback_;
	local_file container_;
	std::string vhd_path_;

	void send_callback(event_type t, event_message m, std::string data="");
};
