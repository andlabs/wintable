// 4 december 2014
#include "tablepriv.h"

static HRESULT doAdjustRect(struct table *t, struct metrics *m, RECT *r, intmax_t *yOffsetOut, BOOL unadjust)
{
	intmax_t xOffset;
	intmax_t yOffset;

	// TODO adjust this comment for xOffset
	// see http://blogs.msdn.com/b/oldnewthing/archive/2003/07/31/54601.aspx
	// we need to get cliprect to be in a position where (0, header height) is row 0
	// we can get row 0 at (0, 0) by moving cliprect down the number of pixels in all the rows above the current Y origin value
	// TODO explain why we subtract t->headerHeight
	xOffset = t->xOrigin;
	yOffset = t->yOrigin * m->rowHeight - t->headerHeight;
	if (unadjust) {
		xOffset = -xOffset;
		yOffset = -yOffset;
	}
	if (OffsetRect(r, xOffset, yOffset) == 0)
		return logLastError("error adjusting cliprect to Table scroll origin in doAdjustRect()");
	if (yOffsetOut != NULL)
		*yOffsetOut = yOffset;
	return S_OK;
}

// takes r and moves it to the position it would be in the scroll area
HRESULT adjustRect(struct table *t, struct metrics *m, RECT *r, intmax_t *yOffsetOut)
{
	return doAdjustRect(t, m, r, yOffsetOut, FALSE);
}

// takes r, which should have been adjusted by adjustRect(), and undoes the adjustment
HRESULT unadjustRect(struct table *t, struct metrics *m, RECT *r)
{
	return doAdjustRect(t, m, r, NULL, TRUE);
}

HRESULT adjustPoint(struct table *t, struct metrics *m, POINT *pt)
{
	RECT r;
	HRESULT hr;

	r.left = pt->x;
	r.top = pt->y;
	r.right = pt->x + 2;		// arbitrary number to make a non-empty rect; these fields will be ignored
	r.bottom = pt->y + 2;
	hr = adjustRect(t, m, &r, NULL);
	if (hr != S_OK)
		return hr;
	pt->x = r.left;
	pt->y = r.top;
	return S_OK;
}

HRESULT clientCoordToRowColumn(struct table *t, struct metrics *m, POINT pt, struct rowcol *rc)
{
	RECT client;		// don't change m
	intmax_t i;
	HRESULT hr;
	LONG cwid;

	client = m->client;
	hr = adjustRect(t, m, &client, NULL);
	if (hr != S_OK)
		return hr;
	hr = adjustPoint(t, m, &pt);
	if (hr != S_OK)
		return hr;
	if (PtInRect(&client, pt) == 0)
		goto outside;

	// the row is easy
	rc->row = pt.y / m->rowHeight;
	if (rc->row >= tableModel_tableRowCount(t->model))
		goto outside;

	// the column... still easy, just tedious
	rc->column = 0;
	for (i = 0; i < t->nColumns; i++) {
		hr = columnWidth(t, i, &cwid);
		if (hr != S_OK)
			return hr;
		pt.x -= cwid;
		// use <, not <=, here:
		// given the first column is 100 wide,
		// pt.x == 0 (first pixel of col 0) -> p.x - 100 == -100 < 0 -> break
		// pt.x == 99 (last pixel of col 0) -> p.x - 100 == -1 < 0 -> break
		// pt.x == 100 (first pixel of col 1) -> p.x - 100 == 0 >= 0 -> next column
		if (pt.x < 0)
			break;
		rc->column++;
	}
	if (rc->column >= t->nColumns)
		goto outside;

	return S_OK;

outside:
	rc->row = -1;
	rc->column = -1;
	return S_OK;
}

// same as client coordinates, but stored in a lParam (like the various mouse messages provide)
HRESULT lParamToRowColumn(struct table *t, struct metrics *m, LPARAM lParam, struct rowcol *rc)
{
	POINT pt;

	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	return clientCoordToRowColumn(t, m, pt, rc);
}

// returns S_OK if the row is visible (even partially visible) and thus has a rectangle in the client area; S_FALSE otherwise
// if S_FALSE is returned, r is unchanged (TODO really?)
// TODO provide a way to intersect with the visible client rect area
HRESULT rowColumnToClientRect(struct table *t, struct metrics *m, struct rowcol rc, RECT *r)
{
	RECT client;		// don't change m
	RECT out;			// don't change r if we return S_FALSE
	RECT intersected;
	intmax_t i;
	HRESULT hr;
	LONG cwid;

	client = m->client;
	hr = adjustRect(t, m, &client, NULL);
	if (hr != S_OK)
		return hr;

	// get the whole rectangle for the cell now
	out.left = 0;
	for (i = 0; i < rc.column; i++) {
		hr = columnWidth(t, i, &cwid);
		if (hr != S_OK)
			return hr;
		out.left += cwid;
	}
	out.top = rc.row * m->rowHeight;
	hr = columnWidth(t, rc.column, &cwid);
	if (hr != S_OK)
		return hr;
	out.right = out.left + cwid;
	out.bottom = out.top + m->rowHeight;

	if (IntersectRect(&intersected, &client, &out) == 0)
		goto invisible;

	// and now we need to return to raw coordinates
	hr = unadjustRect(t, m, &out);
	if (hr != S_OK)
		return hr;
	*r = out;
	return S_OK;

invisible:
	return S_FALSE;
}

// TODO idealCoordToRowColumn/rowColumnToIdealCoord?

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

BOOL rowcolEqual(struct rowcol a, struct rowcol b)
{
	return (a.row == b.row) && (a.column == b.column);
}
