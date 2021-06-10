#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>

static HWND staticControl, editControl, buttonControl;
static HANDLE image;

auto outstandingWork = true;

enum
{
	FILE_MENU_PARENT,
		FILE_MENU_NEW,
		FILE_MENU_OPEN,
		//separator
		FILE_MENU_SAVE,
		FILE_MENU_SAVE_AS,
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
		VIEW_MENU_REFRESH,
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
		DEBUG_MENU_ENABLEALLBREAKPOINTS,
		DEBUG_MENU_DISABLEALLBREAKPOINTS,
		//
		DEBUG_MENU_SINGLE_STEP,
		DEBUG_MENU_CONTINUE,

	HELP_MENU_PARENT,
		HELP_MENU_FEEDBACK,
		//separator
		HELP_MENU_CHECKUPDATES,
		//separator
		HELP_MENU_ABOUT,
};

void AddMenus(HWND wnd) noexcept
{
	auto titlebar = CreateMenu(),
		 filemenu = CreateMenu(),
		 editmenu = CreateMenu(),
		 viewmenu = CreateMenu(),
		buildmenu = CreateMenu(),
		debugmenu = CreateMenu(),
		 helpmenu = CreateMenu();

	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(filemenu), L"File");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(editmenu), L"Edit");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(viewmenu), L"View");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(buildmenu), L"Build");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(debugmenu), L"Debug");
	AppendMenuW(titlebar, MF_POPUP, reinterpret_cast<UINT_PTR>(helpmenu), L"Help");


	//AppendMenuW(submenu, MF_STRING, CHANGE_TITLE, L"Change Window Title");
	//
	//AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(filemenu), L"File");
	//AppendMenuW(menu, MF_STRING, NULL, L"Edit");
	//AppendMenuW(menu, MF_STRING, HELP_MENU, L"Help");
	//
	//AppendMenuW(filemenu, MF_STRING, LOAD_MENU, L"Load");
	//AppendMenuW(filemenu, MF_POPUP, reinterpret_cast<UINT_PTR>(submenu), L"Options");
	//AppendMenuW(filemenu, MF_STRING, ABOUT_MENU, L"About");
	//AppendMenuW(filemenu, MF_SEPARATOR, NULL, NULL);
	//AppendMenuW(filemenu, MF_STRING, EXIT_MENU, L"Exit");

	SetMenu(wnd, titlebar);
}

void AddControls(HWND wnd) noexcept
{
	staticControl = CreateWindowExW(NULL, L"static", L"Do Something:",
		(WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER),
		700, 400, 200, 50, wnd, NULL, NULL, NULL);

	editControl = CreateWindowExW(NULL, L"edit", L"Enter Text Here",
		(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL),
		700, 475, 200, 200, wnd, NULL, NULL, NULL);

	buttonControl = CreateWindowExW(NULL, L"button", L"",
		(WS_CHILD | WS_VISIBLE | BS_BITMAP),
		600, 700, 400, 100, wnd, reinterpret_cast<HMENU>(CHANGE_TITLE), NULL, NULL);

	SendMessageW(buttonControl, BM_SETIMAGE,
		static_cast<WPARAM>(IMAGE_BITMAP), reinterpret_cast<LPARAM>(image));
}

void LoadImages(void) noexcept
{
	image = ::LoadImageW(NULL, L"../app/res/image.bmp", IMAGE_BITMAP, 400, 100, LR_LOADFROMFILE);
}

void SaveOutstandingWork(void) noexcept
{

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
	case WM_SIZE:
	{
		break;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	case WM_CREATE:
	{
		LoadImages();
		AddMenus(wnd);
		AddControls(wnd);

		//auto font = CreateFontW(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
		//SendMessageW(wnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

		//EnumChildWindows(wnd, reinterpret_cast<WNDENUMPROC>(SetFont),
		//	reinterpret_cast<LPARAM>(font));

		break;
	}

	case WM_COMMAND:
	{
		switch (wp)
		{
		case CHANGE_TITLE:
		{
			auto size = GetWindowTextLengthW(editControl) + static_cast<int>(sizeof(wchar_t));
			auto* buffer = new wchar_t[size];
			GetWindowTextW(editControl, buffer, size);
			SetWindowTextW(wnd, buffer);
			delete[] buffer;
			break;
		}

		case EXIT_MENU:
			if (outstandingWork) [[unlikely]]
			{
				switch (MessageBoxW(wnd, L"Save changes to modified items?",
								 L"Unsaved Changes", MB_YESNOCANCEL | MB_ICONERROR))
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

		case LOAD_MENU:
			break;

		case ABOUT_MENU:
			MessageBoxW(wnd, L"Shmizmin © 2021", L"About", NULL);
			break;

		case HELP_MENU:
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

INT WINAPI main(HINSTANCE instance,
   [[maybe_unused]] HINSTANCE previnstance,
   [[maybe_unused]] LPWSTR    args,
   [[maybe_unused]] INT       cmdshow)
{
	auto col = CreateSolidBrush(RGB(45, 45, 45));

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
	

	if (!RegisterClassExW(&wcex))
		MessageBoxW(NULL, L"Failed to register window class", L"Error", NULL);

	auto wnd = CreateWindowExW(NULL, L"MyWindowClass", L"CPU Explorer - Debugger",
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE), 150, 1250, 1600, 900, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	DeleteObject(col);
	return EXIT_SUCCESS;
}