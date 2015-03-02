// 9 december 2014
#include "tablepriv.h"

static HRESULT vscrollParams(struct table *t, struct scrollParams *p)
{
	LONG rh;
	HRESULT hr;

	ZeroMemory(p, sizeof (struct scrollParams));
	p->pos = &(t->yOrigin);
	p->pagesize = t->vpagesize;
	p->length = t->count;
	hr = rowht(t, &rh);
	if (hr != S_OK)
		return hr;
	p->scale = rh;
	p->post = NULL;
	p->wheelCarry = &(t->vwheelCarry);
	return S_OK;
}

HRESULT vscrollto(struct table *t, intmax_t pos)
{
	struct scrollParams p;
	HRESULT hr;

	hr = vscrollParams(t, &p);
	if (hr != S_OK)
		return hr;
	return scrollto(t, SB_VERT, &p, pos);
}

HRESULT vscrollby(struct table *t, intmax_t delta)
{
	struct scrollParams p;
	HRESULT hr;

	hr = vscrollParams(t, &p);
	if (hr != S_OK)
		return hr;
	return scrollby(t, SB_VERT, &p, delta);
}

HRESULT vscroll(struct table *t, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;
	HRESULT hr;

	hr = vscrollParams(t, &p);
	if (hr != S_OK)
		return hr;
	return scroll(t, SB_VERT, &p, wParam, lParam);
}

HRESULT vwheelscroll(struct table *t, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;
	HRESULT hr;

	hr = vscrollParams(t, &p);
	if (hr != S_OK)
		return hr;
	return wheelscroll(t, SB_VERT, &p, wParam, lParam);
}

HANDLER(vscrollHandler)
{
	switch (uMsg) {
	case WM_VSCROLL:
		// ignore failure; there's not much we can do
		// TODO defer to DefWindowProc()?
		vscroll(t, wParam, lParam);
		*lResult = 0;
		return TRUE;
	case WM_MOUSEWHEEL:
		// ignore failure; there's not much we can do
		// TODO defer to DefWindowProc()?
		vwheelscroll(t, wParam, lParam);
		*lResult = 0;
		return TRUE;
	}
	return FALSE;
}
