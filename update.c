// 8 january 2015
#include "tablepriv.h"

// Whenever a number of things in the Table changes, the update() function needs to be called to update any metrics and scrolling positions.
// The control font changing is the big one, as that comes with a flag that decides whether or not to redraw everything. We'll need to respect that here.

// For my personal convenience, each invocation of update() and updateAll() will be suffixed with a DONE comment once I have reasoned that the chosen function is correct and that no additional redrawing is necessary.

// TODO actually use redraw here
HRESULT update(struct table *t, BOOL redraw)
{
	struct metrics m;
	intmax_t i;
	intmax_t height;
	HRESULT hr;
	LONG width;

	// before we do anything we need the various metrics
	hr = metrics(t, &m);
	if (hr != S_OK)
		return hr;

	// the first step is to figure out how wide the whole table is
	// TODO count dividers?
	t->width = 0;
	for (i = 0; i < t->nColumns; i++) {
		hr = columnWidth(t, i, &width);
		if (hr != S_OK)
			return hr;
		t->width += width;
	}

	// now we need to figure out how much of the width of the table can be seen at once
	t->hpagesize = m.client.right - m.client.left;
	// this part is critical: if we resize the columns to less than the client area width, then the following hscrollby() will make t->hscrollpos negative, which does very bad things
	// we do this regardless of which of the two has changed, just to be safe
	if (t->hpagesize > t->width)
		t->hpagesize = t->width;

	// now we do a dummy horizontal scroll to apply the new width and horizontal page size
	// this will also reposition and resize the header (the latter in case the font changed), which will be important for the next step
	hr = hscrollby(t, 0);
	if (hr != S_OK)
		return hr;

	// now that we have the new height of the header, we can fix up vertical scrolling
	// so let's take the header height away from the client area
	m.client.top += t->headerHeight;
	// and update our page size appropriately
	height = m.client.bottom - m.client.top;
	t->vpagesize = height / m.rowHeight;
	// and do a dummy vertical scroll to apply that
	hr = vscrollby(t, 0);
	if (hr != S_OK)
		return hr;

	return S_OK;
}

// this is the same as update(), but also redraws /everything/
// as such, redraw is TRUE
HRESULT updateAll(struct table *t)
{
	HRESULT hr;

	hr = update(t, TRUE);
	if (hr != S_OK)
		return hr;
	if (InvalidateRect(t->hwnd, NULL, TRUE) == 0)
		return logLastError("error queueing all of Table for redraw in updateAll()");
	return S_OK;
}
