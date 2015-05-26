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
#include "GUI_controller.h"

GUI_controller controller;

class ContainerWizard : public wxFrame
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
  ContainerWizard(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, wxID_ANY, title, pos, size)
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

  void set_dropbox(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue(controller.get_dropbox_path());
  }
  void set_onedrive(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue(controller.get_onedrive_path());
  }
  void set_googledrive(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue(controller.get_google_drive_path());
  }
  void close(wxCommandEvent& event)
  {
    txtctrl_cloud_loc_->ChangeValue("");
    txtctrl_password_->ChangeValue("");
    textctrl_containername_->ChangeValue("");
    this->Show(false);
  }
  void create(wxCommandEvent& event)
  {
    controller.create_container(std::string(textctrl_containername_->GetValue().mb_str()),
      std::string(txtctrl_password_->GetValue().mb_str()),
      std::string(txtctrl_cloud_loc_->GetValue().mb_str()));
    close(event);
  }
};

class MyFrame : public wxFrame
{
  enum ids
  {
    id_menu_quit,
    id_menu_about,
    id_menu_options,
    id_new_container,
    id_open_container,
    id_close_container,
    id_container_grid,
    id_sync_all
  };

public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, wxID_ANY, title, pos, size), wizard_(new ContainerWizard(wxT("Wizard"), wxDefaultPosition, wxDefaultSize))
	{
    SetBackgroundColour(*wxWHITE);
    bar_ = new wxMenuBar;
    fileMenu_ = new wxMenu(_T(""));
    fileMenu_->Append(id_new_container, _("&New"), _("Create a new Scrate"));
    Connect(id_new_container, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnNew));
    fileMenu_->AppendSeparator();
    fileMenu_->Append(id_menu_quit, _("&Quit\tAlt-F4"), _("Quit Scrates"));
    Connect(id_menu_quit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnExit));
    bar_->Append(fileMenu_, _("&File"));
    extraMenu_ = new wxMenu(_T(""));
    extraMenu_->Append(id_menu_options, _("&Settings\tAlt-o"), _("Scrates Settings"));
    Connect(id_menu_options, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSettings));
    bar_->Append(extraMenu_, _("&Extras"));
    helpMenu_ = new wxMenu(_T(""));
    helpMenu_->Append(id_menu_about, _("&About\tF1"), _("About Scrates"));
    bar_->Append(helpMenu_, _("&Help"));
    SetMenuBar(bar_);

   // new_container_ = new wxButton(this, id_new_container, _("New"));
    open_container_ = new wxButton(this, id_open_container, _("Open"));
    sync_all_ = new wxButton(this, id_sync_all, _("Sync"));
    close_container_ = new wxButton(this, id_close_container, _("Close"));

    container_view_ = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    container_view_->SetMinSize(wxSize(600, 300));
    container_view_name_ = container_view_->AppendTextColumn(wxT("Name"));
    container_view_status_ = container_view_->AppendTextColumn(wxT("Status"));
    container_view_log_ = container_view_->AppendIconTextColumn(wxT("Log"));

    sizer_ = new wxBoxSizer(wxVERTICAL);
    button_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    sizer_->Add(button_sizer_, 0, wxALL, 5);
    sizer_->Add(container_view_, 1, wxALL | wxEXPAND, 5);
   // button_sizer_->Add(new_container_, 0, wxALL, 5);
    button_sizer_->Add(open_container_, 0, wxALL, 5);
    button_sizer_->Add(sync_all_, 0, wxALL, 5);
    button_sizer_->Add(close_container_, 0, wxALL, 5);

   // wizard = new ContainerWizard(wxT("Wizard"), wxDefaultPosition, wxDefaultSize);

    SetSizer(sizer_);
    Layout();
    SetMinSize(wxSize(640, 480));
    Fit();
	}
  
  ~MyFrame()
  {
    wizard_->Close(true);
  }

protected:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event)
  {
    wizard_->Close(true);
    Close(true);
  }
  void OnAbout(wxCommandEvent& event);

  void OnNew(wxCommandEvent& event)
  {
    wizard_->Show(true);
  }
  void OnSettings(wxCommandEvent& event)
  {

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

  //ContainerWizard* wizard;
  std::unique_ptr<ContainerWizard> wizard_;
};

class MyApp : public wxApp
{
public:
	virtual bool OnInit()
	{
		MyFrame* frame = new MyFrame("Scrates", wxDefaultPosition, wxSize(640, 480));
		frame->Show(true);
		return true;
	}
};

wxIMPLEMENT_APP(MyApp);