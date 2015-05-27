#include "GUI_controller.h"


GUI_controller::GUI_controller()
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
  FileSystem::make_folders("C:\\tmp\\Scrates\\mounted");
  CloudManager& mgr = CloudManager::instance();

  auto files = FileSystem::list_files("C:\\tmp\\Scrates", false);
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

  ContainerController* scrate = new ContainerController([=](container_event e) { callback_func(e, container_name); }, "C:\\tmp\\Scrates\\mounted\\");
  scrate->create(container_name, "C:\\tmp\\Scrates", password, sync_location, ContainerController::stor_t::FOLDER, 0);

  scrates_.push_back(std::pair<std::string, ContainerController*>(container_name, scrate));
  gui_update_();
  return true;
}

bool GUI_controller::open_container(const std::string& container_name, const std::string& password)
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

  if (it->second == nullptr)
  {
    it->second = new ContainerController([=](container_event e) { callback_func(e, container_name); }, "C:\\tmp\\Scrates\\mounted\\");
    std::string loc("C:\\tmp\\Scrates\\");
    loc.append(container_name);
    loc.append(".cco");
    it->second->open(loc, password, ContainerController::stor_t::FOLDER);
  }
  else
    return false; //already opened
  
  gui_update_();
  return true;
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