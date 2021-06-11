#include "App.hpp"

wxIMPLEMENT_APP(App);

App::App() noexcept
{
}

App::~App() noexcept
{
}

bool App::OnInit() noexcept
{
	frame = new Main;
	frame->Show();

	return true;
}