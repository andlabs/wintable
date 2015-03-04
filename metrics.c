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

// TODO add firstVisible?
// TODO really return an error in the freeing process?
HRESULT getMetrics(struct table *t, HDC dc, BOOL select, struct metrics *m)
{
	BOOL release;
	HFONT prevfont, newfont;
	TEXTMETRICW tm;
	HRESULT hr;

	if (GetClientRect(t->hwnd, &(m->client)) == 0)
		return logLastError("error getting Table client rect in getMetrics()");
	// don't adjust by t->headerHeight; update() needs to do that itself

	m->imageWidth = GetSystemMetrics(SM_CXSMICON);
	m->imageHeight = GetSystemMetrics(SM_CYSMICON);

	release = FALSE;
	if (dc == NULL) {
		dc = GetDC(t->hwnd);
		if (dc == NULL)
			return logLastError("error getting Table DC in getMetrics()");
		release = TRUE;
	}
	if (select) {
		hr = selectFont(t, dc, &newfont, &prevfont);
		if (hr != S_OK)
			return hr;
	}

	if (GetTextMetricsW(dc, &tm) == 0)
		return logLastError("error getting text metrics in getMetrics()");
	m->textHeight = tm.tmHeight;

	// compute checkbox size here since GetThemePartSize() requires an HDC
	// (we could assume that it doesn't need to select a font in, but let's play by its rules just to be safe)
	hr = getCheckboxSize(t, dc, &(m->checkboxWidth), &(m->checkboxHeight));
	if (hr != S_OK)
		return hr;

	if (select) {
		hr = deselectFont(dc, prevfont, newfont);
		if (hr != S_OK)
			return hr;
	}
	if (release)
		if (ReleaseDC(t->hwnd, dc) == 0)
			return logLastError("error releasing Table DC in getMetrics()");

	// TODO omit column types that are not present?
	m->rowHeight = m->textHeight;
	if (m->rowHeight < m->imageHeight)
		m->rowHeight = m->imageHeight;
	if (m->rowHeight < m->checkboxHeight)
		m->rowHeight = m->checkboxHeight;
	return S_OK;
}
