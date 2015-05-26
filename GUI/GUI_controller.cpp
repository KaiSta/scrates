#include "GUI_controller.h"


GUI_controller::GUI_controller()
{
}


GUI_controller::~GUI_controller()
{
  sync_all();
  close_all();
}

bool GUI_controller::create_container(const std::string& container_name, const std::string& password, const std::string& sync_location)
{
  auto it = scrates_.find(container_name);
  if (it != std::end(scrates_))
    return false;

  ContainerController* scrate = new ContainerController([=](container_event e) { callback_func(e, container_name); }, "C:\\tmp\\folder");
  scrate->create(container_name, "C:\\tmp\\local", password, sync_location, ContainerController::stor_t::FOLDER, 0);

  scrates_.insert(std::pair<std::string, ContainerController*>(container_name, scrate));

  return true;
}

bool GUI_controller::open_container(const std::string& container_name, const std::string& password)
{
  auto it = scrates_.find(container_name);
  if (it == std::end(scrates_))
    return false;

  if (it->second == nullptr)
  {
    it->second = new ContainerController([=](container_event e) { callback_func(e, container_name); }, "C:\\tmp\\folder");
    std::string loc("C:\\tmp\\local\\");
    loc.append(container_name);
    loc.append(".cco");
    it->second->open(loc, password, ContainerController::stor_t::FOLDER);
  }
  else
    return false; //already opened

  return true;
}

void GUI_controller::sync_all()
{
  for (auto& e : scrates_)
    e.second->sync_now();
}
void GUI_controller::sync(const std::string& container_name)
{
  auto it = scrates_.find(container_name);
  it->second->sync_now();
}
void GUI_controller::close_all()
{
  for (auto& e : scrates_)
  {
    e.second->close();
    e.second = nullptr;
  }
}
void GUI_controller::close(const std::string& container_name)
{
  auto it = scrates_.find(container_name);
  it->second->close();
  it->second = nullptr;
}