// 18 march 2015
#include "tablepriv.h"

// TODO rename to tooltip.c?

static void initTOOLINFOW(struct table *t, TOOLINFOW *ti)
{
	ZeroMemory(ti, sizeof (TOOLINFOW));
	ti->cbSize = sizeof (TOOLINFOW);
	ti->hwnd = t->hwnd;
	ti->uId = (UINT_PTR) (t->hwnd);
	ti->hinst = t->tooltipHINSTANCE;
}

// we need to handle WM_WINDOWPOSCHANGED to forcibly hide unwanted tooltips
// this is because the system indiscriminately shows tooltips after a TTN_SHOW (all we can do is change the tooltip size, position, text, etc.)
// as for what we're doing, this is what .net does! see http://referencesource.microsoft.com/#System.Windows.Forms/winforms/Managed/System/WinForms/ToolTip.cs,2137
// (note that NativeWindow.DefWndProc() is really DefSubclassProc(), not DefWindowProc(); the .net tooltip is really just a wrapper around the comctl32 tooltip)
static LRESULT CALLBACK tooltipSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR data)
{
	struct table *t = (struct table *) data;

	switch (uMsg) {
	case WM_WINDOWPOSCHANGED:
		if (t->cancelTooltip) {
			t->cancelTooltip = FALSE;
			ShowWindow(t->tooltip, SW_HIDE);
			return 0;
		}
		// otherwise defer to DefSubclassProc()
		break;
	case WM_NCDESTROY:
		// TODO what to return here?
		if (RemoveWindowSubclass(hwnd, tooltipSubclassProc, uIdSubclass) == FALSE)
			logLastError("error removing Table tooltip subclass in tooltipSubclassProc()");
		// fall out to DefSubclassProc()
	}
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// TODO set font
HRESULT makeTooltip(struct table *t, HINSTANCE hInstance)
{
	TOOLINFOW ti;

	t->tooltipHINSTANCE = hInstance;
	// TODO verify extended styles and WS_POPUP
	// TODO TTS_NOANIMATE and TTS_NOFADE? check list view control after changing transition animation
	// TODO TTS_ALWAYSTIP? check list view control
	t->tooltip = CreateWindowExW(WS_EX_TOOLWINDOW,
		TOOLTIPS_CLASSW, L"",
		WS_POPUP | TTS_NOPREFIX,
		0, 0,
		0, 0,		// TODO really?
		// TODO really NULL control ID?
		t->hwnd, NULL, t->tooltipHINSTANCE, NULL);
	if (t->tooltip == NULL)
		return logLastError("error creating Table tooltip");
	initTOOLINFOW(t, &ti);
	// TODO figure out and explain why TTF_TRANSPARENT is necessary (if it is, anyway; it seems to be)
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
	// TODO only needed on wine?
	ti.lpszText = L"initial text that you should not see";
	if (SendMessageW(t->tooltip, TTM_ADDTOOL, 0, (LPARAM) (&ti)) == FALSE)
		return logLastError("error setting up Table tooltip");
	if (SetWindowSubclass(t->tooltip, tooltipSubclassProc, 0, (DWORD_PTR) t) == FALSE)
		return logLastError("error subclassing Table tooltip");
	return S_OK;
}

// TODO really error out here? it's cleanup...
HRESULT destroyTooltip(struct table *t)
{
	// the tooltip is already destroyed by this point
	// seemingly, anyway (TODO)
	// see http://stackoverflow.com/questions/29419291/is-my-subclassing-common-controls-tooltip-destroying-itself-in-wm-destroy-before
	return S_OK;
}

/* comctl32 listview behavior notes
when the mouse hoves over a cell, the tooltip should show
when the mouse hovers over another cell, the tooltip should hide [TODO? and the tooltip counter should restart]
it doesn't matter if the tooltip is wider than the cell; once the mouse leaves the cell, the tooltip hides
TODO xoff?
TODO on mouse leave?
*/

// TODO make sure this matches the above behavior
// TODO rename t->tooltipMouseMoved to t->tooltipShown? and change its being set accordingly?
// TODO how do we handle captures? and mouse leaves? and scrolls?
EVENTHANDLER(tooltipMouseMoveHandler)
{
	BOOL lastMouseMoved;
	struct rowcol rc;
	HRESULT hr;

	lastMouseMoved = t->tooltipMouseMoved;
	t->tooltipMouseMoved = TRUE;
	t->tooltipMouseMoveLPARAM = lParam;
	hr = lParamToRowColumn(t, m, t->tooltipMouseMoveLPARAM, &rc);
	if (hr != S_OK)
		;	// TODO
	if (lastMouseMoved)
		if (!rowcolEqual(rc, t->tooltipMouseMoveRowColumn))
			// TODO will this reset the tooltip timer, if needed?
			SendMessageW(t->tooltip, TTM_POP, 0, 0);
	t->tooltipMouseMoveRowColumn = rc;
	return TRUE;
}

// TODO can this be fired before a WM_MOUSEMOVE?
// TODO makek this follow the cursor
// TODO is it supposed to disappear after a few seconds and then never reappear until the taskbar shows a tooltip?
// TODO reorganize the parts of this function
HANDLER(tooltipNotifyHandler)
{
	NMHDR *nmhdr = (NMHDR *) lParam;
	struct metrics m;
	struct rowcol rc;
	int coltype;
	RECT r;
	tableCellValue value;
	TOOLINFOW ti;
	HDC dc;
	HFONT newfont, prevfont;
	SIZE extents;
	HRESULT hr;
	DWORD le;

	if (nmhdr->hwndFrom != t->tooltip)
		return FALSE;
	if (nmhdr->code != TTN_SHOW)
		return FALSE;

	// not only do we need the Table's metrics, we also need a DC to determine if the current cell's text doesn't fit
	dc = GetDC(t->hwnd);
	if (dc != NULL)
		;	// TODO
	hr = selectFont(t, dc, &newfont, &prevfont);
	if (hr != S_OK)
		;	// TODO
	hr = getMetrics(t, dc, FALSE, &m);
	if (hr != S_OK)
		;	// TODO

	// figure out which cell we're on and whether it has text
	// TODO just use t->tooltipMouseMoveRowColumn?
	// TODO would doing so break with scrolling?
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

	// get the cell's text
	hr = tableModel_tableCellValue(t->model, rc.row, rc.column, &value);
	if (hr != S_OK)
		;	// TODO
	// TODO verify cell type

	// get the size of the text now; we'll check it later
	// TODO find a way to use the same method DrawTextExW() uses (and doesn't require SysStringLen())
	// TODO really use SysStringLen() and not wcslen()?
	if (GetTextExtentPoint32W(dc, value.stringVal, SysStringLen(value.stringVal), &extents) == 0)
		;	// TODO
	// we're done with the DC now
	// TODO really error out if cleanup failed?
	hr = deselectFont(dc, prevfont, newfont);
	if (hr != S_OK)
		;	// TODO
	if (ReleaseDC(t->hwnd, dc) == 0)
		;	// TODO

	// set the tooltip's text
	initTOOLINFOW(t, &ti);
	ti.lpszText = value.stringVal;
	SendMessageW(t->tooltip, TTM_UPDATETIPTEXT, 0, (LPARAM) (&ti));
	SysFreeString(value.stringVal);

	// put the tooltip in the right place
	// TODO S_FALSE?
	// don't crop to the visible area; we want the tooltip to hang off the edge if we're scrolled to the right (the real listview acts like this)
	hr = rowColumnToClientRect(t, &m, rc, &r);
	if (hr != S_OK)
		;	// TODO
	// TODO split into its own function
	toCellContentRect(t, &r, 0, 0, m.textHeight);
	// now that we have the right rect, we can check if the text fits
	if (extents.cx < (r.right - r.left))
		;	// TODO does fit; no tooltip needed
	SetLastError(0);
	if (MapWindowRect(t->hwnd, NULL, &r) == 0) {
		le = GetLastError();
		SetLastError(le);		// just to be safe
		if (le != 0)
			;	// TODO
	}
	if (SendMessageW(t->tooltip, TTM_ADJUSTRECT, (WPARAM) TRUE, (LPARAM) (&r)) == 0)
		;	// TODO
	if (SetWindowPos(t->tooltip, NULL, r.left, r.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER) == 0)
		;	// TODO

	// and we're done
	*lResult = (LRESULT) TRUE;
	return TRUE;
}
