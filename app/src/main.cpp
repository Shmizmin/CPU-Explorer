#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Uxtheme.h>
#include <comdef.h>

#pragma comment(lib, "uxtheme.lib")

static HWND staticControl, editControl;

enum
{
	FILE_MENU,
	ABOUT_MENU,
	EXIT_MENU,
	LOAD_MENU,
	HELP_MENU,
};

COLORREF GetBrushColor(HBRUSH brush)
{
	LOGBRUSH lbr;

	if (GetObjectW(brush, sizeof(lbr), &lbr) != sizeof(lbr))
		return CLR_NONE;

	if (lbr.lbStyle != BS_SOLID)
		return CLR_NONE;

	return lbr.lbColor;
}

void AddMenus(HWND wnd) noexcept
{
	SetWindowTheme(wnd, L"Explorer", NULL);
	SendMessageW(wnd, WM_THEMECHANGED, NULL, NULL);

	HMENU menu = CreateMenu(),
	  filemenu = CreateMenu(),
	   submenu = CreateMenu();

	AppendMenuW(submenu, MF_STRING, NULL, L"Change Window Title");

	AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(filemenu), L"File");
	AppendMenuW(menu, MF_STRING, NULL, L"Edit");
	AppendMenuW(menu, MF_STRING, HELP_MENU, L"Help");

	AppendMenuW(filemenu, MF_STRING, LOAD_MENU, L"Load");
	AppendMenuW(filemenu, MF_POPUP, reinterpret_cast<UINT_PTR>(submenu), L"Options");
	AppendMenuW(filemenu, MF_STRING, ABOUT_MENU, L"About");
	AppendMenuW(filemenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(filemenu, MF_STRING, EXIT_MENU, L"Exit");


	SetMenu(wnd, menu);
}

void AddControls(HWND wnd) noexcept
{
	staticControl = CreateWindowExW(NULL, L"static", L"Do Something:",
		(WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER),
		700, 400, 200, 50, wnd, NULL, NULL, NULL);

	editControl = CreateWindowExW(NULL, L"edit", L"Enter Text Here",
		(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL),
		700, 475, 200, 200, wnd, NULL, NULL, NULL);
}

bool CALLBACK SetFont(HWND child, LPARAM font)
{
	SendMessageW(child, WM_SETFONT, font, true);
	return true;
}

LRESULT CALLBACK WindowProcedure([[maybe_unused]] HWND wnd,
								 [[maybe_unused]] UINT msg,
								 [[maybe_unused]] WPARAM wp,
								 [[maybe_unused]] LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	case WM_CREATE:
	{
		AddMenus(wnd);
		AddControls(wnd);

		auto font = CreateFontW(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
		SendMessageW(wnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

		EnumChildWindows(wnd, reinterpret_cast<WNDENUMPROC>(SetFont),
			reinterpret_cast<LPARAM>(font));

		break;
	}

	case WM_COMMAND:
	{
		switch (wp)
		{
		case EXIT_MENU:
			DestroyWindow(wnd);
			break;

		case LOAD_MENU:
			break;

		case ABOUT_MENU:
			MessageBoxW(wnd, L"Shmizmin © 2021", L"About", NULL);
			break;

		case HELP_MENU:
			MessageBeep(MB_OK);
			break;
		}

		break;
	}

	default:
	{
		return DefWindowProcW(wnd, msg, wp, lp);
		break;
	}
	}
}



INT WINAPI wWinMain(HINSTANCE instance,
   [[maybe_unused]] HINSTANCE previnstance,
   [[maybe_unused]] LPWSTR    args,
   [[maybe_unused]] INT       cmdshow)
{
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GRAY_BRUSH);
	wcex.hCursor = LoadCursorW(instance, IDC_ARROW);
	wcex.hInstance = instance;
	wcex.lpszClassName = L"WindowClass";
	wcex.lpfnWndProc = &WindowProcedure;
	wcex.lpszMenuName = MAKEINTRESOURCEW(1);
	wcex.hIcon = static_cast<HICON>(::LoadImageW(NULL,
		L"../app/res/icon.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE));

	wcex.hIconSm = static_cast<HICON>(::LoadImageW(NULL,
		L"../app/res/icon.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE));

	if (!RegisterClassExW(&wcex))
		MessageBoxW(NULL, L"Failed to register window class", L"Error", NULL);

	auto wnd = CreateWindowExW(NULL, L"WindowClass", L"CPU Explorer - Debugger",
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE), 150, 1250, 1600, 900, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}