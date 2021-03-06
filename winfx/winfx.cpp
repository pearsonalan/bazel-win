#include <windows.h>
#include <windowsx.h>
#include <string>

#include "winfx.h"

namespace winfx {
	
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Window::~Window() {
}

bool Window::create(LPSTR pstrCmdLine, int nCmdShow) {
	if (!classIsRegistered) {
		if (!registerWindowClass()) {
			return false;
		}
	}

	Point pos = getDefaultWindowPosition();
	Size  sz  = getDefaultWindowSize();
		
    hwnd = CreateWindow(className.c_str(), windowName.c_str(),
						WS_OVERLAPPEDWINDOW,
						pos.x, pos.y,
						sz.cx, sz.cy,
						HWND_DESKTOP, NULL,
						App::getSingleton().getInstance(), this);

    if (!hwnd)
		return false;

    ShowWindow(hwnd,nCmdShow);

    return true;
}

Point Window::getDefaultWindowPosition() {
	return Point(CW_USEDEFAULT, CW_USEDEFAULT);
}

Size Window::getDefaultWindowSize() {
	return Size(CW_USEDEFAULT, CW_USEDEFAULT);
}
	
LRESULT Window::onClose(HWND hwndParam) {
	PostQuitMessage(0);
	return 0;
}

LRESULT Window::onCreate(HWND hwndParam, LPCREATESTRUCT lpCreateStruct) {
	return 1;
}

bool Window::registerWindowClass() {
	WNDCLASS wc;
	 
	// 
	// Register window classes 
	// 

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc; 
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = sizeof(void*);
	wc.hInstance	 = App::getSingleton().getInstance();
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1); 
	wc.lpszMenuName  = 0; 
	wc.lpszClassName = className.c_str(); 

	modifyWndClass(wc);

	// don't allow modification of these
	wc.lpfnWndProc   = WndProc; 
	wc.cbWndExtra	 = sizeof(void*);

	if (!wc.lpszClassName)
		return false;
	
	if (!::RegisterClass(&wc)) 
		return false; 

	classIsRegistered = true;
	return true;
}

void Window::modifyWndClass(WNDCLASS& wc) { 
}

LRESULT Window::handleWindowMessage(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_CREATE, onCreate);
		HANDLE_MSG(hwndParam, WM_CLOSE, onClose);
	}
	
	return DefWindowProc(hwndParam, uMsg, wParam, lParam);
}


LRESULT CALLBACK WndProc(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Window* pWnd;

	if (uMsg == WM_CREATE) {
		pWnd = (Window*) (((LPCREATESTRUCT)lParam)->lpCreateParams);
		::SetWindowLong(hwndParam, GWLP_USERDATA, (LONG_PTR)pWnd);
	} else
		pWnd = (Window*) GetWindowLongPtr(hwndParam, GWLP_USERDATA);

	if (!pWnd)
		return DefWindowProc(hwndParam, uMsg, wParam, lParam);

	return pWnd->handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

std::string Dialog::getItemText(int id) {
	HWND hwndCtl = ::GetDlgItem(hwnd, id);
	if (hwndCtl) {
		int len = Edit_GetTextLength(hwndCtl) + 1;
		char* buffer = static_cast<char*>(_malloca(len * sizeof(char)));
		if (buffer) {
			Edit_GetText(hwndCtl, buffer, len);
			return std::string(buffer);
		}
	}
	return std::string();
}

void Dialog::setItemText(int id, const std::string& str) {
	HWND hwndCtl = GetDlgItem(hwnd, id);
	if (hwndCtl) {
		::SetWindowText(hwndCtl, str.c_str());
	}
}

LRESULT Dialog::onInitDialog(HWND hwndParam, HWND hwndFocus, LPARAM lParam) {
	return TRUE;
}

LRESULT Dialog::onCommand(HWND hwndParam, int id, HWND hwndCtl, UINT codeNotify) {
	switch (id) {
	case IDOK:
		endDialog(IDOK);
		break;

	case IDCANCEL:
		endDialog(IDCANCEL);
		break;
	}

	return TRUE;
}

LRESULT Dialog::handleWindowMessage(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_INITDIALOG, onInitDialog);
		HANDLE_MSG(hwndParam, WM_COMMAND, onCommand);
	}

	return FALSE;
}

void Dialog::create() {
	hwnd = CreateDialogParam(App::getSingleton().getInstance(), MAKEINTRESOURCE(idd),
							 pwndParent->getWindow(), (DLGPROC)DialogProc, (LPARAM)this);
}

int Dialog::doDialogBox() {
	return DialogBoxParam(App::getSingleton().getInstance(), MAKEINTRESOURCE(idd),
						  pwndParent->getWindow(), (DLGPROC)DialogProc, (LPARAM)this);
}

BOOL CALLBACK DialogProc(HWND hwndParam, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Dialog* pdlg;

	if (uMsg == WM_INITDIALOG) {
		pdlg = (Dialog*) lParam ;
		::SetWindowLong(hwndParam, DWLP_USER, lParam);
		pdlg->hwnd = hwndParam;
	} else {
		pdlg = (Dialog*) ::GetWindowLong(hwndParam, DWLP_USER);
	}

	if (!pdlg)
		return FALSE;

	return pdlg->handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

App::~App() {
}

void App::terminate() {
}

bool App::initInstance(HINSTANCE hInstParam, HINSTANCE hInstPrev) {
	hInst = hInstParam;
	return true;
}

bool App::translateModelessMessage(MSG* pmsg) {
	return false;
}

App* App::singleton = NULL;

}  // namespace winfx

int APIENTRY WinMain(_In_ HINSTANCE     hInst,
					 _In_opt_ HINSTANCE hInstPrev,
					 _In_ LPSTR         pstrCmdLine,
					 _In_ int           nCmdShow) {
	winfx::App& app = winfx::App::getSingleton();
	
	if (!app.initInstance(hInst,hInstPrev))
		return app.getExitCode();

	if (!app.initWindow(pstrCmdLine, nCmdShow))
		return app.getExitCode();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!app.translateModelessMessage(&msg)) {
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
	} 

	app.terminate();

    return app.getExitCode();  
}
