#pragma once

#include <wx/wx.h>
#include <cstdlib>

class Main : public wxFrame
{
public:
	Main() noexcept;
	~Main() noexcept;

public:
	int width = 16, height = 16;
	wxButton** buttons;

	int* field = nullptr;
	bool firstClick = true;

public:
	void OnButtonClicked(wxCommandEvent& evt) noexcept;

public:
	wxDECLARE_EVENT_TABLE();
};