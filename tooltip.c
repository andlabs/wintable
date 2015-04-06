// 4 april 2015
#include "tablepriv.h"

/* list view tooltip behavior:
- after hovering over an ellipsized item, the tooltip appears in place
- the tooltip goes away when:
	- vertically scrolling
	- [TODO] horizontally scrolling
	- changing selection to something else
		- in which case, unlike the others, tooltips don't come back until you move the mouse
	- clicking on the hovered-over cell, regardless of whether it is selected
		- same as before
		- EXCEPT if the cell is already selected, in which case the tooltip doesn't come back until you change cells
	- moving the mouse to another cell
	- moving the mouse outside the control
	- moving the mouse onto the nonclient area
	- [TODO] activating the system menu or some other control
- tooltips do NOT go away when
	- toggling the list view checkbox
	- pressing other unhandled keys
- tooltips are INHIBITED during capture (marquee selection in the case of the list view, checkbox mouse down in the case of Table)
- if I move to another cell and back, the tooltip comes back
- the rect that controls whether the tooltip is triggered includes the margin of the cell to the left of the first pixel of its text
- the tooltip shows up with the top left corner of the text would be if we had the cell fully visible, even if we''re scrolled right
- the tooltip does not seem to honor the 80-character limit and will show even absurdly long single-word tooltips
	- [TODO] multi-word labels
- [TODO] tooltips have starting animations
- tooltips have popping fade

TODO what are the timings for each popup?
TODO is the double-click rectangle used to determine if the mouse has moved?
TODO TTS_ALWAYSTIP?
*/

static HRESULT makeTooltip(struct table *t, BSTR text)
{
	TOOLINFOW ti;

printf("makeTooltip()\n");
	// TODO verify extended styles and WS_POPUP
	// TODO TTS_NOANIMATE and TTS_NOFADE? check list view control after changing transition animation
	// TODO TTS_ALWAYSTIP? check list view control
	t->tooltip = CreateWindowExW(WS_EX_TOOLWINDOW,
		TOOLTIPS_CLASSW, L"",
		WS_POPUP | TTS_NOPREFIX,
		0, 0,
		0, 0,		// TODO really?
		// TODO really NULL control ID?
		t->hwnd, NULL, t->hInstance, NULL);
	if (t->tooltip == NULL)
		return logLastError("error creating Table tooltip");

	ZeroMemory(&ti, sizeof (TOOLINFOW));
	ti.cbSize = sizeof (TOOLINFOW);
	// TODO figure out and explain why TTF_TRANSPARENT is necessary (if it is, anyway; it seems to be)
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
	ti.hwnd = t->hwnd;
	ti.uId = (UINT_PTR) (t->hwnd);
	ti.hinst = t->hInstance;
	ti.lpszText = text;
	if (SendMessageW(t->tooltip, TTM_ADDTOOL, 0, (LPARAM) (&ti)) == FALSE)
		return logLastError("error setting up Table tooltip");

//TODO	if (SetWindowSubclass(t->tooltip, tooltipSubclassProc, 0, (DWORD_PTR) t) == FALSE)
//TODO		return logLastError("error subclassing Table tooltip");

	return S_OK;
}

static HRESULT destroyTooltip(struct table *t)
{
printf("destroyTooltip()\n");
	if (t->tooltip != NULL)
		// TODO check for error here? it is cleanup
		if (DestroyWindow(t->tooltip) == 0)
			return logLastError("error destroying existing Table tooltip in destroyTooltip()");
	return S_OK;
}

static HRESULT rescheduleTooltip(struct table *t)
{
printf("rescheduleTooltip()\n");
	// default tooltip show time according to https://msdn.microsoft.com/en-us/library/windows/desktop/bb760404%28v=vs.85%29.aspx
	if (SetTimer(t->hwnd, tooltipTimer, GetDoubleClickTime(), NULL) == 0)
		return logLastError("error rescheduling Table tooltip in rescheduleTooltip()");
	return S_OK;
}

HRESULT popTooltip(struct table *t, BOOL reschedule)
{
	HRESULT hr;

printf("popTooltip()\n");
	if (t->tooltip != NULL) {
		SendMessageW(t->tooltip, TTM_POP, 0, 0);
		if (reschedule) {
			hr = rescheduleTooltip(t);
			if (hr != S_OK)
				return hr;
		}
	}
	return S_OK;
}

EVENTHANDLER(tooltipMouseMoveHandler)
{
	BOOL handled = FALSE;
	struct rowcol rc;
	HRESULT hr;

printf("tooltipMouseMoveHandler() %d %I32X %d %I32X\n",
t->mouseMoved, t->mouseMoveLPARAM,
t->lastMouseMoved, t->lastMouseMoveLPARAM);

if (t->tooltip == NULL)
	if (!mouseHovering(t)) {
		hr = rescheduleTooltip(t);
		if (hr != S_OK)
			;	// TODO
		handled = TRUE;
	}

	return TRUE;
}

EVENTHANDLER(tooltipMouseLeaveHandler)
{
	// TODO
	return FALSE;
}

EVENTHANDLER(tooltipTimerHandler)
{
	struct rowcol rc;
	RECT r;
	tableCellValue value;
	HRESULT hr;

printf("tooltipTimerHandler()\n");
	if (wParam != tooltipTimer)
		return FALSE;
	// TODO does this count as a cleanup function?
	if (KillTimer(t->hwnd, tooltipTimer) == 0)
		;	// TODO

	if (!t->mouseMoved)	// not in client area; nothing to do
		return FALSE;
	hr = lParamToRowColumn(t, m, t->mouseMoveLPARAM, &rc);
	if (hr != S_OK)
		;	// TODO
	hr = rowColumnToClientRect(t, m, rc, &r);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr == S_FALSE)		// still not in client area
		return FALSE;

	hr = tableModel_tableCellValue(t->model, rc.row, rc.column, tableModelColumnString, &value);
	if (hr == tableModelErrorWrongColumnType)		// not a string column; no tooltips needed
		return FALSE;
	if (hr != S_OK)
		;	// TODO

	// TODO measure the width of value.stringVal

	// if there's already a tooltip, we need to get rid of it
	// TODO fade out?
	hr = destroyTooltip(t);
	if (hr != S_OK)
		;	// TODO
	hr = makeTooltip(t, value.stringVal);
	if (hr != S_OK)
		;	// TODO
	SysFreeString(value.stringVal);

	// TODO position r

	SendMessage(t->tooltip, TTM_POPUP, 0, 0);
	return TRUE;
}

HANDLER(tooltipNotifyHandler)
{
	NMHDR *nm = (NMHDR *) lParam;
	HRESULT hr;

printf("tooltipNotifyHandler() %d %d %d\n", nm->code, TTN_SHOW, TTN_POP);
	if (nm->hwndFrom != t->tooltip)
		return FALSE;
	switch (nm->code) {
	case TTN_SHOW:
		// TODO show at given rectangle
		return FALSE;
	case TTN_POP:
		// TODO remove tool
		;
	}
	return FALSE;
}
