// 3 march 2015
#include "tablepriv.h"

// as it turns out, t->yOrigin IS the first visible row; this is a good thing
// this is because t->yOrigin cannot be negative after all; see scroll.c

intmax_t firstVisible(struct table *t)
{
	return t->yOrigin;
}

HRESULT ensureVisible(struct table *t, struct metrics *m, struct rowcol rc)
{
	RECT r;
	BOOL dovscroll;
	intmax_t xpos;
	LONG clientWidth;
	LONG cwid;
	intmax_t i;
	HRESULT hr;

	// first vertically scroll to the selected row to make it fully visible (or as visible as possible)
	if (rc.row < t->yOrigin) {
		hr = vscrollto(t, rc.row);
		if (hr != 0)
			return hr;
	} else {
		// first assume entirely outside the client area
		dovscroll = TRUE;
		hr = rowColumnToClientRect(t, m, rc, &r);
		if (hr != S_OK && hr != S_FALSE)
			return hr;
		if (hr == S_OK)
			// partially outside the client area?
			// <= here since we are comparing bottoms (which are the first pixels outside the rectangle)
			if (r.bottom <= m->client.bottom)
				dovscroll = FALSE;
		if (dovscroll) {
			// + 1 because apparently just rc.row - t->vpagesize results in no scrolling (rc.row - t->vpagesize == t->vscrollpos)...
			hr = vscrollto(t, rc.row - t->vpagesize + 1);
			if (hr != S_OK)
				return hr;
		}
	}

	// TODO rewrite this comment
	// now see if the cell we want is to the left of offscreen, in which case scroll to its x-position
	xpos = 0;
	for (i = 0; i < rc.column; i++) {
		hr = columnWidth(t, i, &cwid);
		if (hr != 0)
			return hr;
		xpos += cwid;
	}
	if (xpos < t->xOrigin) {
		hr = hscrollto(t, xpos);
		if (hr != 0)
			return hr;
	} else {
		// if the full cell is not visible, scroll to the right just enough to make it fully visible (or as visible as possible)
		hr = columnWidth(t, t->selectedColumn, &cwid);
		if (hr != 0)
			return hr;
		clientWidth = m->client.right - m->client.left;
		if (xpos + cwid > t->xOrigin + clientWidth)			// > because both sides deal with the first pixel outside
			// if the column is too wide, then just make it occupy the whole visible area (left-aligned)
			if (cwid > clientWidth) {			// TODO >= ?
				hr = hscrollto(t, xpos);
				if (hr != 0)
					return hr;
			} else {
				// TODO don't use t->hpagesize here? depends if other code uses it
				hr = hscrollto(t, (xpos + cwid) - t->hpagesize);
				if (hr != 0)
					return hr;
			}
	}

	return S_OK;
}

// TODO keep here?
HRESULT queueRedrawRow(struct table *t, struct metrics *m, intmax_t row)
{
	RECT r;

	if (row < t->yOrigin)		// too high
		return S_OK;
	// TODO check too low
	row -= t->yOrigin;
	r.top = row * m->rowHeight + t->headerHeight;
	r.bottom = r.top + m->rowHeight;
	r.left = m->client.left;
	r.right = m->client.right;
	if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
		return logLastError("error queueing Table row for redraw in queueRowRedraw()");
	return S_OK;
}
