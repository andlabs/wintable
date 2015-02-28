// 9 december 2014
#include "tablepriv.h"

static struct scrollParams vscrollParams(struct table *t)
{
	struct scrollParams p;
	LONG rh;

	ZeroMemory(&p, sizeof (struct scrollParams));
	p.pos = &(t->vscrollpos);
	p.pagesize = t->vpagesize;
	p.length = t->count;
	// TODO handle error
	rowht(t, &rh);
	p.scale = rh;
	p.post = NULL;
	p.wheelCarry = &(t->vwheelCarry);
	return p;
}

DWORD vscrollto(struct table *t, intmax_t pos)
{
	struct scrollParams p;

	p = vscrollParams(t);
	return scrollto(t, SB_VERT, &p, pos);
}

DWORD vscrollby(struct table *t, intmax_t delta)
{
	struct scrollParams p;

	p = vscrollParams(t);
	return scrollby(t, SB_VERT, &p, delta);
}

DWORD vscroll(struct table *t, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	p = vscrollParams(t);
	return scroll(t, SB_VERT, &p, wParam, lParam);
}

DWORD vwheelscroll(struct table *t, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	p = vscrollParams(t);
	return wheelscroll(t, SB_VERT, &p, wParam, lParam);
}

// TODO what do we do if a function fails?
HANDLER(vscrollHandler)
{
	switch (uMsg) {
	case WM_VSCROLL:
		vscroll(t, wParam, lParam);
		*lResult = 0;
		return TRUE;
	case WM_MOUSEWHEEL:
		vwheelscroll(t, wParam, lParam);
		*lResult = 0;
		return TRUE;
	}
	return FALSE;
}
