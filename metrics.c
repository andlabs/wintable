// 4 december 2014

DWORD columnWidth(struct table *t, intmax_t n, LONG *width)
{
	RECT r;

	if (SendMessageW(t->header, HDM_GETITEMRECT, (WPARAM) n, (LPARAM) (&r)) == 0)
		return panicLastError("error getting Table column width");
	*width = r.right - r.left;
	return 0;
}

// TODO really return an error in the freeing process?
DWORD textHeight(struct table *t, HDC dc, BOOL select, LONG *height)
{
	BOOL release;
	HFONT prevfont, newfont;
	TEXTMETRICW tm;
	DWORD le;

	release = FALSE;
	if (dc == NULL) {
		dc = GetDC(t->hwnd);
		if (dc == NULL)
			return panicLastError("error getting Table DC for rowHeight()");
		release = TRUE;
	}
	if (select) {
		le = selectFont(t, dc, &newfont, &prevfont);
		if (le != 0)
			return le;
	}
	if (GetTextMetricsW(dc, &tm) == 0)
		return panicLastError("error getting text metrics for rowHeight()");
	if (select) {
		le = deselectFont(dc, prevfont, newfont);
		if (le != 0)
			return le;
	}
	if (release)
		if (ReleaseDC(t->hwnd, dc) == 0)
			return panicLastError("error releasing Table DC for rowHeight()");
	*height = tm.tmHeight;
	return 0;
}

// TODO move to tablepriv.h
#define tableImageWidth() GetSystemMetrics(SM_CXSMICON)
#define tableImageHeight() GetSystemMetrics(SM_CYSMICON)

// TODO omit column types that are not present?
DWORD rowHeight(struct table *t, HDC dc, BOOL select, LONG *height)
{
	LONG tmHeight;
	DWORD le;

	// do this first to avoid overwriting height before an error case
	le = textHeight(t, dc, select, &tmHeight);
	if (le != 0)
		return le;
	*height = tableImageHeight();		// start with this to avoid two function calls
	if (*height < tmHeight)
		*height = tmHeight;
	if (*height < t->checkboxHeight)
		*height = t->checkboxHeight;
	return 0;
}

// TODO move to tablepriv.h
#define rowht(t, p) rowHeight(t, NULL, TRUE, p)
