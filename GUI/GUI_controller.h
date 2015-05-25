#pragma once
#include <string>

class GUI_controller
{
public:
  GUI_controller();
  
  bool create_container(const std::string& container_name, const std::string& password, const std::string& sync_location)
  {
    return true;
  }

  std::string get_dropbox_path()
  {
    return "$Dropbox";
  }

  std::string get_onedrive_path()
  {
    return "$Onedrive";
  }

  std::string get_google_drive_path()
  {
    return "$Googledrive";
  }

  ~GUI_controller();
};

