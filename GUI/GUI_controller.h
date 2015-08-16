#pragma once
#include <string>
#include <ContainerController.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <Poco/Util/IniFileConfiguration.h>

class GUI_controller
{
public:
  GUI_controller();

  void init();
  
  bool create_container(const std::string& container_name, const std::string& password, const std::string& sync_location);
  bool open_container(const std::string& container_name, const std::string& password);
  void sync_all();
  void sync(const std::string& container_name);
  void close_all();
  void close(const std::string& container_name);
  void set_gui_update(std::function<void()> f);
  void set_log_update(std::function<void(std::string)> f);
  std::string get_path_for(std::string containername);

  void exit();

  void get_containers(std::vector<std::pair<std::string, std::string> >& cts);

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

  std::vector<std::pair<std::string, std::string > > get_logs(std::string filter);
  std::string get_last_log(std::string scrate_name);

  ~GUI_controller();

private:
//  std::unordered_map<std::string, ContainerController*> scrates_;
  std::vector<std::pair<std::string, ContainerController*> > scrates_;
  void callback_func(container_event e, std::string container_name);
  std::function<void()> gui_update_;
  std::function<void(std::string)> log_update_;
  
  Poco::AutoPtr<Poco::Util::IniFileConfiguration> config_;
  std::vector<std::pair<std::string, std::string > > logs_;
  std::mutex mtx_;
};

