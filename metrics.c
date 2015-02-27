// 4 december 2014

DWORD columnWidth(struct table *t, intmax_t n, LONG *width)
{
	RECT r;

	if (SendMessageW(t->header, HDM_GETITEMRECT, (WPARAM) n, (LPARAM) (&r)) == 0)
		return panicLastError("error getting Table column width");
	*width = r.right - r.left;
	return 0;
}

static LONG textHeight(struct table *t, HDC dc, BOOL select)
{
	BOOL release;
	HFONT prevfont, newfont;
	TEXTMETRICW tm;

	release = FALSE;
	if (dc == NULL) {
		dc = GetDC(t->hwnd);
		if (dc == NULL)
			panic("error getting Table DC for rowHeight()");
		release = TRUE;
	}
	if (select)
		prevfont = selectFont(t, dc, &newfont);
	if (GetTextMetricsW(dc, &tm) == 0)
		panic("error getting text metrics for rowHeight()");
	if (select)
		deselectFont(dc, prevfont, newfont);
	if (release)
		if (ReleaseDC(t->hwnd, dc) == 0)
			panic("error releasing Table DC for rowHeight()");
	return tm.tmHeight;
}

#define tableImageWidth() GetSystemMetrics(SM_CXSMICON)
#define tableImageHeight() GetSystemMetrics(SM_CYSMICON)

// TODO omit column types that are not present?
static LONG rowHeight(struct table *t, HDC dc, BOOL select)
{
	LONG height;
	LONG tmHeight;

	height = tableImageHeight();		// start with this to avoid two function calls
	tmHeight = textHeight(t, dc, select);
	if (height < tmHeight)
		height = tmHeight;
	if (height < t->checkboxHeight)
		height = t->checkboxHeight;
	return height;
}

#define rowht(t) rowHeight(t, NULL, TRUE)
