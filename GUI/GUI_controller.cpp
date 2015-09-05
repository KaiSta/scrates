#include "GUI_controller.h"


GUI_controller::GUI_controller() : config_(new Poco::Util::IniFileConfiguration("config.ini"))
{
}


GUI_controller::~GUI_controller()
{
  sync_all();
  for (auto& e : scrates_)
  {
    if (e.second)
    {
      e.second->close();
      e.second = nullptr;
    }
  }
}

void GUI_controller::init()
{
  //FileSystem::make_folders("C:\\tmp\\Scrates\\mounted");
  FileSystem::make_folders(Poco::Path::expand(config_->getString("Scrates.MountPath")));
  CloudManager& mgr = CloudManager::instance();

 // auto files = FileSystem::list_files("C:\\tmp\\Scrates", false);
  auto files = FileSystem::list_files(Poco::Path::expand(config_->getString("Scrates.ScrateLocation")), false);
  for (auto& e : files)
  {
    path p(e);
    scrates_.push_back(std::pair<std::string, ContainerController*>(p.get_filename().substr(0, p.get_filename().size() - 4), nullptr));
    //cts.push_back(p.get_filename().substr(0, p.get_filename().size() - 4));
  }
}

void GUI_controller::exit()
{
  sync_all();
  for (auto& e : scrates_)
  {
    if (e.second)
    {
      e.second->close();
      e.second = nullptr;
    }
  }
}

void GUI_controller::set_gui_update(std::function<void()> f)
{
  gui_update_ = f;
}

bool GUI_controller::create_container(const std::string& container_name, const std::string& password, const std::string& sync_location)
{
  std::pair<std::string, ContainerController*>* it = nullptr;
  for (auto& e : scrates_)
  {
    if (e.first == container_name && e.second == nullptr)
    {
      it = &e;
      break;
    }
  }
  if (it)
    return false;

  ContainerController* scrate = new ContainerController([=](container_event e) { callback_func(e, container_name); }, 
    Poco::Path::expand(config_->getString("Scrates.MountPath"))/*"C:\\tmp\\Scrates\\mounted\\"*/);
  scrate->create(container_name, Poco::Path::expand(config_->getString("Scrates.ScrateLocation"))/*"C:\\tmp\\Scrates"*/, password, sync_location, ContainerController::stor_t::FOLDER, 0);

  scrates_.push_back(std::pair<std::string, ContainerController*>(container_name, scrate));
  gui_update_();
  return true;
}

bool GUI_controller::open_container(const std::string& container_name, const std::string& password, std::string default_path)
{
  std::pair<std::string, ContainerController*>* it = nullptr;
  for (auto& e : scrates_)
  {
    if (e.first == container_name && e.second == nullptr)
    {
      it = &e;
      break;
    }
  }
  if (!it)
    return false;

  bool succ = false;

  if (it->second == nullptr)
  {
    std::string mountpath = (default_path == "") ? config_->getString("Scrates.MountPath") : default_path;
    mountpath = Poco::Path::expand(mountpath);
    it->second = new ContainerController([=](container_event e) { callback_func(e, container_name); }, mountpath/*Poco::Path::expand(config_->getString("Scrates.MountPath")*/);
      /*"C:\\tmp\\Scrates\\mounted\\"*///);
   // std::string loc("C:\\tmp\\Scrates\\");
    std::string loc(Poco::Path::expand(config_->getString("Scrates.ScrateLocation")));
    loc.append(container_name);
    loc.append(config_->getString("Scrates.FileExtension"));
    //loc.append(".cco");
    succ = it->second->open(loc, password, ContainerController::stor_t::FOLDER);
    if (!succ)
    {
      delete it->second;
      it->second = nullptr;
    }
  }
  else
    return false; //already opened
  
  gui_update_();
  return succ;
}

void GUI_controller::sync_all()
{
  for (auto& e : scrates_)
  {
    if (e.second)
      e.second->sync_now();
  }
}
void GUI_controller::sync(const std::string& container_name)
{
  for (auto& e : scrates_)
  {
    if (e.first == container_name)
    {
      e.second->sync_now();
      break;
    }
  }
}
void GUI_controller::close_all()
{
  for (auto& e : scrates_)
  {
    e.second->close();
    e.second = nullptr;
  }
  gui_update_();
}
void GUI_controller::close(const std::string& container_name)
{
  for (auto& e : scrates_)
  {
    if (e.first == container_name)
    {
      e.second->close();
      e.second = nullptr;
      break;
    }
  }
  gui_update_();
}

void GUI_controller::get_containers(std::vector<std::pair<std::string, std::string> >& cts)
{
  for (auto& e : scrates_)
  {
    cts.push_back({ e.first, (e.second) ? "open" : "closed" });
  }
}

void GUI_controller::callback_func(container_event e, std::string container_name)
{
  std::lock_guard<std::mutex> guard(mtx_);
  std::string type;
  std::string message;
  switch (e.type)
  {
  case INFORMATION:
    type = "information";
    break;
  case CONFLICT:
    type = "error";
    break;
  case WARNING:
    type = "warning";
    break;
  case VERBOSE:
    type = "verbose";
    break;
  }

  switch (e.message)
  {
  case NONE:
    message = "none";
    break;
  case CTR_FILE_NOT_FOUND:
    message = "container file not found";
    break;
  case WRONG_PASSWORD:
    message = "wrong password";
    break;
  case MISSING_ENC_FILES:
    message = "missing encrypted file in cloud";
    break;
  case DECRYPTION_ERROR:
    message = "decryption error";
    break;
  case WRONG_ARGUMENTS:
    message = "wrong arguments";
    break;
  case SUCC:
    message = "succ";
    break;
  case SYNCHRONIZING:
    message = "sync";
    break;
  case FINISHED_SYNCHRONIZING:
    message = "finished sync";
    break;
  case NO_WARNING:
    message = "no warning";
    break;
  case CLOSING:
    message = "closing container";
    break;
  case ADD_FILE:
    message = "adding file " + std::string(e._data_.data());
    break;
  case UPDATE_FILE:
    message = "updating file " + std::string(e._data_.data());
    break;
  case DELETE_FILE:
    message = "delete file " + std::string(e._data_.data());
    break;
  case EXTRACT_FILE:
    message = "extract file " + std::string(e._data_.data());
    break;
  case EXTRACT_FILES:
    message = "extract files ";
    break;
  case INIT_CONTAINER:
    message = "initializing container";
    break;
  }
  //logfile << type << " - " << message << std::endl;
  logs_.push_back({ container_name, message });
  //log_update_(container_name);
}

void GUI_controller::set_log_update(std::function<void(std::string)> f)
{
  log_update_ = f;
}

std::vector<std::pair<std::string, std::string > > GUI_controller::get_logs(std::string filter)
{
  if (filter == "*")
    return logs_;
  else
  {
    std::vector<std::pair<std::string, std::string> > ret;
    for (auto& e : logs_)
    {
      if (e.first == filter)
        ret.push_back(e);
      else if (e.second.find(filter) != std::string::npos)
        ret.push_back(e);
    }
    return ret;
  }
}

std::string GUI_controller::get_path_for(std::string containername)
{
  if (FileSystem::file_exists("settings.xml"))
  {
    pugi::xml_document doc;
    doc.load_file("settings.xml");
    pugi::xpath_node_set provider = doc.select_nodes(std::string("//locations/location[@name = \"" + containername + "\"]").c_str());
    if (provider.size())
    {
      return provider[0].node().attribute("path").as_string();
    }
  }
  
  auto path = Poco::Path::expand(config_->getString("Scrates.MountPath"));
  Poco::Path p(config_->getString("Scrates.MountPath"));
  //p.append(containername);
  return p.toString();
}

void GUI_controller::set_path_for(std::string containername, std::string path)
{
  pugi::xml_document doc;
  pugi::xml_node root;
  if (FileSystem::file_exists("settings.xml"))
  {
    doc.load_file("settings.xml");
  }
  else
  {
    auto root = doc.append_child("settings");
    auto locs = root.append_child("locations");
    doc.save_file("settings.xml");
  }

  pugi::xpath_node_set found = doc.select_nodes(std::string("/settings/locations/location[@name = \"" + containername + "\"]").c_str());
  root = doc.child("settings").child("locations");
  if (found.size())
  {
    root.remove_child(found[0].node());
  }

  auto nnode = root.append_child("location");
  doc.save_file("settings.xml");
  nnode.append_attribute("name") = containername.c_str();
  nnode.append_attribute("path") = path.c_str();
  doc.save_file("settings.xml");
}

bool GUI_controller::is_open(std::string containername)
{
  for (auto& e : scrates_)
  {
    if (e.first == containername)
    {
      return e.second != nullptr;
    }
  }
  return false;
}

void GUI_controller::open_container_folder(std::string containername)
{
  for (auto& e : scrates_)
  {
    if (e.first == containername)
    {
      auto p = e.second->get_vhd_path() + FileSystem::path_separator + containername;
#ifdef _WIN32
      ShellExecute(nullptr, nullptr, p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#endif
    }
  }
}