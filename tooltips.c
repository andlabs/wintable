// 18 march 2015
#include "tablepriv.h"

// TODO rename to tooltip.c?

// TODO set font
HRESULT makeTooltip(struct table *t, HINSTANCE hInstance)
{
	TOOLINFOW ti;

	// TODO verify extended styles and WS_POPUP
	// TODO TTS_NOANIMATE and TTS_NOFADE? check list view control after changing transition animation
	// TODO TTS_ALWAYSTIP? check list view control
	t->tooltip = CreateWindowExW(WS_EX_TOOLWINDOW,
		TOOLTIPS_CLASSW, L"",
		WS_POPUP | TTS_NOPREFIX,
		0, 0,
		0, 0,		// TODO really?
		// TODO really NULL control ID?
		t->hwnd, NULL, hInstance, NULL);
	if (t->tooltip == NULL)
		return logLastError("error creating Table tooltip");
	ZeroMemory(&ti, sizeof (TOOLINFOW));
	ti.cbSize = sizeof (TOOLINFOW);
	// TODO TTF_TRANSPARENT?
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = t->hwnd;
	ti.uId = (UINT_PTR) (t->hwnd);
	ti.hinst = hInstance;		// TODO
	// TODO only needed on wine?
	ti.lpszText = L"initial text that you should not see";
	if (SendMessageW(t->tooltip, TTM_ADDTOOL, 0, (LPARAM) (&ti)) == FALSE)
		return logLastError("error setting up Table tooltip");
	return S_OK;
}

// TODO really error out here? it's cleanup...
HRESULT destroyTooltip(struct table *t)
{
	if (DestroyWindow(t->tooltip) == 0)
		return logLastError("error destroying Table tooltip");
	return S_OK;
}

HANDLER(tooltipNotifyHandler)
{
	NMHDR *nmhdr = (NMHDR *) lParam;
	struct metrics m;
	struct rowcol rc;
	int coltype;
	RECT r;
	HRESULT hr;

	if (nmhdr->hwndFrom != t->tooltip)
		return FALSE;
	if (nmhdr->code != TTN_SHOW)
		return FALSE;
	hr = metrics(t, &m);
	if (hr != S_OK)
		;	// TODO
//TODO	hr = lParamToRowColumn(t, &m, xxxxxx, &rc);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr != S_FALSE)	// not in a cell
		;	// TODO not in a cell
	hr = tableModel_tableColumnType(t->model, rc.column, &coltype);
	if (hr != s_OK)
		;	// TODO
	if (coltype != tableModelColumnString)
		;	// TODO not a text cell
	// TODO set text
	// TODO S_FALSE?
	// don't crop to the visible area; we want the tooltip to hang off the edge if we're scrolled to the right (the real listview acts like this)
	hr = rowColumnToClientRect(t, &m, rc, &r);
	if (hr != S_OK)
		;	// TODO
	// TODO split into its own function
	toCellContentRect(t, &r, 0, 0, m->textHeight);
	// TODO ClientToScreen() instead?
	// TODO this error handling is wrong
	if (MapWindowRect(t->hwnd, NULL, &r) == 0)
		;	// TODO
	if (SendMessageW(t->tooltip, TTM_ADJUSTRECT, (WPARAM) TRUE, (LPARAM) (&r)) == 0)
		;	// TODO
	if (SetWindowPos(t->tooltip, NULL, r.left, r.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER) == 0)
		;	// TODO
	*lResult = (LRESULT) TRUE;
	return TRUE;
}
