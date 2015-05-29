#pragma once

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include <wx/frame.h>
#include <memory>
#include <thread>
#include <future>
#include "GUI_controller.h"
#include "PasswordDialog.h"
#include "LogDialog.h"

class MainFrame : public wxFrame
{
  enum ids
  {
    id_menu_quit,
    id_menu_about,
    id_menu_options,
    id_menu_log,
    id_new_container,
    id_open_container,
    id_close_container,
    id_container_grid,
    id_sync_all,
    id_sync_container
  };

public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, GUI_controller* ctrl) : controller_(ctrl),
    wxFrame(nullptr, wxID_ANY, title, pos, size)
  {
    SetBackgroundColour(*wxWHITE);
    controller_->set_gui_update(std::bind(&MainFrame::update_scrates_view, this));

    bar_ = new wxMenuBar;
    fileMenu_ = new wxMenu(_T(""));
    fileMenu_->Append(id_new_container, _("&New"), _("Create a new Scrate"));
    Connect(id_new_container, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnNew));
    fileMenu_->AppendSeparator();
    fileMenu_->Append(id_menu_quit, _("&Quit\tAlt-F4"), _("Quit Scrates"));
    Connect(id_menu_quit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnExit));
    bar_->Append(fileMenu_, _("&File"));
    extraMenu_ = new wxMenu(_T(""));
    extraMenu_->Append(id_sync_all, _("&Sync all\tAlt-s"), _("Sync all scrates"));
    Connect(id_sync_all, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSyncAll));
    extraMenu_->Append(id_menu_options, _("&Settings\tAlt-o"), _("Scrates Settings"));
    Connect(id_menu_options, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSettings));
    extraMenu_->Append(id_menu_log, _("&Log\tAlt-l"), _("Log file"));
    Connect(id_menu_log, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnLog));
    bar_->Append(extraMenu_, _("&Extras"));
    helpMenu_ = new wxMenu(_T(""));
    helpMenu_->Append(id_menu_about, _("&About\tF1"), _("About Scrates"));
    bar_->Append(helpMenu_, _("&Help"));
    SetMenuBar(bar_);

    // new_container_ = new wxButton(this, id_new_container, _("New"));
    open_container_ = new wxButton(this, id_open_container, _("Open"));
    Connect(id_open_container, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnOpenContainer));
    sync_all_ = new wxButton(this, id_sync_container, _("Sync"));
    Connect(id_sync_container, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnSyncContainer));
    close_container_ = new wxButton(this, id_close_container, _("Close"));
    Connect(id_close_container, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnCloseContainer));

    container_view_ = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    container_view_->SetMinSize(wxSize(600, 300));
    container_view_name_ = container_view_->AppendTextColumn(wxT("Name"));
    container_view_status_ = container_view_->AppendTextColumn(wxT("Status"));
    container_view_log_ = container_view_->AppendTextColumn(wxT("Log"));

    sizer_ = new wxBoxSizer(wxVERTICAL);
    button_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    sizer_->Add(button_sizer_, 0, wxALL, 5);
    sizer_->Add(container_view_, 1, wxALL | wxEXPAND, 5);
    // button_sizer_->Add(new_container_, 0, wxALL, 5);
    button_sizer_->Add(open_container_, 0, wxALL, 5);
    button_sizer_->Add(sync_all_, 0, wxALL, 5);
    button_sizer_->Add(close_container_, 0, wxALL, 5);

    // wizard = new ContainerWizard(wxT("Wizard"), wxDefaultPosition, wxDefaultSize);
    std::vector<std::pair<std::string, std::string> > files;
    controller_->get_containers(files);
    for (auto& e : files)
    {
      wxVector<wxVariant> item;
      item.push_back(wxVariant(e.first));
      item.push_back(e.second);
      item.push_back("bla");
      container_view_->AppendItem(item);
    }

    SetSizer(sizer_);
    Layout();
    SetMinSize(wxSize(640, 480));
    Fit();
  }

  ~MainFrame()
  {
  }

protected:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event)
  {
    Close(true);
  }
  void OnAbout(wxCommandEvent& event);

  void OnNew(wxCommandEvent& event)
  {
    wizard_ = new ContainerWizard(wxT("Wizard"), wxDefaultPosition, wxDefaultSize, controller_);
    wizard_->Show(true);
    update_scrates_view();
  }
  void OnSettings(wxCommandEvent& event)
  {

  }
  void OnCloseContainer(wxCommandEvent& event)
  {
    auto selected = container_view_->GetSelectedRow();
    if (selected != wxNOT_FOUND)
    {
      auto val = container_view_->GetTextValue(selected, 0);
      controller_->close(std::string(val.mb_str()));
      container_view_->SetFocus();
      container_view_->SelectRow(selected);
    }
  }
  void OnOpenContainer(wxCommandEvent& event)
  {
    auto selected = container_view_->GetSelectedRow();
    if (selected != wxNOT_FOUND)
    {
      auto val = container_view_->GetTextValue(selected, 0);
      std::string pw;
      pwdialog_ = new PasswordDialog("Enter Password", pw);
      pwdialog_->Show(true);
      
      if (!controller_->open_container(std::string(val.mb_str()), pw))
      {
        wxMessageBox(wxT("Wrong password"), wxT("Error"), wxICON_ERROR);
      }
      container_view_->SetFocus();
      container_view_->SelectRow(selected);
    }
  }
  void OnSyncContainer(wxCommandEvent& event)
  {
    auto selected = container_view_->GetSelectedRow();
    if (selected != wxNOT_FOUND)
    {
      auto val = container_view_->GetTextValue(selected, 0);
      controller_->sync(std::string(val.mb_str()));
      container_view_->SetFocus();
      container_view_->SelectRow(selected);
    }
  }
  void OnSyncAll(wxCommandEvent& event)
  {
    controller_->sync_all();
  }
  void OnLog(wxCommandEvent& event)
  {
    auto logd = new LogDialog("Log", wxDefaultPosition, wxDefaultSize, controller_);
    logd->Show(true);
  }

private:
  //menu bar
  wxMenuBar* bar_;
  wxMenu* fileMenu_;
  wxMenu* helpMenu_;
  wxMenu* extraMenu_;

  //sizer
  wxBoxSizer* sizer_;
  wxBoxSizer* button_sizer_;

  //elements
  wxButton* new_container_;
  wxButton* open_container_;
  wxButton* close_container_;
  wxButton* sync_all_;

  wxDataViewListCtrl* container_view_;
  wxDataViewColumn* container_view_name_;
  wxDataViewColumn* container_view_status_;
  wxDataViewColumn* container_view_log_;

  ContainerWizard* wizard_;
  PasswordDialog* pwdialog_;
  GUI_controller* controller_;
  //std::unique_ptr<ContainerWizard> wizard_;

  void update_scrates_view()
  {
    container_view_->DeleteAllItems();
    std::vector<std::pair<std::string, std::string> > files;
    controller_->get_containers(files);
    for (auto& e : files)
    {
      wxVector<wxVariant> item;
      item.push_back(wxVariant(e.first));
      item.push_back(e.second);
      item.push_back("bla");
      container_view_->AppendItem(item);
    }
  }
};