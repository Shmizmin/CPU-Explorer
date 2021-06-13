#include "app.hh"
#include "frame.hh"

//generate the WinMain function
wxIMPLEMENT_APP(dbg::MainApp);

bool dbg::MainApp::OnInit(void) noexcept
{
	//construct main frame
	auto* frame = new dbg::MainFrame("Test");
	frame->Show(true);
	return true;
}