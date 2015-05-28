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
#include <functional>
#include "GUI_controller.h"

class ContainerWizard : public wxDialog
{
  enum ids
  {
    id_label_containername,
    id_txtctrl_containername,
    id_label_password,
    id_txtctrl_password,
    id_label_cloud,
    id_txtctrl_cloud,
    id_button_dropbox,
    id_button_onedrive,
    id_button_googledrive,
    id_label_seed,
    id_label_seed_val,
    id_button_ok,
    id_button_abort
  };

public:
  ContainerWizard(const wxString& title, const wxPoint& pos, const wxSize& size, GUI_controller* ctrl) : controller_(ctrl),
    wxDialog(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
  {
    SetBackgroundColour(*wxWHITE);
    wxImage::AddHandler(new wxPNGHandler);
    sizer_ = new wxBoxSizer(wxVERTICAL);
    col1_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    col2_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    col3_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    col4_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    col5_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    label_containername_ = new wxStaticText(this, id_label_containername, wxT("Scrate name:"));
    label_containername_->SetMinSize(wxSize(150, -1));
    textctrl_containername_ = new wxTextCtrl(this, id_txtctrl_containername, wxEmptyString);
    textctrl_containername_->SetMinSize(wxSize(400, 32));

    label_password_ = new wxStaticText(this, id_label_password, wxT("Password:"));
    label_password_->SetMinSize(wxSize(150, -1));
    txtctrl_password_ = new wxTextCtrl(this, id_txtctrl_password, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    txtctrl_password_->SetMinSize(wxSize(400, 32));

    label_sync_loc_ = new wxStaticText(this, id_label_cloud, wxT("Cloud:"));
    label_sync_loc_->SetMinSize(wxSize(150, -1));
    txtctrl_cloud_loc_ = new wxTextCtrl(this, id_txtctrl_cloud, wxEmptyString);
    txtctrl_cloud_loc_->SetMinSize(wxSize(-1, 32));
    button_dropbox_ = new wxBitmapButton(this, id_button_dropbox,
      wxBitmap(wxT("icons\\dropbox.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition,
      wxDefaultSize, wxBU_AUTODRAW);
    Connect(id_button_dropbox, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ContainerWizard::set_dropbox));
    button_onedrive_ = new wxBitmapButton(this, id_button_onedrive,
      wxBitmap(wxT("icons\\skydrive.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition,
      wxDefaultSize, wxBU_AUTODRAW);
    Connect(id_button_onedrive, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ContainerWizard::set_onedrive));
    button_googledrive_ = new wxBitmapButton(this, id_button_googledrive,
      wxBitmap(wxT("icons\\googledrive.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition,
      wxDefaultSize, wxBU_AUTODRAW);
    Connect(id_button_googledrive, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ContainerWizard::set_googledrive));

    label_seed_ = new wxStaticText(this, id_label_seed, wxT("Seed:"));
    label_seed_->SetMinSize(wxSize(150, -1));
    label_seed_value_ = new wxStaticText(this, id_label_seed_val, wxT("AABBCCDDEEFFGGHHIIJJKKLLMMNNOO"));

    button_ok_ = new wxButton(this, id_button_ok, wxT("OK"));
    Connect(id_button_ok, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ContainerWizard::create));
    button_abort_ = new wxButton(this, id_button_abort, wxT("Abort"));
    Connect(id_button_abort, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ContainerWizard::close));

    col1_sizer_->Add(label_containername_, 0, wxALL, 5);
    col1_sizer_->Add(textctrl_containername_, 0, wxALL, 5);

    col2_sizer_->Add(label_password_, 0, wxALL, 5);
    col2_sizer_->Add(txtctrl_password_, 0, wxALL, 5);

    col3_sizer_->Add(label_sync_loc_, 0, wxALL, 5);
    col3_sizer_->Add(txtctrl_cloud_loc_, 0, wxALL, 5);
    col3_sizer_->Add(button_dropbox_, 0, wxALL, 5);
    col3_sizer_->Add(button_onedrive_, 0, wxALL, 5);
    col3_sizer_->Add(button_googledrive_, 0, wxALL, 5);

    col4_sizer_->Add(label_seed_, 0, wxALL, 5);
    col4_sizer_->Add(label_seed_value_, 0, wxALL, 5);

    col5_sizer_->Add(button_ok_, 0, wxALL, 5);
    col5_sizer_->Add(button_abort_, 0, wxALL, 5);

    sizer_->Add(col1_sizer_, 1, wxALL, 5);
    sizer_->Add(col2_sizer_, 1, wxALL, 5);
    sizer_->Add(col3_sizer_, 1, wxALL, 5);
    sizer_->Add(col4_sizer_, 1, wxALL, 5);
    sizer_->Add(col5_sizer_, 1, wxALL, 5);

    SetSizer(sizer_);
    Layout();
    Fit();
    //Centre();
    ShowModal();
  }

private:
  wxBoxSizer* sizer_;
  wxBoxSizer* col1_sizer_;
  wxBoxSizer* col2_sizer_;
  wxBoxSizer* col3_sizer_;
  wxBoxSizer* col4_sizer_;
  wxBoxSizer* col5_sizer_;

  wxStaticText* label_containername_;
  wxTextCtrl* textctrl_containername_;

  wxStaticText* label_password_;
  wxTextCtrl* txtctrl_password_;

  wxStaticText* label_sync_loc_;
  wxTextCtrl* txtctrl_cloud_loc_;
  wxBitmapButton* button_dropbox_;
  wxBitmapButton* button_onedrive_;
  wxBitmapButton* button_googledrive_;

  wxStaticText* label_seed_;
  wxStaticText* label_seed_value_;

  wxButton* button_ok_;
  wxButton* button_abort_;

  GUI_controller* controller_;
  std::function<void()> update_;

  void set_dropbox(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue(controller_->get_dropbox_path());
  }
  void set_onedrive(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue(controller_->get_onedrive_path());
  }
  void set_googledrive(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue(controller_->get_google_drive_path());
  }
  void close(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue("");
    txtctrl_password_->ChangeValue("");
    textctrl_containername_->ChangeValue("");
    /*this->Show(false);*/
    Destroy();
  }
  void create(wxCommandEvent& event)
  {
    controller_->create_container(std::string(textctrl_containername_->GetValue().mb_str()),
      std::string(txtctrl_password_->GetValue().mb_str()),
      std::string(txtctrl_cloud_loc_->GetValue().mb_str()));
    close(event);
   // close(event);
  }
};