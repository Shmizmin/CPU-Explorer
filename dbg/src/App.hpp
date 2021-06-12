#pragma once

#include <wx/wx.h>

#include <memory>

#include "Main.hpp"

class App : public wxApp
{
public:
	App() noexcept;
	~App() noexcept;

private:
	mainFrame* frame = nullptr;

public:
	virtual bool OnInit() noexcept;
};