// 7 january 2015

#include "tablepriv.h"

/* TODO
static const handlerfunc handlers[] = {
	eventHandlers,
	childrenHandlers,
	resizeHandler,
	drawHandlers,
	apiHandlers,
	hscrollHandler,
	vscrollHandler,
	accessibilityHandler,
	NULL,
};
*/

// TODO migrate this
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

/*TODO
			t = (struct table *) tableAlloc(sizeof (struct table), "error allocating internal Table data structure");
			t->hwnd = hwnd;
			makeHeader(t, cs->hInstance);
			t->selectedRow = -1;
			t->selectedColumn = -1;
			loadCheckboxThemeData(t);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR) t);
*/
		}
		// even if we did the above, fall through
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_DESTROY) {
/*TODO
		// TODO free appropriate (after figuring this part out) components of t
		invalidateTableAccs(t);
		freeCheckboxThemeData(t);
		destroyHeader(t);
		tableFree(t, "error allocating internal Table data structure");
*/
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
/*TODO
	if (runHandlers(handlers, t, uMsg, wParam, lParam, &lResult))
		return lResult;
*/
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// TODO is this the best way to store the DLL hInstance?
static HINSTANCE hInstance;

// TODO WINAPI or some equivalent instead of __stdcall?
// TODO initialize common controls here?
__declspec(dllexport) ATOM __stdcall tableInit(void)
{
	WNDCLASSW wc;
	ATOM a;

	ZeroMemory(&wc, sizeof (WNDCLASSW));
	wc.lpszClassName = tableWindowClass;
	wc.lpfnWndProc = tableWndProc;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	if (wc.hCursor == NULL) {
		panicLastError("error loading Table window class cursor");
		return 0;		// pass last error up
	}
	wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	if (wc.hIcon == NULL) {
		panicLastError("error loading Table window class icon");
		return 0;		// pass last error up
	}
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);		// TODO correct? (check list view behavior on COLOR_WINDOW change)
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	a = RegisterClassW(&wc);
	if (a == 0)
		panicLastError("error registering Table window class");
	return a;		// pass last error up if a == 0
}

// TODO consider DisableThreadLibraryCalls() (will require removing ALL C runtime calls)
// TODO make sure this is the correct name for C runtime initialization (CHECK MSDN OR THE HEADERS)
// TODO can hinstDLL ever change? (see above about hInstance)
// TODO handle DLL_PROCESS_DETACH?
// TODO __declspec(dllexport)?
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInstance = hinstDLL;
	return TRUE;
}

// this is solely for the test program's use and will be removed (TODO) when we're ready to split
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
HINSTANCE tableTestProgramInit(void)
{
	hInstance = (HINSTANCE) (&__ImageBase);
	return hInstance;
}
