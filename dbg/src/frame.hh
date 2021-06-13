#ifndef _DBG_FRAME_HH
#define _DBG_FRAME_HH

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/artprov.h>

namespace dbg
{
	class MainFrame : public wxFrame
	{
	public:
		MainFrame(const wxString& title) noexcept;
		virtual ~MainFrame(void) noexcept;

	private:
		//main aui manager
		wxAuiNotebook* auiNotebook = nullptr;

		//main ribbon manager
		wxRibbonBar* ribbonBar = nullptr;

		//ribbon bar 'tabs'
		wxRibbonPage *homeRibbonPage = nullptr,
					 *editRibbonPage = nullptr;

		//ribbon bar 'tab' contents
		wxRibbonPanel *homeRibbonPanel = nullptr,
					  *itemRibbonPanel = nullptr;

		//ribbon bar 'tab' buttons
		wxRibbonButtonBar *homeRibbonButtonBar = nullptr,
						  *itemRibbonButtonBar = nullptr;

	}; //MainFrame class

} //dbg namespace

#endif //_DBG_FRAME_HH