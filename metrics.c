// 4 december 2014
#include "tablepriv.h"

HRESULT columnWidth(struct table *t, intmax_t n, LONG *width)
{
	RECT r;

	if (SendMessageW(t->header, HDM_GETITEMRECT, (WPARAM) n, (LPARAM) (&r)) == 0)
		return logLastError("error getting Table column width");
	*width = r.right - r.left;
	return S_OK;
}

// TODO really return an error in the freeing process?
HRESULT textHeight(struct table *t, HDC dc, BOOL select, LONG *height)
{
	BOOL release;
	HFONT prevfont, newfont;
	TEXTMETRICW tm;
	HRESULT hr;

	release = FALSE;
	if (dc == NULL) {
		dc = GetDC(t->hwnd);
		if (dc == NULL)
			return logLastError("error getting Table DC for rowHeight()");
		release = TRUE;
	}
	if (select) {
		hr = selectFont(t, dc, &newfont, &prevfont);
		if (hr != S_OK)
			return hr;
	}
	if (GetTextMetricsW(dc, &tm) == 0)
		return logLastError("error getting text metrics for rowHeight()");
	if (select) {
		hr = deselectFont(dc, prevfont, newfont);
		if (hr != S_OK)
			return hr;
	}
	if (release)
		if (ReleaseDC(t->hwnd, dc) == 0)
			return logLastError("error releasing Table DC for rowHeight()");
	*height = tm.tmHeight;
	return S_OK;
}

// TODO omit column types that are not present?
// TODO compute checkbox height here?
HRESULT rowHeight(struct table *t, HDC dc, BOOL select, LONG *height)
{
	LONG tmHeight;
	HRESULT hr;

	// do this first to avoid overwriting height before an error case
	hr = textHeight(t, dc, select, &tmHeight);
	if (hr != 0)
		return hr;
	*height = tableImageHeight();		// start with this to avoid two function calls
	if (*height < tmHeight)
		*height = tmHeight;
	if (*height < t->checkboxHeight)
		*height = t->checkboxHeight;
	return S_OK;
}
