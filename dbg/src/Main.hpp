#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/splitter.h>
#include <wx/string.h>
#include <wx/stc/stc.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>

class mainFrame : public wxFrame
{
private:

protected:
	enum
	{
		FILE_MENU_NEW = 1000,
		FILE_MENU_OPEN,
		FILE_MENU_SAVE,
		FILE_MENU_PRINT,
		FILE_MENU_EXIT,
		EDIT_MENU_UNDO,
		EDIT_MENU_REDO,
		EDIT_MENU_CUT,
		EDIT_MENU_DELETE,
		EDIT_MENU_COPY,
		EDIT_MENU_PASTE,
		EDIT_MENU_DUPLICATE,
		EDIT_MENU_PREFERENCES,
		VIEW_MENU_MINIMIZE,
		VIEW_MENU_MAXIMIZE,
		VIEW_MENU_FULLSCREEN,
		ZOOM_MENU_ZOOMIN,
		ZOOM_MENU_ZOOMOUT,
		VIEW_MENU_RESETZOOM,
		VIEW_MENU_DISPLAYLINES
	};

	wxStatusBar* mainStatusbar;
	wxStyledTextCtrl* sourceEditor;
	wxStyledTextCtrl* binaryEditor;
	wxRichTextCtrl* m_richText2;
	wxMenuBar* menuBarMain;
	wxMenu* fileMenu;
	wxMenu* editMenu;
	wxMenu* viewMenu;
	wxMenu* buildMenu;
	wxMenu* debugMenu;
	wxMenu* windowMenu;

public:

	mainFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CPU Explorer - Debugger"), const wxPoint& pos = wxPoint(50, 50), const wxSize& size = wxSize(1600, 900), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~mainFrame();

};

