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
#include <wx/wxprec.h>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "GUI_controller.h"

class PasswordDialog : public wxDialog
{
  enum ids
  {
    id_txtctrl_password,
    id_button_ok,
    id_button_abort,
    id_label_val,
    id_txtctrl_folder,
    id_button_select
  };

public:
  PasswordDialog(const wxString& title, wxString default_path/*, std::string& pw*/) :
    wxDialog(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))/*, password_(pw)*/
  {
    SetBackgroundColour(*wxWHITE);
   // wxImage::AddHandler(new wxPNGHandler);
    sizer_ = new wxBoxSizer(wxVERTICAL);
    folder_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    entry_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    button_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    label_folder_ = new wxStaticText(this, wxNewId(), wxT("Folder:"));
    label_folder_->SetMinSize(wxSize(100, -1));
    txtctrl_folder_ = new wxTextCtrl(this, id_txtctrl_folder, wxEmptyString);
    txtctrl_folder_->ChangeValue(default_path);
    button_select_ = new wxButton(this, id_button_select, wxT("..."));
    button_select_->SetMinSize(wxSize(-1, 30));
    button_select_->SetMaxSize(wxSize(32, -1));
    Connect(id_button_select, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PasswordDialog::select_folder));
    folder_sizer_->Add(label_folder_, 0, wxALL, 5);
    folder_sizer_->Add(txtctrl_folder_, 0, wxALL, 5);
    folder_sizer_->Add(button_select_, 0, wxALL, 5);
    
    label_pw_ = new wxStaticText(this, wxNewId(), wxT("Password:"));
    label_pw_->SetMinSize(wxSize(100, -1));
    txtctrl_password_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxTextValidator(wxFILTER_ASCII));
    button_ok_ = new wxButton(this, id_button_ok, wxT("OK"));
    Connect(id_button_ok, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PasswordDialog::OnOK));
    button_abort_ = new wxButton(this, id_button_abort, wxT("Abort"));
    Connect(id_button_abort, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PasswordDialog::OnAbort));

    entry_sizer_->Add(label_pw_, 0, wxALL, 5);
    entry_sizer_->Add(txtctrl_password_, 0, wxALL, 5);
    button_sizer_->Add(button_ok_, 0, wxALL, 5);
    button_sizer_->Add(button_abort_, 0, wxALL, 5);
    
    sizer_->Add(folder_sizer_, 1, wxALL, 5);
    sizer_->Add(entry_sizer_, 1, wxALL, 5);
    sizer_->Add(button_sizer_, 1, wxALL, 5);

    SetSizer(sizer_);
    Layout();
    Fit();
    //Centre();
    ShowModal();
    Destroy();
  }

  void OnOK(wxCommandEvent& event)
  {
    password_ = txtctrl_password_->GetValue().ToStdString();
    Destroy();
  }

  void OnAbort(wxCommandEvent& event)
  {
    password_ = "";
    Destroy();
  }

  std::string get_password()
  {
    return password_;
  }

  std::string get_path()
  {
    return path_;
  }

  void reset()
  {
    txtctrl_password_->ChangeValue("");
    password_.clear();
  }

  void select_folder(wxCommandEvent& event)
  {
    wxDirDialog dialog(this);
    dialog.ShowModal();

    txtctrl_folder_->ChangeValue(dialog.GetPath());
    path_ = dialog.GetPath().ToStdString();
  }

  std::string get_folderpath()
  {
    return txtctrl_folder_->GetValue().ToStdString();
  }

private:
  wxBoxSizer* sizer_;
  wxBoxSizer* folder_sizer_;
  wxBoxSizer* entry_sizer_;
  wxBoxSizer* button_sizer_;

  wxStaticText* label_pw_;
  wxTextCtrl* txtctrl_password_;
  wxTextCtrl* label_validation_;

  wxStaticText* label_folder_;
  wxTextCtrl* txtctrl_folder_;
  wxButton* button_select_;

  wxButton* button_ok_;
  wxButton* button_abort_;

  std::string password_;
  std::string path_;
  std::thread pwvalidator_;
};
