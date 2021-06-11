#include "Main.hpp"

wxBEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_BUTTON(10001, OnButtonClicked)
wxEND_EVENT_TABLE()

Main::Main() noexcept
	: wxFrame(nullptr, wxID_ANY, "CPU Explorer - Debugger", wxPoint(50, 50), wxSize(1600, 900))
{
	buttons = new wxButton*[width * height];
	auto* grid = new wxGridSizer(width, height, 0, 0);

	field = new int[width * height];

	wxFont font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);

	for (auto x = 0; x < width; ++x)
	{
		for (auto y = 0; y < height; ++y)
		{
			buttons[y * width + x] = new wxButton(this, 10000 + (y * width + x));
			buttons[y * width + x]->SetFont(font);
			grid->Add(buttons[y * width + x], 1, wxEXPAND | wxALL);

			buttons[y * width + x]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Main::OnButtonClicked, this);

			field[y * width + x] = 0;
		}
	}

	this->SetSizer(grid);
	grid->Layout();
}

Main::~Main() noexcept
{
	delete[] buttons;
	delete[] field;
}

void Main::OnButtonClicked(wxCommandEvent& evt) noexcept
{
	auto x = ((evt.GetId() - 10000) & (width - 1)), //mod width
		 y = ((evt.GetId() - 10000) >> 4);          //div width

	if (firstClick) [[unlikely]]
	{
		firstClick = false;

		int mines = 60;

		while (mines)
		{
			int rx = std::rand() & (width - 1);  //mod width
			int ry = std::rand() & (height - 1); //mod height

			if (field[ry * width + rx] == 0 and rx not_eq x and ry not_eq y)
			{
				field[ry * width + rx] = -1;
				--mines;
			}
		}
	}

	buttons[y * width + x]->Enable(false); //prevent button from being pressed again

	if (field[y * width + x] == -1) [[unlikely]]
	{
		wxMessageBox("Game Over");

		//reset the game state
		firstClick = true;

		for (auto x = 0; x < width; ++x)
		{
			for (auto y = 0; y < height; ++y)
			{
				field[y * width + x] = 0;
				buttons[y * width + x]->SetLabel("");
				buttons[y * width + x]->Enable(true);
			}
		}
	}

	else
	{
		int mines = 0;

		for (auto i = -1; i < 2; ++i)
		{
			for (auto j = -1; j < 2; ++j)
			{
				if (x + i >= 0 and x + i < width and y + j >= 0 and y + j < height) [[unlikely]]
				{
					if (field[(y + j) * width + (x + y)] == -1)
						++mines;
				}
			}
		}

		if (mines > 0) [[likely]]
			buttons[y * width + x]->SetLabel(std::to_string(mines));
	}


	evt.Skip();
}