// 16 august 2014
#include "tablepriv.h"

// TODO
#define panic(...) abort()

static UINT dfcState(int cbstate)
{
	UINT ret;

	ret = DFCS_BUTTONCHECK;
	if ((cbstate & checkboxStateChecked) != 0)
		ret |= DFCS_CHECKED;
	if ((cbstate & checkboxStateHot) != 0)
		ret |= DFCS_HOT;
	if ((cbstate & checkboxStatePushed) != 0)
		ret |= DFCS_PUSHED;
	return ret;
}

static DWORD drawFrameControlCheckbox(HDC dc, RECT *r, int cbState)
{
	if (DrawFrameControl(dc, r, DFC_BUTTON, dfcState(cbState)) == 0)
		return panicLastError("error drawing Table checkbox image with DrawFrameControl()");
	return 0;
}

static HRESULT getFrameControlCheckboxSize(HDC dc, int *width, int *height)
{
	// there's no real metric around
	// let's use SM_CX/YSMICON and hope for the best
	*width = GetSystemMetrics(SM_CXSMICON);
	*height = GetSystemMetrics(SM_CYSMICON);
	return S_OK;
}

static int themestates[checkboxnStates] = {
	CBS_UNCHECKEDNORMAL,			// 0
	CBS_CHECKEDNORMAL,				// checked
	CBS_UNCHECKEDHOT,				// hot
	CBS_CHECKEDHOT,					// checked | hot
	CBS_UNCHECKEDPRESSED,			// pushed
	CBS_CHECKEDPRESSED,				// checked | pushed
	CBS_UNCHECKEDPRESSED,			// hot | pushed
	CBS_CHECKEDPRESSED,				// checked | hot | pushed
};

static HRESULT getStateSize(HDC dc, int cbState, HTHEME theme, SIZE *s)
{
	// TODO rename this and all future instances to hr
	HRESULT res;

	res = GetThemePartSize(theme, dc, BP_CHECKBOX, themestates[cbState], NULL, TS_DRAW, s);
	if (res != S_OK)
		return panicHRESULT("error getting theme part size for Table checkboxes", res);
	return res;
}

static HRESULT drawThemeCheckbox(HDC dc, RECT *r, int cbState, HTHEME theme)
{
	HRESULT res;

	res = DrawThemeBackground(theme, dc, BP_CHECKBOX, themestates[cbState], r, NULL);
	if (res != S_OK)
		return panicHRESULT("error drawing Table checkbox image from theme", res);
	return res;
}

static HRESULT getThemeCheckboxSize(HDC dc, int *width, int *height, HTHEME theme)
{
	HRESULT res;
	SIZE size;
	int cbState;

	res = getStateSize(dc, 0, theme, &size);
	if (res != S_OK)
		return res;
	for (cbState = 1; cbState < checkboxnStates; cbState++) {
		SIZE against;

		res = getStateSize(dc, cbState, theme, &res);
		if (res != S_OK)
			return res;
		if (size.cx != against.cx || size.cy != against.cy)
			panic("size mismatch in Table checkbox states");
	}
	*width = (int) size.cx;
	*height = (int) size.cy;
	return S_OK;
}

HRESULT drawCheckbox(struct table *t, HDC dc, RECT *r, int cbState)
{
	DWORD le;

	if (t->theme != NULL)
		return drawThemeCheckbox(dc, r, cbState, t->theme);
	le = drawFrameControlCheckbox(dc, r, cbState);
	if (le != 0)
		return HRESULT_FROM_WIN32(le);
	return S_OK;
}

// TODO really panic on failure?
HRESULT freeCheckboxThemeData(struct table *t)
{
	if (t->theme != NULL) {
		HRESULT res;

		res = CloseThemeData(t->theme);
		if (res != S_OK)
			return panicHRESULT("error closing Table checkbox theme", res);
		t->theme = NULL;
	}
	return S_OK;
}

// TODO really panic on failure to ReleaseDC()?
HRESULT loadCheckboxThemeData(struct table *t)
{
	HDC dc;
	HRESULT hr;

	hr = freeCheckboxThemeData(t);
	if (hr != S_OK)
		return hr;
	dc = GetDC(t->hwnd);
	if (dc == NULL)
		return panicLastErrorAsHRESULT("error getting Table DC for loading checkbox theme data");
	// ignore error; if it can't be done, we can fall back to DrawFrameControl()
	if (t->theme == NULL)		// try to open the theme
		t->theme = OpenThemeData(t->hwnd, L"button");
	if (t->theme != NULL)		// use the theme
		hr = getThemeCheckboxSize(dc, &(t->checkboxWidth), &(t->checkboxHeight), t->theme);
	else						// couldn't open; fall back
		hr = getFrameControlCheckboxSize(dc, &(t->checkboxWidth), &(t->checkboxHeight));
	if (hr != S_OK)
		return hr;
	if (ReleaseDC(t->hwnd, dc) == 0)
		return panicLastErrorAsHRESULT("error releasing Table DC for loading checkbox theme data");
	return S_OK;
}

static DWORD redrawCheckboxRect(struct table *t, LPARAM lParam)
{
	struct rowcol rc;
	RECT r;
	DWORD le;
	BOOL visible;

	le = lParamToRowColumn(t, lParam, &rc);
	if (le != 0)
		;	// TODO
	if (rc.row == -1 && rc.column == -1)
		return 0;
	if (t->columnTypes[rc.column] != tableColumnCheckbox)
		return 0;
	le = rowColumnToClientRect(t, rc, &r, &visible);
	if (le != 0)
		return le;
	if (!visible)
		return 0;
	// TODO only the checkbox rect?
	if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
		return panicLastError("error redrawing Table checkbox rect for mouse events");
	return 0;
}

// TODO what happens if any of these functions fail?
HANDLER(checkboxMouseMoveHandler)
{
	// TODO make sure this function is even needed
	// in particular, see http://stackoverflow.com/a/28731761/3408572

	// don't actually check to see if the mouse is in the checkbox rect
	// if there's scrolling without mouse movement, that will change
	// instead, drawCell() will handle it
	if (!t->checkboxMouseOverLast)
		t->checkboxMouseOverLast = TRUE;
	else
		redrawCheckboxRect(t, t->checkboxMouseOverLastPoint);
	t->checkboxMouseOverLastPoint = lParam;
	redrawCheckboxRect(t, t->checkboxMouseOverLastPoint);
	*lResult = 0;
	return TRUE;
}

// TODO if we click on a partially invisible checkbox, should the mouse be moved up along with the scroll?
// TODO what happens if any of these fail?
HANDLER(checkboxMouseDownHandler)
{
	struct rowcol rc;
	RECT r;
	POINT pt;
	DWORD le;
	BOOL visible;

	le = lParamToRowColumn(t, lParam, &rc);
	if (le != 0)
		;	// TODO
	if (rc.row == -1 || rc.column == -1)
		return FALSE;
	if (t->columnTypes[rc.column] != tableColumnCheckbox)
		return FALSE;
	le = rowColumnToClientRect(t, rc, &r, &visible);
	if (le != 0)
		;	// TODO
	if (!visible)
		return FALSE;
	toCheckboxRect(t, &r, 0);
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	if (PtInRect(&r, pt) == 0)
		return FALSE;
	t->checkboxMouseDown = TRUE;
	t->checkboxMouseDownRow = rc.row;
	t->checkboxMouseDownColumn = rc.column;
	SetCapture(t->hwnd);
	// TODO redraw the whole cell?
	if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
		panic("error redrawing Table checkbox after mouse down");
	*lResult = 0;
	return TRUE;
}

// TODO what happens if any of these fail?
HANDLER(checkboxMouseUpHandler)
{
	struct rowcol rc;
	RECT r;
	POINT pt;
	DWORD le;
	BOOL visible;

	if (!t->checkboxMouseDown)
		return FALSE;
	// the logic behind goto wrongUp is that the mouse must be released on the same checkbox
	le = lParamToRowColumn(t, lParam, &rc);
	if (le != 0)
		;	// TODO
	if (rc.row == -1 || rc.column == -1)
		goto wrongUp;
	if (rc.row != t->checkboxMouseDownRow || rc.column != t->checkboxMouseDownColumn)
		goto wrongUp;
	if (t->columnTypes[rc.column] != tableColumnCheckbox)
		goto wrongUp;
	le = rowColumnToClientRect(t, rc, &r, &visible);
	if (le != 0)
		;	// TODO
	if (!visible)
		goto wrongUp;
	toCheckboxRect(t, &r, 0);
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	if (PtInRect(&r, pt) == 0)
		goto wrongUp;
	notify(t, tableNotificationCellCheckboxToggled, rc.row, rc.column, 0);
	t->checkboxMouseDown = FALSE;
	// NOW ReleaseCapture() so we can ignore the WM_CAPTURECHANGED
	if (ReleaseCapture() == 0)
		panic("error releasing mouse capture on mouse up in TODO actually write this");
	// TODO redraw the whole cell?
	if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
		panic("error redrawing Table checkbox after mouse up");
	// TODO really only the row? no way to specify column too?
	NotifyWinEvent(EVENT_OBJECT_STATECHANGE, t->hwnd, OBJID_CLIENT, rc.row);
	*lResult = 0;
	return TRUE;
wrongUp:
	if (t->checkboxMouseDown) {
		rc.row = t->checkboxMouseDownRow;
		rc.column = t->checkboxMouseDownColumn;
		le = rowColumnToClientRect(t, rc, &r, &visible);
		if (le != 0)
			;	// TODO
		if (visible)
			// TODO only the checkbox rect?
			if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
				panic("error redrawing Table checkbox rect for aborted mouse up event");
	}
	// if we landed on another checkbox, be sure to draw that one too
	if (t->checkboxMouseOverLast)
		redrawCheckboxRect(t, t->checkboxMouseOverLastPoint);
	t->checkboxMouseDown = FALSE;
	if (ReleaseCapture() == 0)
		panic("error releasing mouse capture on aborted mouse up in TODO actually write this");
	return FALSE;		// TODO really?
}

// TODO what happens if any of these fail?
HANDLER(checkboxCaptureChangedHandler)
{
	struct rowcol rc;
	RECT r;
	DWORD le;
	BOOL visible;

	if (!t->checkboxMouseDown)
		return FALSE;
	// abort without checking anything
	// TODO merge with above
	if (t->checkboxMouseDown) {
		rc.row = t->checkboxMouseDownRow;
		rc.column = t->checkboxMouseDownColumn;
		le = rowColumnToClientRect(t, rc, &r, &visible);
		if (le != 0)
			;	// TODO
		if (visible)
			// TODO only the checkbox rect?
			if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
				panic("error redrawing Table checkbox rect for aborted mouse up event");
	}
	// if we landed on another checkbox, be sure to draw that one too
	if (t->checkboxMouseOverLast)
		redrawCheckboxRect(t, t->checkboxMouseOverLastPoint);
	t->checkboxMouseDown = FALSE;
	// DON'T call ReleaseCapture() here; we've already lost the capture by this point (see http://stackoverflow.com/questions/28729808/do-i-have-the-right-idea-with-using-setcapture-for-a-windowless-checkbox#comment45750793_28731761)
	*lResult = 0;
	return TRUE;
}
