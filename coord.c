// 4 december 2014
#include "tablepriv.h"

/*
TODO MIGRATE

// TODO these functions rely on t->firstVisible being > 0; this causes problems for our drawing and scrolling logic, so that will have to change...

// TODO call erroring functions before returning anything else?
DWORD clientCoordToRowColumn(struct table *t, POINT pt, struct rowcol *rc)
{
	RECT r;
	intmax_t i;
	LONG height;
	DWORD le;
	LONG cwid;

	if (GetClientRect(t->hwnd, &r) == 0)
		return panicLastError("error getting Table client rect in clientCoordToRowColumn()");
	r.top += t->headerHeight;
	if (PtInRect(&r, pt) == 0)
		goto outside;

	// the row is easy
	le = rowht(t, &height);
	if (le != 0)
		return le;
	pt.y -= t->headerHeight;
	rc->row = (pt.y / height) + t->vscrollpos;
	if (rc->row >= t->count)
		goto outside;

	// the column... not so much
	// we scroll p.x, then subtract column widths until we cross the left edge of the control
	pt.x += t->hscrollpos;
	rc->column = 0;
	for (i = 0; i < t->nColumns; i++) {
		le = columnWidth(t, i, &cwid);
		if (le != 0)
			return le;
		pt.x -= cwid;
		// use <, not <=, here:
		// assume r.left and t->hscrollpos == 0;
		// given the first column is 100 wide,
		// pt.x == 0 (first pixel of col 0) -> p.x - 100 == -100 < 0 -> break
		// pt.x == 99 (last pixel of col 0) -> p.x - 100 == -1 < 0 -> break
		// pt.x == 100 (first pixel of col 1) -> p.x - 100 == 0 >= 0 -> next column
		if (pt.x < r.left)
			break;
		rc->column++;
	}
	if (rc->column >= t->nColumns)
		goto outside;

	return 0;

outside:
	rc->row = -1;
	rc->column = -1;
	return 0;
}

// same as client coordinates, but stored in a lParam (like the various mouse messages provide)
DWORD lParamToRowColumn(struct table *t, LPARAM lParam, struct rowcol *rc)
{
	POINT pt;

	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	return clientCoordToRowColumn(t, pt, rc);
}

// returns TRUE if the row is visible (even partially visible) and thus has a rectangle in the client area; FALSE otherwise
// TODO provide a way to intersect with the visible client rect area
// TODO call erroring functions before returning anything else?
DWORD rowColumnToClientRect(struct table *t, struct rowcol rc, RECT *r, BOOL *visible)
{
	RECT client;
	RECT out;			// don't change r if we return FALSE
	LONG height;
	intmax_t xpos;
	intmax_t i;
	DWORD le;
	LONG cwid;

	if (rc.row < t->vscrollpos)
		goto invisible;
	rc.row -= t->vscrollpos;		// align with client.top

	if (GetClientRect(t->hwnd, &client) == 0)
		return panicLastError("error getting Table client rect in rowColumnToClientRect()");
	client.top += t->headerHeight;

	le = rowht(t, &height);
	if (le != 0)
		return le;
	out.top = client.top + (rc.row * height);
	if (out.top >= client.bottom)			// >= because RECT.bottom is the first pixel outside the rectangle
		goto invisible;
	out.bottom = out.top + height;

	// and again the columns are the hard part
	// so we start with client.left - t->hscrollpos, then keep adding widths until we get to the column we want
	xpos = client.left - t->hscrollpos;
	for (i = 0; i < rc.column; i++) {
		le = columnWidth(t, i, &cwid);
		if (le != 0)
			return le;
		xpos += cwid;
	}
	// did we stray too far to the right? if so it's not visible
	if (xpos >= client.right)		// >= because RECT.right is the first pixel outside the rectangle
		goto invisible;
	out.left = xpos;
	le = columnWidth(t, rc.column, &cwid);
	if (le != 0)
		return le;
	out.right = xpos + cwid;
	// and is this too far to the left?
	if (out.right < client.left)		// < because RECT.left is the first pixel inside the rect
		goto invisible;

	*r = out;
	*visible = TRUE;
	return 0;

invisible:
	*visible = FALSE;
	return 0;
}

// TODO idealCoordToRowColumn/rowColumnToIdealCoord?

*/

void toCellContentRect(struct table *t, RECT *r, LRESULT xoff, intmax_t width, intmax_t height)
{
	if (xoff == 0)
		xoff = SendMessageW(t->header, HDM_GETBITMAPMARGIN, 0, 0);
	r->left += xoff;
	if (width != 0)
		r->right = r->left + width;
	if (height != 0)
		// TODO vertical center
		r->bottom = r->top + height;
}
