// 13 december 2014
#include "tablepriv.h"

// damn winsock
// TODO should failure to ensure visible really be fatal to selection?
HRESULT doselect(struct table *t, struct metrics *m, intmax_t row, intmax_t column)
{
	intmax_t oldrow;
	struct rowcol rc;
	HRESULT hr;

	oldrow = t->selectedRow;
	t->selectedRow = row;
	t->selectedColumn = column;

	// only ensure visibility if we selected something
	if (t->selectedRow != -1 || t->selectedColumn != -1) {
		rc.row = t->selectedRow;
		rc.column = t->selectedColumn;
		hr = ensureVisible(t, m, rc);
		if (hr != S_OK)
			return hr;
	}

	// now redraw the old and new /rows/
	// we do this after scrolling so the rectangles to be invalidated make sense
	if (oldrow != -1) {
		hr = queueRedrawRow(t, m, oldrow);
		if (hr != S_OK)
			return hr;
	}
	if (t->selectedRow != -1) {
		hr = queueRedrawRow(t, m, t->selectedRow);
		if (hr != S_OK)
			return hr;
	}

	// always pop tooltips on selection change, regardless of what caused it
	// TODO pop at the beginning?
	// TODO pop on mouse selection?
//TODO	popTooltip(t);

	return S_OK;
}

// TODO which WM_xBUTTONDOWNs?
// TODO what if any of these functions fail?
EVENTHANDLER(mouseDownSelectHandler)
{
	struct rowcol rc;
	HRESULT hr;

	hr = lParamToRowColumn(t, m, lParam, &rc);
	if (hr != S_OK)
		return FALSE;
	// don't check if lParamToRowColumn() returned row -1 or column -1; we want deselection behavior
	doselect(t, m, rc.row, rc.column);
	return TRUE;
}

/*
the routine below is intended to simulate the comctl32.dll listview keyboard navigation rules, at least as far as vertical navigation is concerned.
horizontal scrolling is different because unlike the comctl32 listview, we say that a single column in each row has the keyboard focus, so left and right navigate between columns here, instead of scrolling left/right by pixels.
	TODO provide an override for scrolling by pixels?
	TODO any other keyboard shortcuts?
		TODO browser keys
		TODO media navigation keys
			TODO XBUTTON1/2?
		TODO clear keys?

keyboard selection behaviors of the windows 7 listview:
with 100 items (0-99), the window currently shows items 30 through 47 as well as having item 48 partially visible
-  item 30:
	- page up -> item 13
	- page down -> item 47
- item 31:
	- page up -> item 30
	- page down -> item 47
- item 42:
	- page up -> item 30
	- page down -> item 47
- item 46:
	- page up -> item 30
	- page down -> item 47
- item 47:
	- page up: -> item 30
	- page down: -> item 64

when nothing is selected:
- down selects item 0 regardless of scroll
- up selects nothing regardless of scroll
- page down selects the last fully visible item depending on scroll
	- so with the above configuration:
		- item 0 -> item 17
		- item 30 -> item 47
		- item 80 -> item 97
- page up selects item 0 regardless of scroll
- home selects item 0 regardless of scroll
- end selects the last item regardless of scroll

for left and right we will simulate up and down, respectively (so right selects row 0 column 0); remember that you can't have either row or column be -1 but not both

TODO what happens if page up and page down are pressed with an item selected and the scroll in a different position?
*/

// TODO what if any of these functions fail
EVENTHANDLER(keyDownSelectHandler)
{
	intmax_t row;
	intmax_t column;
	intmax_t rowCount;

	rowCount = tableModel_tableRowCount(t->model);
	if (rowCount == 0 || t->nColumns == 0)		// no items to select
		return FALSE;
	row = t->selectedRow;
	column = t->selectedColumn;
	switch (wParam) {
	case VK_UP:
		if (row == -1)
			return FALSE;
		row--;
		if (row < 0)
			row = 0;
		break;
	case VK_DOWN:
		if (row == -1) {
			row = 0;
			column = 0;
		} else {
			row++;
			if (row >= rowCount)
				row = rowCount - 1;
		}
		break;
	case VK_LEFT:
		if (column == -1)
			return FALSE;
		column--;
		if (column < 0)
			column = 0;
		break;
	case VK_RIGHT:
		if (column == -1) {
			row = 0;
			column = 0;
		} else {
			column++;
			if (column >= t->nColumns)
				column = t->nColumns - 1;
		}
		break;
	case VK_HOME:
		row = 0;
		if (column == -1)
			column = 0;
		break;
	case VK_END:
		row = rowCount - 1;
		if (column == -1)
			column = 0;
		break;
	case VK_PRIOR:
		if (row == -1) {
			row = 0;
			column = 0;
		} else {
			row = firstVisible(t);
			if (row == t->selectedRow)
				// TODO investigate why the - 1 is needed here and below
				// TODO if this is a misunderstanding of how t->vpagesize works, figure out what happens if there is no partially visible row, and what is supposed to happen
				row -= t->vpagesize - 1;
			if (row < 0)
				row = 0;
		}
		break;
	case VK_NEXT:
		if (row == -1) {
			row = firstVisible(t) + t->vpagesize - 1;
			// TODO ensusre this is the case with the real list view
			if (row >= rowCount)
				row = rowCount - 1;
			column = 0;
		} else {
			row = firstVisible(t) + t->vpagesize - 1;
			if (row == t->selectedRow)
				row += t->vpagesize - 1;
			if (row >= rowCount)
				row = rowCount - 1;
		}
		break;
	default:
		return FALSE;
	}
	doselect(t, m, row, column);
	return TRUE;
}
