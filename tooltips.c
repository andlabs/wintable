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
	// TODO figure out and explain why TTF_TRANSPARENT is necessary (if it is, anyway; it seems to be)
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
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

/* comctl32 listview behavior notes
when the mouse hoves over a cell, the tooltip should show
when the mouse hovers over another cell, the tooltip should hide [TODO? and the tooltip counter should restart]
it doesn't matter if the tooltip is wider than the cell; once the mouse leaves the cell, the tooltip hides
TODO xoff?
*/

// TODO this seems rather hackish; is it necessary?
// TODO if so, how do we handle captures? and mouse leaves?
EVENTHANDLER(tooltipMouseMoveHandler)
{
	t->tooltipMouseMoveLPARAM = lParam;
	return TRUE;
}

// TODO can this be fired before a WM_MOUSEMOVE?
// TODO makek this follow the cursor
// TODO is it supposed to disappear after a few seconds and then never reappear until the taskbar shows a tooltip?
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
	hr = lParamToRowColumn(t, &m, t->tooltipMouseMoveLPARAM, &rc);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr != S_FALSE)	// not in a cell
		;	// TODO not in a cell
	hr = tableModel_tableColumnType(t->model, rc.column, &coltype);
	if (hr != S_OK)
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
	toCellContentRect(t, &r, 0, 0, m.textHeight);
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
