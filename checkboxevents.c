// 16 august 2014
#include "tablepriv.h"

// TODO wait did we really need WM_MOUSELEAVE after all?

// TODO http://stackoverflow.com/a/22695333/3408572

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
