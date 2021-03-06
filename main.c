// 7 january 2015
#include "tablepriv.h"

static const handlerfunc handlers[] = {
	eventHandlers,
	childrenHandlers,
	resizeHandler,
	drawHandlers,
	apiHandlers,
	hscrollHandler,
	vscrollHandler,
	accessibilityHandler,
	modelNotificationHandler,
	enableFocusHandlers,
	NULL,
};

// TODO migrate this
// TODO check all of these functions for failure and/or copy comments from hscroll.c
static LRESULT CALLBACK tableWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	struct table *t;
	LRESULT lResult;

	t = (struct table *) GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	if (t == NULL) {
		// we have to do things this way because creating the header control will fail mysteriously if we create it first thing
		// (which is fine; we can get the parent hInstance this way too)
		// we use WM_CREATE because we have to use WM_DESTROY to destroy the header; we can't do it in WM_NCDESTROY because Windows will have destroyed it for us by then, and let's match message pairs to be safe
		if (uMsg == WM_CREATE) {
			CREATESTRUCTW *cs = (CREATESTRUCTW *) lParam;

			t = (struct table *) tableAlloc(sizeof (struct table));
			if (t == NULL) {
				logMemoryExhausted("error allocating internal Table data structure");
				// ABORT CREATION
				// TODO correct value?
				// TODO last error or some other way to set error?
				return FALSE;
			}
			t->hwnd = hwnd;
			t->hInstance = cs->hInstance;
			// TODO use t->hInstance here
			makeHeader(t, cs->hInstance);
			t->selectedRow = -1;
			t->selectedColumn = -1;
			loadCheckboxThemeData(t);
			t->model = nullModel;
			initTableAcc(t);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR) t);
		}
		// even if we did the above, fall through
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_DESTROY) {
		// TODO free appropriate (after figuring this part out) components of t
		uninitTableAcc(t);
		// TODO what if any of this fails?
		if (t->model != nullModel)
			tableModel_tableUnsubscribe(t->model, t->hwnd);
		tableModel_Release(t->model);
		freeCheckboxThemeData(t);
		destroyHeader(t);
		tableFree(t);//TODO, "error allocating internal Table data structure");
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	if (runHandlers(handlers, t, uMsg, wParam, lParam, &lResult))
		return lResult;
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static HINSTANCE hInstance;

#define wantedICCClasses ( \
	ICC_LISTVIEW_CLASSES |		/* table headers */		\
	ICC_BAR_CLASSES |			/* tooltips */			\
	0)

// TODO WINAPI or some equivalent instead of __stdcall?
// TODO return HRESULT and store the ATOM in a parameter
// TODO provide a reserved parameter for configuration
__declspec(dllexport) ATOM __stdcall tableInit(void)
{
	INITCOMMONCONTROLSEX icc;
	WNDCLASSW wc;
	ATOM a;

	ZeroMemory(&icc, sizeof (INITCOMMONCONTROLSEX));
	icc.dwSize = sizeof (INITCOMMONCONTROLSEX);
	icc.dwICC = wantedICCClasses;
	if (InitCommonControlsEx(&icc) == 0) {
		logLastError("error initializing Common Controls library for Table");
		return 0;
	}
	ZeroMemory(&wc, sizeof (WNDCLASSW));
	wc.lpszClassName = tableWindowClass;
	wc.lpfnWndProc = tableWndProc;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	if (wc.hCursor == NULL) {
		logLastError("error loading Table window class cursor");
		return 0;		// pass last error up
	}
	wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	if (wc.hIcon == NULL) {
		logLastError("error loading Table window class icon");
		return 0;		// pass last error up
	}
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);		// TODO correct? (check list view behavior on COLOR_WINDOW change)
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	a = RegisterClassW(&wc);
	if (a == 0)
		logLastError("error registering Table window class");
	return a;		// pass last error up if a == 0
}

__declspec(dllexport) HINSTANCE __stdcall tableHINSTANCE(void)
{
	return hInstance;
}

// TODO consider DisableThreadLibraryCalls() (will require removing ALL C runtime calls); if we do so we will also need to fix the signature below
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInstance = hinstDLL;
	// TODO provide cleanup function
	return TRUE;
}

// TODO don't duplicate the GUIDs
__declspec(dllexport) const IID IID_tableModel = { 0x8f361d46, 0xcaab, 0x489f, { 0x8d, 0x20, 0xae, 0xaa, 0xea, 0xa9, 0x10, 0x4f } };
