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

class LogDialog : public wxDialog
{
  enum ids
  {
    id_list_logs,
    id_button_filter,
    id_button_update,
    id_txtctrl_filter
  };

public:
  LogDialog(const wxString& title, const wxPoint& pos, const wxSize& size, GUI_controller* ctrl) : ctrl_(ctrl),
    wxDialog(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
  {
    SetBackgroundColour(*wxWHITE);
    wxImage::AddHandler(new wxPNGHandler);
    sizer_ = new wxBoxSizer(wxVERTICAL);
    col1_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    list_logs_ = new wxDataViewListCtrl(this, id_list_logs, wxDefaultPosition, wxDefaultSize, 0);
    list_logs_->SetMinSize(wxSize(600, 300));
    column_name_ =  list_logs_->AppendTextColumn(wxT("Scrate"));
    column_message_ = list_logs_->AppendTextColumn(wxT("Message"));
    button_filter_ = new wxButton(this, id_button_update, wxT("Update"));
    Connect(id_button_update, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LogDialog::OnUpdate));

    sizer_->Add(button_filter_, 0, wxALL, 5);
    sizer_->Add(list_logs_, 1, wxALL, 5);

    button_filter_ = new wxButton(this, id_button_filter, wxT("Filter"));
    Connect(id_button_filter, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LogDialog::OnFilter));
    filter_key_ = new wxTextCtrl(this, id_txtctrl_filter, wxT("*"));
    filter_key_->SetMinSize(wxSize(300, -1));
    col1_sizer_->Add(filter_key_, 0, wxALL, 5);
    col1_sizer_->Add(button_filter_, 0, wxALL, 5);
    sizer_->Add(col1_sizer_, 0, wxALL, 5);

    auto logs = ctrl_->get_logs("*");
    for (auto& e : logs)
    {
      wxVector<wxVariant> item;
      item.push_back(wxVariant(e.first));
      item.push_back(e.second);
      list_logs_->AppendItem(item);
    }

    SetSizer(sizer_);
    Layout();
    Fit();
    //Centre();
    ShowModal();
    Destroy();
  }

protected:
  void OnFilter(wxCommandEvent& event)
  {
    list_logs_->DeleteAllItems();
    auto logs = ctrl_->get_logs(std::string(filter_key_->GetValue().mb_str()));
    for (auto& e : logs)
    {
      wxVector<wxVariant> item;
      item.push_back(wxVariant(e.first));
      item.push_back(e.second);
      list_logs_->AppendItem(item);
    }
  }

  void OnUpdate(wxCommandEvent& event)
  {
    OnFilter(event);
  }

private:
  wxBoxSizer* sizer_;
  wxBoxSizer* col1_sizer_;

  wxDataViewListCtrl* list_logs_;
  wxDataViewColumn* column_name_;
  wxDataViewColumn* column_message_;
  wxButton* button_filter_;
  wxButton* button_update_;
  wxTextCtrl* filter_key_;

  GUI_controller* ctrl_;
};