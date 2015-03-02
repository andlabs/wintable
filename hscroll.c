// 9 december 2014
#include "tablepriv.h"

static struct scrollParams hscrollParams(struct table *t)
{
	struct scrollParams p;

	ZeroMemory(&p, sizeof (struct scrollParams));
	p.pos = &(t->xOrigin);
	p.pagesize = t->hpagesize;
	p.length = t->width;
	p.scale = 1;
	p.post = repositionHeader;
	p.wheelCarry = &(t->hwheelCarry);
	return p;
}

HRESULT hscrollto(struct table *t, intmax_t pos)
{
	struct scrollParams p;

	p = hscrollParams(t);
	return scrollto(t, SB_HORZ, &p, pos);
}

HRESULT hscrollby(struct table *t, intmax_t delta)
{
	struct scrollParams p;

	p = hscrollParams(t);
	return scrollby(t, SB_HORZ, &p, delta);
}

HRESULT hscroll(struct table *t, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	p = hscrollParams(t);
	return scroll(t, SB_HORZ, &p, wParam, lParam);
}

// TODO find out if we can indicriminately check for WM_WHEELHSCROLL
HANDLER(hscrollHandler)
{
	if (uMsg != WM_HSCROLL)
		return FALSE;
	// ignore failure; there's not much we can do
	// TODO defer to DefWindowProc()?
	hscroll(t, wParam, lParam);
	*lResult = 0;
	return TRUE;
}
