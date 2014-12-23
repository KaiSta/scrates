#include <wx/wx.h>
#include <wx/window.h>

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
		wxFrame(nullptr, wxID_ANY, title, pos, size)
	{

	}

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
};

class MyApp : public wxApp
{
public:
	virtual bool OnInit()
	{
		MyFrame* frame = new MyFrame("HelloWorld", wxDefaultPosition, wxSize(640, 480));
		frame->Show(true);
		return true;
	}
};

wxIMPLEMENT_APP(MyApp);