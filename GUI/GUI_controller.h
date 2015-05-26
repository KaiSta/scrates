#pragma once
#include <string>
#include <ContainerController.h>
#include <unordered_map>

class GUI_controller
{
public:
  GUI_controller();
  
  bool create_container(const std::string& container_name, const std::string& password, const std::string& sync_location);
  bool open_container(const std::string& container_name, const std::string& password);
  void sync_all();
  void sync(const std::string& container_name);
  void close_all();
  void close(const std::string& container_name);

  std::string get_dropbox_path()
  {
    if (ContainerController::contains_provider("$Dropbox"))
      return "$Dropbox";
    else
      return "$$error, provider not available";
  }

  std::string get_onedrive_path()
  {
    if (ContainerController::contains_provider("$OneDrive"))
      return "$OneDrive";
    else
      return "$$error, provider not available";
  }

  std::string get_google_drive_path()
  {
    if (ContainerController::contains_provider("$GoogleDrive"))
      return "$GoogleDrive";
    else
      return "$$error, provider not available";
  }

  ~GUI_controller();

private:
  std::unordered_map<std::string, ContainerController*> scrates_;
  void callback_func(container_event e, std::string container_name)
  {}
};

