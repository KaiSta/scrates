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
#include "ContainerWizard.h"
#include "MainFrame.h"

class MyApp : public wxApp
{
public:
  std::shared_ptr<GUI_controller> controller;
  //GUI_controller* controller;
  MainFrame* frame;
	virtual bool OnInit()
	{
    controller = std::make_shared < GUI_controller >();// .reset(new GUI_controller());
   // controller = new GUI_controller();
    controller->init();
    frame = new MainFrame("Scrates", wxDefaultPosition, wxSize(640, 480), controller.get());
		frame->Show(true);
		return true;
	}
};

wxIMPLEMENT_APP(MyApp);