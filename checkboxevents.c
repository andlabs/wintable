// 16 august 2014
#include "tablepriv.h"

// TODO wait did we really need WM_MOUSELEAVE after all?

// TODO http://stackoverflow.com/a/22695333/3408572

// returns S_FALSE if we're outside the client rect or not in a checkbox
static HRESULT lParamToCheckbox(struct table *t, struct metrics *m, LPARAM lParam, struct rowcol *rc, RECT *rCell, RECT *rCheckbox, BOOL mutableOnly)
{
	int coltype;
	HRESULT hr;

	// TODO does this make sure we're not in the client rect?
	hr = lParamToRowColumn(t, m, lParam, rc);
	if (hr != S_OK)
		return hr;
	if (rc->row == -1 || rc->column == -1)
		return S_FALSE;
	hr = tableModel_tableColumnType(t->model, t->columns[rc->column].modelColumn, &coltype);
	if (hr != S_OK)
		return logHRESULT("error getting Table moel column type in lParamToCheckbox()", hr);
	if (coltype != tableModelColumnBool)
		return S_FALSE;
	if (mutableOnly) {
		hr = tableModel_tableIsColumnMutable(t->model, rc->column);
		if (hr != S_OK && hr != S_FALSE)
			return logHRESULT("error determining if Table model column is mutable in lParamToCheckbox()", hr);
		if (hr == S_FALSE)		// we only want mutable columns
			return S_FALSE;
	}
	hr = rowColumnToClientRect(t, m, *rc, rCell);
	if (hr != S_OK)			// handles the S_FALSE case
		return hr;
	*rCheckbox = *rCell;
	toCellContentRect(t, rCheckbox, 0, m->checkboxWidth, m->checkboxHeight);
	return S_OK;
}

// TODO what happens if any of these functions fail?
EVENTHANDLER(checkboxMouseMoveHandler)
{
	// TODO see http://stackoverflow.com/a/28731761/3408572
	// TODO when rewriting, handle captures/the possibility that the mouse may not be in the client rect

	struct rowcol rc;
	RECT rCell, rCheckbox;
	HRESULT hr;

	// we don't need to do any sort of logic to figure out what specifically to draw here
	// the drawing code will figure that out
	// we simply need to mark the old and new cells for redraw
	if (t->checkboxMouseMoved) {		// mark the previous cell in case we've changed cells
		// TODO mutable only?
		hr = lParamToCheckbox(t, m, t->checkboxMouseMoveLPARAM, &rc, &rCell, &rCheckbox, FALSE);
		if (hr != S_OK && hr != S_FALSE)
			;	// TODO
		if (hr != S_FALSE)
			if (InvalidateRect(t->hwnd, &rCell, TRUE) == 0)
				;	// TODO
	}
	t->checkboxMouseMoved = TRUE;
	t->checkboxMouseMoveLPARAM = lParam;
	// TODO see if we need to get rid of code duplication here
	// TODO mutable only?
	hr = lParamToCheckbox(t, m, t->checkboxMouseMoveLPARAM, &rc, &rCell, &rCheckbox, FALSE);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr != S_FALSE)
		if (InvalidateRect(t->hwnd, &rCell, TRUE) == 0)
			;	// TODO
	// TODO return FALSE if hr == S_FALSE for both?
	return TRUE;
}

// TODO if we click on a partially invisible checkbox, should the mouse be moved up along with the scroll?
// TODO what happens if any of these fail?
EVENTHANDLER(checkboxMouseDownHandler)
{
	struct rowcol rc;
	RECT rCell, rCheckbox;
	HRESULT hr;

	// definitely mutable only; we can't toggle immutable checkboxes
	hr = lParamToCheckbox(t, m, lParam, &rc, &rCell, &rCheckbox, TRUE);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr == S_FALSE)			// not in a checkbox cell
		return FALSE;
	if (lParamInRect(&rCheckbox, lParam) == 0)
		return FALSE;
	SetCapture(t->hwnd);
	t->checkboxMouseDown = TRUE;
	t->checkboxMouseDownRowColumn = rc;
	if (InvalidateRect(t->hwnd, &rCell, TRUE) == 0)
		;	// TODO
	return TRUE;
}

// TODO what happens if any of these fail?
EVENTHANDLER(checkboxMouseUpHandler)
{
	struct rowcol rc;
	RECT rHoverCell, rHoverCheckbox;
	BOOL redrawHover;
	RECT rDownCell;
	HRESULT hr;

	if (!t->checkboxMouseDown)
		return FALSE;

	// first get out of mouse down mode and release the mouse capture
	// we do things in that order because we want to ignore the WM_CAPTURECHANGED that ReleaseCapture() generates
	t->checkboxMouseDown = FALSE;
	if (ReleaseCapture() == 0)
		;	// TODO

	// now we need to figure out if we released the mouse button in the same checkbox that was initially clicked
	// so we have to figure out which checkbox is being hovered over right now
	// if it is a different checkbox, we need to redraw that one as well, as it will now need to be shown hot
	redrawHover = FALSE;
	// definitely mutable only; we can't toggle immutable checkboxes
	hr = lParamToCheckbox(t, m, lParam, &rc, &rHoverCell, &rHoverCheckbox, TRUE);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr == S_FALSE)		// not in checkbox cell
		goto noToggle;
	redrawHover = TRUE;
	if (rc.row != t->checkboxMouseDownRowColumn.row || rc.column != t->checkboxMouseDownRowColumn.column)
		goto noToggle;
	if (lParamInRect(&rHoverCheckbox, lParam) == 0)
		goto noToggle;

	// okay, we're good; toggle
	hr = tableModel_tableCellToggleBool(t->model, rc.row, rc.column);
	if (hr != S_OK)
		;	// TODO

noToggle:
	// now redraw the checkbox that we clicked
	// do this regardless of whether we toggle; otherwise, the clicked checkbox doesn't get redrawn from its pressed state until later
	hr = rowColumnToClientRect(t, m, t->checkboxMouseDownRowColumn, &rDownCell);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr != S_FALSE)
		if (InvalidateRect(t->hwnd, &rDownCell, TRUE) == 0)
			;	// TODO
	// and redraw the cell being hovered over as well, just in case (see above)
	// TODO could we reliably tell if rHoverCell == rDownCell somehow and avoid the duplicate call (other than comparing the rects)?
	if (redrawHover)
		if (InvalidateRect(t->hwnd, &rHoverCell, TRUE) == 0)
			;	// TODO
	// TODO return FALSE if we jumped over the toggling?
	return TRUE;
}

// TODO WM_CANCELMODE?
// TODO what happens if any of these fail?
EVENTHANDLER(checkboxCaptureChangedHandler)
{
	RECT r;
	HRESULT hr;

	if (!t->checkboxMouseDown)
		return FALSE;
	t->checkboxMouseDown = FALSE;
	// capture was taken before the mouse was released
	// don't release capture or toggle the checkbox (even if every other toggling condition was met)
	// do redraw the checkbox that used to be clicked on
	hr = rowColumnToClientRect(t, m, t->checkboxMouseDownRowColumn, &r);
	if (hr != S_OK && hr != S_FALSE)
		;	// TODO
	if (hr != S_FALSE)
		if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
			;	// TODO
	return TRUE;
}
