#define NOMINMAX               //avoid macro/function name collisions
#define WIN32_LEAN_AND_MEAN    //exclude the rarely used API features
#include <Windows.h>
#include <cstdlib>             //for opening the browser in the help menu

//is set when there is something that needs to 
auto outstandingWork = true;

//represents the main window position and size
//RECT prev = { sizeof(prev) };

enum
{
	FILE_MENU_PARENT,
		FILE_MENU_NEW,
		FILE_MENU_OPEN,
		//separator
		FILE_MENU_SAVE,
		FILE_MENU_SAVEAS,
		//separator
		FILE_MENU_PRINT,
		FILE_MENU_EXIT,

	EDIT_MENU_PARENT,
		EDIT_MENU_UNDO,
		EDIT_MENU_REDO,
		//separator
		EDIT_MENU_CUT,
		EDIT_MENU_DELETE,
		EDIT_MENU_COPY,
		EDIT_MENU_PASTE,
		EDIT_MENU_DUPLICATE,
		//separator
		EDIT_MENU_PREFERENCES,

	VIEW_MENU_PARENT,
		VIEW_MENU_MINIMIZE,
		VIEW_MENU_MAXIMIZE,
		VIEW_MENU_FULLSCREEN,
		//separator
		VIEW_MENU_ZOOMIN,
		VIEW_MENU_ZOOMOUT,
		VIEW_MENU_ZOOMRESET,
		//separator
		VIEW_MENU_SHOWLINES,
		VIEW_MENU_SHOWOUTPUT,

	BUILD_MENU_PARENT,
		BUILD_MENU_BUILDCURRENT,
		BUILD_MENU_BUILDALL,
		//separator
		BUILD_MENU_CLEANCURRENT,
		BUILD_MENU_CLEANALL,
		//separator
		BUILD_MENU_MANAGE,

	DEBUG_MENU_PARENT,
		DEBUG_MENU_RUNWITH,
		DEBUG_MENU_RUNWITHOUT,
		//separator
		DEBUG_MENU_SETBREAKPOINT,
		DEBUG_MENU_CLEARBREAKPOINT,
		DEBUG_MENU_ENABLEALLBREAKPOINTS,
		DEBUG_MENU_DISABLEALLBREAKPOINTS,
		//separator
		DEBUG_MENU_SINGLE_STEP,
		DEBUG_MENU_CONTINUE,

	WINDOW_MENU_PARENT,
		WINDOW_MENU_CLOSECURRENT,
		WINDOW_MENU_CLOSEALL,
		//separator
		WINDOW_MENU_REOPENLAST,
		WINDOW_MENU_REOPENALL,

	HELP_MENU_PARENT,
		HELP_MENU_FEEDBACK,
		HELP_MENU_GETHELP,
		//separator
		HELP_MENU_CHECKUPDATES,
		//separator
		HELP_MENU_ABOUT,
};

void AddMenus(HWND wnd) noexcept
{
	//obtain a handle to the titlebar and each of it's items
	auto titlebar = CreateMenu(),
		 filemenu = CreateMenu(),
		 editmenu = CreateMenu(),
		 viewmenu = CreateMenu(),
		buildmenu = CreateMenu(),
		debugmenu = CreateMenu(),
	   windowmenu = CreateMenu(),
		 helpmenu = CreateMenu();

	//place each item on the titlebar
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(filemenu),   L"File");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(editmenu),   L"Edit");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(viewmenu),   L"View");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(buildmenu),  L"Build");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(debugmenu),  L"Debug");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(windowmenu), L"Window");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(helpmenu),   L"Help");

	//place each sub-item in the file titlebar item
	AppendMenuW(filemenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_NEW),    L"New\t\tCtrl + N");
	AppendMenuW(filemenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_OPEN),   L"Open\tCtrl + O");
	AppendMenuW(filemenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(filemenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVE),   L"Save\tCtrl + S");
	AppendMenuW(filemenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVEAS), L"Save As\tCtrl + Shift + S");
	AppendMenuW(filemenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(filemenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_PRINT),  L"Print\tCtrl + P");
	AppendMenuW(filemenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_EXIT),   L"Exit\tCtrl + Esc");

	//place each sub-item in the edit titlebar item
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(EDIT_MENU_UNDO),   L"Undo\tCtrl + Z");
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(EDIT_MENU_REDO),   L"Redo\tCtrl + U");
	AppendMenuW(editmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVE),   L"Cut\tCtrl + X");
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVEAS), L"Delete\tDel");
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVEAS), L"Copy\tCtrl + C");
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVEAS), L"Paste\tCtrl + V");
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_SAVEAS), L"Duplicate\tCtrl + D");
	AppendMenuW(editmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(editmenu, MF_STRING, static_cast<UINT_PTR>(FILE_MENU_PRINT),  L"Preferences");

	//place each sub-item in the view titlebar item
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_MINIMIZE), L"Minimize\tWindows + Down");
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_MAXIMIZE), L"Maximize\tWindows + Up");
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_FULLSCREEN), L"Fullscreen\tAlt + Enter");
	AppendMenuW(viewmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_ZOOMIN), L"Zoom In\tCtrl + ]");
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_ZOOMOUT), L"Zoom Out\tCtrl + [");
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_ZOOMRESET), L"Reset Zoom\tCtrl + \\");
	AppendMenuW(viewmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_SHOWLINES), L"Display Line Numbers");
	AppendMenuW(viewmenu, MF_STRING, static_cast<UINT_PTR>(VIEW_MENU_SHOWOUTPUT), L"Display Code Output");

	//place each sub-item in the build titlebar item
	AppendMenuW(buildmenu, MF_STRING, static_cast<UINT_PTR>(BUILD_MENU_BUILDCURRENT), L"Build Current File\tCtrl + B");
	AppendMenuW(buildmenu, MF_STRING, static_cast<UINT_PTR>(BUILD_MENU_BUILDALL), L"Build All Files\tF7");
	AppendMenuW(buildmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(buildmenu, MF_STRING, static_cast<UINT_PTR>(BUILD_MENU_CLEANCURRENT), L"Clean Current File\tCtrl + Shift + B");
	AppendMenuW(buildmenu, MF_STRING, static_cast<UINT_PTR>(BUILD_MENU_CLEANALL), L"Clean All Files\tCtrl + F7");
	AppendMenuW(buildmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(buildmenu, MF_STRING, static_cast<UINT_PTR>(BUILD_MENU_CLEANALL), L"Manage\tF6");

	//place each sub-item in the debug titlebar item
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_RUNWITH), L"Start Current With Debugging\tF5");
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_RUNWITH), L"Start Current Without Debugging\tCtrl + F5");
	AppendMenuW(debugmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_SETBREAKPOINT), L"Set Breakpoint\tF9");
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_CLEARBREAKPOINT), L"Clear Breakpoint\tCtrl + F9");
	AppendMenuW(debugmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_ENABLEALLBREAKPOINTS), L"Enable All Breakpoints\tF8");
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_DISABLEALLBREAKPOINTS), L"Disable All Breakpoints\tCtrl + F8");
	AppendMenuW(debugmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_ENABLEALLBREAKPOINTS), L"Single Step\tF4");
	AppendMenuW(debugmenu, MF_STRING, static_cast<UINT_PTR>(DEBUG_MENU_DISABLEALLBREAKPOINTS), L"Continue\tF3");

	//place each sub-item in the window titlebar item
	AppendMenuW(windowmenu, MF_STRING, static_cast<UINT_PTR>(WINDOW_MENU_CLOSECURRENT), L"Close Current Window\tCtrl + W");
	AppendMenuW(windowmenu, MF_STRING, static_cast<UINT_PTR>(WINDOW_MENU_CLOSEALL), L"Close All Windows\tCtrl + Shift + W");
	AppendMenuW(windowmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(windowmenu, MF_STRING, static_cast<UINT_PTR>(WINDOW_MENU_REOPENLAST), L"Reopn Last Window\tCtrl + T");
	AppendMenuW(windowmenu, MF_STRING, static_cast<UINT_PTR>(WINDOW_MENU_REOPENALL), L"Reopn All Windows\tCtrl + Shift + T");

	//place each sub-item in the help titlebar item
	AppendMenuW(helpmenu, MF_STRING, static_cast<UINT_PTR>(HELP_MENU_FEEDBACK), L"Feedback");
	AppendMenuW(helpmenu, MF_STRING, static_cast<UINT_PTR>(HELP_MENU_GETHELP), L"Get Help");
	AppendMenuW(helpmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(helpmenu, MF_STRING, static_cast<UINT_PTR>(HELP_MENU_CHECKUPDATES), L"Check for Updates");
	AppendMenuW(helpmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenuW(helpmenu, MF_STRING, static_cast<UINT_PTR>(HELP_MENU_ABOUT), L"About cpu-explorer-dbg");

	//assign the titlebar to the main window
	SetMenu(wnd, titlebar);
}

void AddControls(HWND wnd) noexcept
{
	//generate any sub-main window controls
}

void LoadImages(void) noexcept
{
	//generate the resources for any image
}

void SaveOutstandingWork(void) noexcept
{
	//write pending file changes out to disk
}

bool CALLBACK SetFont(HWND child, LPARAM font)
{
	//change the active font for a window
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
		//quit out immediately
		PostQuitMessage(0);
		break;
	}

	case WM_CREATE:
	{
		LoadImages();
		AddMenus(wnd);
		AddControls(wnd);

		//create a custom larger Consolas Mono font
		//auto font = CreateFontW(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
		//SendMessageW(wnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

		//activate this font for all child sub-windows
		//EnumChildWindows(wnd, reinterpret_cast<WNDENUMPROC>(SetFont),
		//	reinterpret_cast<LPARAM>(font));

		break;
	}

	case WM_COMMAND:
	{
		switch (wp)
		{
		case VIEW_MENU_FULLSCREEN:
		{
			DWORD dwStyle = GetWindowLongW(wnd, GWL_STYLE);
			if (dwStyle & WS_OVERLAPPEDWINDOW)
			{
				MONITORINFO mi = { sizeof(mi) };

				if (GetMonitorInfoW(MonitorFromWindow(wnd, MONITOR_DEFAULTTOPRIMARY), &mi))
				{
					SetWindowLongW(wnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

					SetWindowPos(wnd, HWND_TOP,
						mi.rcMonitor.left, mi.rcMonitor.top,
						mi.rcMonitor.right - mi.rcMonitor.left,
						mi.rcMonitor.bottom - mi.rcMonitor.top,
						SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				}
			}

			else
			{
				//ShowWindow(wnd, SW_SHOW);

				//SetWindowPos(wnd, wnd, prev.left, prev.top,
				//	(prev.right - prev.left),
				//	(prev.bottom - prev.top),
				//	//SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				//	SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				//
				SetWindowLongW(wnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
			}
			break;
		}

		case FILE_MENU_EXIT:
			if (outstandingWork) [[unlikely]]
			{
				switch (MessageBoxW(wnd, L"Save changes to modified items?",
								 L"Unsaved Changes", MB_YESNOCANCEL | MB_ICONWARNING))
				{
				case IDYES:
					SaveOutstandingWork();
					[[fallthrough]];

				case IDNO:
					goto QUIT;

				case IDCANCEL:
					break;
				}
			}

			else [[likely]]
			{
			QUIT:
				DestroyWindow(wnd);
			}
			break;

		case HELP_MENU_ABOUT:
			MessageBoxW(wnd, L"Shmizmin © 2021", L"About", NULL);
			break;

		case HELP_MENU_GETHELP:
			//invoke the system default browser to open up this repo
			std::system("start https://www.github.com/shmizmin/cpu-explorer");
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
	//is used for coloring the background
	auto col = CreateSolidBrush(RGB(45, 45, 45));

	//define our main window class
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = col;
	wcex.hCursor = LoadCursorW(instance, IDC_ARROW);
	wcex.hInstance = instance;
	wcex.lpszClassName = L"MyWindowClass";
	wcex.lpfnWndProc = &WindowProcedure;
	wcex.lpszMenuName = MAKEINTRESOURCEW(1);
	wcex.hIcon = static_cast<HICON>(::LoadImageW(NULL,
		L"../app/res/icon.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE));
	
	//attempt to register our window class
	if (!RegisterClassExW(&wcex))
		MessageBoxW(NULL, L"Failed to register window class", L"Error", NULL);

	//attempt to create a window from our window class
	auto wnd = CreateWindowExW(NULL, L"MyWindowClass", L"CPU Explorer - Debugger",
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE), 150, 1250, 1600, 900, NULL, NULL, NULL, NULL);

	//start the message loop
	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	//avoid memory leaks from GDI objects
	DeleteObject(col);

	//indicate successful program completion
	return 0;
}