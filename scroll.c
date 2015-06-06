// 9 december 2014
#include "tablepriv.h"

HRESULT scrollto(struct table *t, int which, struct scrollParams *p, intmax_t pos)
{
	RECT scrollArea;
	SCROLLINFO si;
	intmax_t xamount, yamount;
	HRESULT hr;

	// note that the pos < 0 check is /after/ the p->length - p->pagesize check
	// it used to be /before/; this was actually a bug in Raymond Chen's original algorithm: if there are fewer than a page's worth of items, p->length - p->pagesize will be negative and our items draw at the bottom of the window
	// this SHOULD have the same effect with that bug fixed and no others introduced... (thanks to devin on irc.badnik.net for confirming this logic)
	// TODO further confirmation would also be good
	if (pos > p->length - p->pagesize)
		pos = p->length - p->pagesize;
	if (pos < 0)
		pos = 0;

	// we don't want to scroll the header
	if (GetClientRect(t->hwnd, &scrollArea) == 0)
		return logLastError("error getting Table client rect for scrollto()");
	scrollArea.top += t->headerHeight;

	// negative because ScrollWindowEx() is "backwards"
	xamount = -(pos - *(p->pos)) * p->scale;
	yamount = 0;
	if (which == SB_VERT) {
		yamount = xamount;
		xamount = 0;
	}

	if (ScrollWindowEx(t->hwnd, xamount, yamount,
		&scrollArea, &scrollArea, NULL, NULL,
		SW_ERASE | SW_INVALIDATE) == ERROR)
		return logLastError("error scrolling Table");

	*(p->pos) = pos;

	// now commit our new scrollbar setup...
	ZeroMemory(&si, sizeof (SCROLLINFO));
	si.cbSize = sizeof (SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nPage = p->pagesize;
	si.nMin = 0;
	si.nMax = p->length - 1;		// endpoint inclusive
	si.nPos = *(p->pos);
	SetScrollInfo(t->hwnd, which, &si, TRUE);

	if (p->post != NULL) {
		hr = (*(p->post))(t);
		if (hr != S_OK)
			return hr;
	}

	// always pop tooltips on scroll, regardless of what caused it
	// TODO pop at the beginning?
	// TODO pop on horizontal scroll?
/*TODO	hr = popTooltip(t, TRUE);
	if (hr != S_OK)
		return hr;

*/	return S_OK;
}

HRESULT scrollby(struct table *t, int which, struct scrollParams *p, intmax_t delta)
{
	return scrollto(t, which, p, *(p->pos) + delta);
}

HRESULT scroll(struct table *t, int which, struct scrollParams *p, WPARAM wParam, LPARAM lParam)
{
	intmax_t pos;
	SCROLLINFO si;

	pos = *(p->pos);
	switch (LOWORD(wParam)) {
	case SB_LEFT:			// also SB_TOP
		pos = 0;
		break;
	case SB_RIGHT:		// also SB_BOTTOM
		pos = p->length - p->pagesize;
		break;
	case SB_LINELEFT:		// also SB_LINEUP
		pos--;
		break;
	case SB_LINERIGHT:		// also SB_LINEDOWN
		pos++;
		break;
	case SB_PAGELEFT:		// also SB_PAGEUP
		pos -= p->pagesize;
		break;
	case SB_PAGERIGHT:	// also SB_PAGEDOWN
		pos += p->pagesize;
		break;
	case SB_THUMBPOSITION:
		ZeroMemory(&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);
		si.fMask = SIF_POS;
		if (GetScrollInfo(t->hwnd, which, &si) == 0)
			return logLastError("error getting thumb position for scroll() in Table");
		pos = si.nPos;
		break;
	case SB_THUMBTRACK:
		ZeroMemory(&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		if (GetScrollInfo(t->hwnd, which, &si) == 0)
			return logLastError("error getting thumb track position for scroll() in Table");
		pos = si.nTrackPos;
		break;
	}
	return scrollto(t, which, p, pos);
}

HRESULT wheelscroll(struct table *t, int which, struct scrollParams *p, WPARAM wParam, LPARAM lParam)
{
	int delta;
	int lines;
	UINT scrollAmount;

	delta = GET_WHEEL_DELTA_WPARAM(wParam);
	// TODO make a note of what the appropriate hscroll constant is
	if (SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &scrollAmount, 0) == 0)
		// TODO use scrollAmount == 3 instead?
		return logLastError("error getting wheel scroll amount in wheelscroll()");
	if (scrollAmount == WHEEL_PAGESCROLL)
		scrollAmount = p->pagesize;
	if (scrollAmount == 0)		// no mouse wheel scrolling (or t->pagesize == 0)
		return S_OK;
	// the rest of this is basically http://blogs.msdn.com/b/oldnewthing/archive/2003/08/07/54615.aspx and http://blogs.msdn.com/b/oldnewthing/archive/2003/08/11/54624.aspx
	// see those pages for information on subtleties
	delta += *(p->wheelCarry);
	lines = delta * ((int) scrollAmount) / WHEEL_DELTA;
	*(p->wheelCarry) = delta - lines * WHEEL_DELTA / ((int) scrollAmount);
	return scrollby(t, which, p, -lines);
}
