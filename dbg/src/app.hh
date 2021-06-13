#ifndef _DBG_APP_HH
#define _DBG_APP_HH

#include <wx/wx.h>

namespace dbg
{
	class MainApp : public wxApp
	{
	public:
		virtual bool OnInit(void) noexcept;
	}; //MainApp class

} //dbg namespace

#endif //_DBG_APP_HH