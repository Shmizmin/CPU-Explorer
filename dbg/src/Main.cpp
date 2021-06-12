#include "Main.hpp"

mainFrame::mainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));

	mainStatusbar = this->CreateStatusBar(1, wxSTB_SIZEGRIP, wxID_ANY);
	mainStatusbar->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* parentSizer;
	parentSizer = new wxBoxSizer(wxHORIZONTAL);

	wxSplitterWindow* splitterSizer;
	splitterSizer = new wxSplitterWindow(nullptr);

	//splitterSizer->AddChild()

	sourceEditor = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxBORDER_THEME | wxCLIP_CHILDREN | wxHSCROLL | wxNO_FULL_REPAINT_ON_RESIZE | wxTAB_TRAVERSAL | wxVSCROLL, wxEmptyString);
	sourceEditor->SetUseTabs(true);
	sourceEditor->SetTabWidth(4);
	sourceEditor->SetIndent(4);
	sourceEditor->SetTabIndents(true);
	sourceEditor->SetBackSpaceUnIndents(true);
	sourceEditor->SetViewEOL(false);
	sourceEditor->SetViewWhiteSpace(false);
	sourceEditor->SetMarginWidth(2, 0);
	sourceEditor->SetIndentationGuides(true);
	sourceEditor->SetReadOnly(false);
	sourceEditor->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
	sourceEditor->SetMarginMask(1, wxSTC_MASK_FOLDERS);
	sourceEditor->SetMarginWidth(1, 16);
	sourceEditor->SetMarginSensitive(1, true);
	sourceEditor->SetProperty(wxT("fold"), wxT("1"));
	sourceEditor->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
	sourceEditor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
	sourceEditor->SetMarginWidth(0, sourceEditor->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
	sourceEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("BLACK")));
	sourceEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("WHITE")));
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
	sourceEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("BLACK")));
	sourceEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("WHITE")));
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS);
	sourceEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("BLACK")));
	sourceEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("WHITE")));
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS);
	sourceEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("BLACK")));
	sourceEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("WHITE")));
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
	sourceEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
	sourceEditor->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	sourceEditor->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	parentSizer->Add(sourceEditor, 1, wxEXPAND | wxALL, 0);

	wxBoxSizer* rightSizer;
	rightSizer = new wxBoxSizer(wxVERTICAL);

	binaryEditor = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxBORDER_THEME | wxCLIP_CHILDREN | wxHSCROLL | wxNO_FULL_REPAINT_ON_RESIZE | wxTAB_TRAVERSAL | wxVSCROLL, wxEmptyString);
	binaryEditor->SetUseTabs(true);
	binaryEditor->SetTabWidth(4);
	binaryEditor->SetIndent(4);
	binaryEditor->SetTabIndents(true);
	binaryEditor->SetBackSpaceUnIndents(true);
	binaryEditor->SetViewEOL(false);
	binaryEditor->SetViewWhiteSpace(false);
	binaryEditor->SetMarginWidth(2, 0);
	binaryEditor->SetIndentationGuides(true);
	binaryEditor->SetReadOnly(true);
	binaryEditor->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
	binaryEditor->SetMarginMask(1, wxSTC_MASK_FOLDERS);
	binaryEditor->SetMarginWidth(1, 16);
	binaryEditor->SetMarginSensitive(1, true);
	binaryEditor->SetProperty(wxT("fold"), wxT("1"));
	binaryEditor->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
	binaryEditor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
	binaryEditor->SetMarginWidth(0, binaryEditor->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
	binaryEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("BLACK")));
	binaryEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("WHITE")));
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
	binaryEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("BLACK")));
	binaryEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("WHITE")));
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS);
	binaryEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("BLACK")));
	binaryEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("WHITE")));
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS);
	binaryEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("BLACK")));
	binaryEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("WHITE")));
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
	binaryEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
	binaryEditor->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	binaryEditor->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	rightSizer->Add(binaryEditor, 1, wxEXPAND | wxALL, 0);

	m_richText2 = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 | wxALWAYS_SHOW_SB | wxBORDER_THEME | wxCLIP_CHILDREN | wxHSCROLL | wxNO_FULL_REPAINT_ON_RESIZE | wxVSCROLL);
	rightSizer->Add(m_richText2, 1, wxEXPAND | wxALL, 0);


	parentSizer->Add(rightSizer, 1, wxEXPAND, 0);


	this->SetSizer(parentSizer);
	this->Layout();
	menuBarMain = new wxMenuBar(0);
	menuBarMain->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	menuBarMain->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));

	fileMenu = new wxMenu();
	wxMenuItem* fileMenuNew;
	fileMenuNew = new wxMenuItem(fileMenu, wxID_ANY, wxString(wxT("New\tCtrl + N")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(fileMenuNew);

	wxMenuItem* fileMenuOpen;
	fileMenuOpen = new wxMenuItem(fileMenu, wxID_ANY, wxString(wxT("Open\tCtrl + O")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(fileMenuOpen);

	fileMenu->AppendSeparator();

	wxMenuItem* fileMenuSave;
	fileMenuSave = new wxMenuItem(fileMenu, wxID_ANY, wxString(wxT("Save\tCtrl + S")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(fileMenuSave);

	wxMenuItem* fileMenuSaveAs;
	fileMenuSaveAs = new wxMenuItem(fileMenu, wxID_ANY, wxString(wxT("Save As\tCtrl + Shift + S")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(fileMenuSaveAs);

	fileMenu->AppendSeparator();

	wxMenuItem* fileMenuPrint;
	fileMenuPrint = new wxMenuItem(fileMenu, wxID_ANY, wxString(wxT("Print\tCtrl + P")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(fileMenuPrint);

	wxMenuItem* fileMenuExit;
	fileMenuExit = new wxMenuItem(fileMenu, wxID_ANY, wxString(wxT("Exit\tCtrl + Esc")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(fileMenuExit);

	menuBarMain->Append(fileMenu, wxT("File"));

	editMenu = new wxMenu();
	wxMenuItem* editMenuUndo;
	editMenuUndo = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Undo\tCtrl + Z")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuUndo);

	wxMenuItem* editMenuRedo;
	editMenuRedo = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Redo\tCtrl + Y")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuRedo);

	editMenu->AppendSeparator();

	wxMenuItem* editMenuCut;
	editMenuCut = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Cut\tCtrl + X")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuCut);

	wxMenuItem* editMenuDelete;
	editMenuDelete = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Delete\tDel")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuDelete);

	wxMenuItem* editMenuCopy;
	editMenuCopy = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Copy\tCtrl + C")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuCopy);

	wxMenuItem* editMenuPaste;
	editMenuPaste = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Paste\tCtrl + V")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuPaste);

	wxMenuItem* editMenuDuplicate;
	editMenuDuplicate = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Duplicate\tCtrl + D")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuDuplicate);

	editMenu->AppendSeparator();

	wxMenuItem* editMenuPreferences;
	editMenuPreferences = new wxMenuItem(editMenu, wxID_ANY, wxString(wxT("Preferences")), wxEmptyString, wxITEM_NORMAL);
	editMenu->Append(editMenuPreferences);

	menuBarMain->Append(editMenu, wxT("Edit"));

	viewMenu = new wxMenu();
	wxMenuItem* viewMenuMinimize;
	viewMenuMinimize = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Minimize\tWindows + Down")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuMinimize);

	wxMenuItem* viewMenuMaximize;
	viewMenuMaximize = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Maximize\tWindows + Up")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuMaximize);

	wxMenuItem* viewMenuFullscreen;
	viewMenuFullscreen = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Fullscreen\tAlt + Enter")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuFullscreen);

	viewMenu->AppendSeparator();

	wxMenuItem* viewMenuZoomIn;
	viewMenuZoomIn = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Zoom In\tCtrl + ]")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuZoomIn);

	wxMenuItem* viewMenuZoomOut;
	viewMenuZoomOut = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Zoom Out\tCtrl + [")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuZoomOut);

	wxMenuItem* viewMenuResetZoom;
	viewMenuResetZoom = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Reset Zoom\tCtrl + \\")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuResetZoom);

	viewMenu->AppendSeparator();

	wxMenuItem* viewMenuDisplayLines;
	viewMenuDisplayLines = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Display Line Numbers")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuDisplayLines);

	wxMenuItem* viewMenuDisplayOutput;
	viewMenuDisplayOutput = new wxMenuItem(viewMenu, wxID_ANY, wxString(wxT("Display Build Output")), wxEmptyString, wxITEM_NORMAL);
	viewMenu->Append(viewMenuDisplayOutput);

	menuBarMain->Append(viewMenu, wxT("View"));

	buildMenu = new wxMenu();
	wxMenuItem* buildMenuBuildCurrent;
	buildMenuBuildCurrent = new wxMenuItem(buildMenu, wxID_ANY, wxString(wxT("Build Current File\tCtrl + B")), wxEmptyString, wxITEM_NORMAL);
	buildMenu->Append(buildMenuBuildCurrent);

	wxMenuItem* buildMenuBuildAll;
	buildMenuBuildAll = new wxMenuItem(buildMenu, wxID_ANY, wxString(wxT("Build All Files\tF7")), wxEmptyString, wxITEM_NORMAL);
	buildMenu->Append(buildMenuBuildAll);

	buildMenu->AppendSeparator();

	wxMenuItem* buildMenuCleanCurrent;
	buildMenuCleanCurrent = new wxMenuItem(buildMenu, wxID_ANY, wxString(wxT("Clean Current File\tCtrl + Shift + B")), wxEmptyString, wxITEM_NORMAL);
	buildMenu->Append(buildMenuCleanCurrent);

	wxMenuItem* buildMenuCleanAll;
	buildMenuCleanAll = new wxMenuItem(buildMenu, wxID_ANY, wxString(wxT("Clean All Files\tCtrl + F7")), wxEmptyString, wxITEM_NORMAL);
	buildMenu->Append(buildMenuCleanAll);

	buildMenu->AppendSeparator();

	wxMenuItem* buildMenuManage;
	buildMenuManage = new wxMenuItem(buildMenu, wxID_ANY, wxString(wxT("Manage\tF6")), wxEmptyString, wxITEM_NORMAL);
	buildMenu->Append(buildMenuManage);

	menuBarMain->Append(buildMenu, wxT("Build"));

	debugMenu = new wxMenu();
	wxMenuItem* debugMenuRunWith;
	debugMenuRunWith = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Start Current With Debugging\tF5")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuRunWith);

	wxMenuItem* debugMenuRunWithout;
	debugMenuRunWithout = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Run Current Without Debugging\tCtrl + F5")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuRunWithout);

	debugMenu->AppendSeparator();

	wxMenuItem* debugMenuSetBreakpoint;
	debugMenuSetBreakpoint = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Set Breakpoint\tF9")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuSetBreakpoint);

	wxMenuItem* debugMenuClearBreakpoint;
	debugMenuClearBreakpoint = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Clear Breakpoint\tCtrl + F9")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuClearBreakpoint);

	debugMenu->AppendSeparator();

	wxMenuItem* debugMenuEnableAllBreakpoints;
	debugMenuEnableAllBreakpoints = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Enable All Breakpoints\tF8")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuEnableAllBreakpoints);

	wxMenuItem* debugMenuDisableAllBreakpoints;
	debugMenuDisableAllBreakpoints = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Disable All Breakpoints\tCtrl + F8")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuDisableAllBreakpoints);

	debugMenu->AppendSeparator();

	wxMenuItem* debugMenuSingleStep;
	debugMenuSingleStep = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Single Step\tF4")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuSingleStep);

	wxMenuItem* debugMenuContinue;
	debugMenuContinue = new wxMenuItem(debugMenu, wxID_ANY, wxString(wxT("Continue\tF5")), wxEmptyString, wxITEM_NORMAL);
	debugMenu->Append(debugMenuContinue);

	menuBarMain->Append(debugMenu, wxT("Debug"));

	windowMenu = new wxMenu();
	wxMenuItem* windowMenuCloseCurrent;
	windowMenuCloseCurrent = new wxMenuItem(windowMenu, wxID_ANY, wxString(wxT("Close Current File\tCtrl + W")), wxEmptyString, wxITEM_NORMAL);
	windowMenu->Append(windowMenuCloseCurrent);

	wxMenuItem* windowMenuCloseAll;
	windowMenuCloseAll = new wxMenuItem(windowMenu, wxID_ANY, wxString(wxT("Close All Files\tCtrl + Shift + W")), wxEmptyString, wxITEM_NORMAL);
	windowMenu->Append(windowMenuCloseAll);

	windowMenu->AppendSeparator();

	wxMenuItem* windowMenuReopenLast;
	windowMenuReopenLast = new wxMenuItem(windowMenu, wxID_ANY, wxString(wxT("Reopen Last\tCtrl + T")), wxEmptyString, wxITEM_NORMAL);
	windowMenu->Append(windowMenuReopenLast);

	wxMenuItem* windowMenuReopenAll;
	windowMenuReopenAll = new wxMenuItem(windowMenu, wxID_ANY, wxString(wxT("Reopen All Files\tCtrl + Shift + T")), wxEmptyString, wxITEM_NORMAL);
	windowMenu->Append(windowMenuReopenAll);

	menuBarMain->Append(windowMenu, wxT("Window"));

	this->SetMenuBar(menuBarMain);


	this->Centre(wxBOTH);
}

mainFrame::~mainFrame()
{
}
