// 8 december 2014
#include "tablepriv.h"

// TODO move to api.h? definitely move somewhere
// TODO migrate
static WCHAR *getCellText(struct table *t, intmax_t row, intmax_t column)
{
	return (WCHAR *) notify(t, tableNotificationGetCellData, row, column, 0);
}
static void returnCellData(struct table *t, intmax_t row, intmax_t column, void *what)
{
	notify(t, tableNotificationFinishedWithCellData, row, column, (uintptr_t) what);
}
static int isCheckboxChecked(struct table *t, intmax_t row, intmax_t column)
{
	return notify(t, tableNotificationGetCellData, row, column, 0) != 0;
}

struct drawCellParams {
	intmax_t row;
	intmax_t column;
	LONG x;
	LONG y;
	LONG width;		// of column
	LONG height;		// rowHeight()
	LRESULT xoff;		// result of HDM_GETBITMAPMARGIN
};

static HRESULT drawTextCell(struct table *t, HDC dc, struct drawCellParams *p, RECT *r, int textColor)
{
	WCHAR *text;

	toCellContentRect(t, r, p->xoff, 0, 0);		// TODO get the text height
	if (SetTextColor(dc, GetSysColor(textColor)) == CLR_INVALID)
		return logLastError("error setting Table cell text color");
	if (SetBkMode(dc, TRANSPARENT) == 0)
		return logLastError("error setting transparent text drawing mode for Table cell");
	text = getCellText(t, p->row, p->column);
	if (DrawTextExW(dc, text, -1, r, DT_END_ELLIPSIS | DT_LEFT | DT_NOPREFIX | DT_SINGLELINE, NULL) == 0)
		return logLastError("error drawing Table cell text");
	returnCellData(t, p->row, p->column, text);
	return S_OK;
}

// TODO really panic on cleanup failure?
static HRESULT drawImageCell(struct table *t, HDC dc, struct drawCellParams *p, RECT *r)
{
	HBITMAP bitmap;
	BITMAP bi;
	HDC idc;
	HBITMAP previbitmap;
	BLENDFUNCTION bf;

	// only call tableImageWidth() and tableImageHeight() here in case it changes partway through
	// we can get the values back out with basic subtraction (r->right - r->left/r->bottom - r->top)
	toCellContentRect(t, r, p->xoff, tableImageWidth(), tableImageHeight());

	bitmap = (HBITMAP) notify(t, tableNotificationGetCellData, p->row, p->column, 0);
	ZeroMemory(&bi, sizeof (BITMAP));
	if (GetObject(bitmap, sizeof (BITMAP), &bi) == 0)
		return logLastError("error getting Table cell image dimensions for drawing");
	// is it even possible to enforce the type of bitmap we need here based on the contents of the BITMAP (or even the DIBSECTION) structs?

	idc = CreateCompatibleDC(dc);
	if (idc == NULL)
		return logLastError("error creating compatible DC for Table image cell drawing");
	previbitmap = SelectObject(idc, bitmap);
	if (previbitmap == NULL)
		return logLastError("error selecting Table cell image into compatible DC for image drawing");

	ZeroMemory(&bf, sizeof (BLENDFUNCTION));
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;			// per-pixel alpha values
	bf.AlphaFormat = AC_SRC_ALPHA;
	if (AlphaBlend(dc, r->left, r->top, r->right - r->left, r->bottom - r->top,
		idc, 0, 0, bi.bmWidth, bi.bmHeight, bf) == FALSE)
		return logLastError("error drawing image into Table cell");

	if (SelectObject(idc, previbitmap) != bitmap)
		return logLastError("error deselecting Table cell image for drawing image");
	if (DeleteDC(idc) == 0)
		return logLastError("error deleting Table compatible DC for image cell drawing");

	returnCellData(t, p->row, p->column, bitmap);
	return S_OK;
}

/*
TODO
static HRESULT drawCheckboxCell(struct table *t, HDC dc, struct drawCellParams *p, RECT *r)
{
	POINT pt;
	int cbState;

	toCheckboxRect(t, r, p->xoff);
	cbState = 0;
	if (isCheckboxChecked(t, p->row, p->column))
		cbState |= checkboxStateChecked;
	if (t->checkboxMouseDown)
		if (p->row == t->checkboxMouseDownRow && p->column == t->checkboxMouseDownColumn)
			cbState |= checkboxStatePushed;
	if (t->checkboxMouseOverLast) {
		pt.x = GET_X_LPARAM(t->checkboxMouseOverLastPoint);
		pt.y = GET_Y_LPARAM(t->checkboxMouseOverLastPoint);
		if (PtInRect(r, pt) != 0)
			cbState |= checkboxStateHot;
	}
	return drawCheckbox(t, dc, r, cbState);
}
*/

// TODO really abort on error?
static HRESULT drawCell(struct table *t, HDC dc, struct drawCellParams *p)
{
	RECT r;
	HBRUSH background;
	int textColor;
	RECT cellrect;
	HRESULT hr;

	// TODO verify these two
	background = (HBRUSH) (COLOR_WINDOW + 1);
	textColor = COLOR_WINDOWTEXT;
	if (t->selectedRow == p->row) {
		// these are the colors wine uses (http://source.winehq.org/source/dlls/comctl32/listview.c)
		// the two for unfocused are also suggested by http://stackoverflow.com/questions/10428710/windows-forms-inactive-highlight-color
		background = (HBRUSH) (COLOR_HIGHLIGHT + 1);
		textColor = COLOR_HIGHLIGHTTEXT;
		if (GetFocus() != t->hwnd) {
			background = (HBRUSH) (COLOR_BTNFACE + 1);
			textColor = COLOR_BTNTEXT;
		}
		// TODO disabled
	}

	r.left = p->x;
	r.right = p->x + p->width;
	r.top = p->y;
	r.bottom = p->y + p->height;
	if (FillRect(dc, &r, background) == 0)
		return logLastError("error filling Table cell background");
	cellrect = r;		// save for drawing the focus rect

	switch (t->columnTypes[p->column]) {
	case tableColumnText:
		hr = drawTextCell(t, dc, p, &r, textColor);
		if (hr != S_OK)
			return hr;
		break;
	case tableColumnImage:
		hr = drawImageCell(t, dc, p, &r);
		if (hr != S_OK)
			return hr;
		break;
/*TODO	case tableColumnCheckbox:
		hr = drawCheckboxCell(t, dc, p, &r);
		if (hr != S_OK)
			return hr;
		break;
*/	}

	// TODO in front of or behind the cell contents?
	if (t->selectedRow == p->row && t->selectedColumn == p->column)
		if (DrawFocusRect(dc, &cellrect) == 0)
			return logLastError("error drawing focus rect on current Table cell");

	return S_OK;
}

// TODO use cliprect
// TODO abort whole drawing if one cell fails?
static HRESULT draw(struct table *t, HDC dc, RECT cliprect, RECT client)
{
	intmax_t i, j;
	HFONT prevfont, newfont;
	struct drawCellParams p;
	HRESULT hr;
	intmax_t startRow, endRow;
	POINT prevOrigin;

	hr = selectFont(t, dc, &newfont, &prevfont);
	if (hr != S_OK)
		return hr;

	client.top += t->headerHeight;

	ZeroMemory(&p, sizeof (struct drawCellParams));
	hr = rowHeight(t, dc, FALSE, &(p.height));
	if (hr != S_OK)
		return hr;
	p.xoff = SendMessageW(t->header, HDM_GETBITMAPMARGIN, 0, 0);

	// see http://blogs.msdn.com/b/oldnewthing/archive/2003/07/31/54601.aspx
	if (OffsetRect(&cliprect, t->xOrigin, t->yOrigin * p.height) == 0)
		return logLastError("error adjusting cliprect to Table scroll origin in draw()");
	if (GetWindowOrgEx(dc, &prevOrigin) == 0)
		return logLastError("error saving previous Table DC origin in draw()");
	if (SetWindowOrgEx(dc, prevOrigin.x + t->xOrigin,
		prevOrigin.y + (t->yOrigin * p.height), NULL) == 0)
		return logLastError("error setting Table DC origin to account for Table scroll origin in draw()");

	// see http://blogs.msdn.com/b/oldnewthing/archive/2003/07/29/54591.aspx
	startRow = cliprect.top / p.height;
	if (startRow < 0)
		startRow = 0;
	endRow = (cliprect.bottom + p.height - 1) / p.height;
	if (endRow > t->count - 1)
		endRow = t->count - 1;

	for (i = startRow; i < endRow; i++) {
		p.row = i;
		p.y = i * p.height;
		// TODO migrate
		p.x = client.left - t->xOrigin;
		for (j = 0; j < t->nColumns; j++) {
			p.column = j;
			hr = columnWidth(t, p.column, &(p.width));
			if (hr != S_OK)
				return hr;
			hr = drawCell(t, dc, &p);
			if (hr != S_OK)
				return hr;
			p.x += p.width;
		}
		p.y += p.height;
	}

	if (SetWindowOrgEx(dc, prevOrigin.x, prevOrigin.y, NULL) == 0)
		return logLastError("error resetting Table DC after finishing drawing in draw()");
	hr = deselectFont(dc, prevfont, newfont);
	if (hr != S_OK)
		return hr;
	return S_OK;
}

// TODO what to do if any function fails?
// TODO copy/paste approrpiate comments from hscroll.c
HANDLER(drawHandlers)
{
	HDC dc;
	PAINTSTRUCT ps;
	RECT client;
	RECT r;

	if (uMsg != WM_PAINT && uMsg != WM_PRINTCLIENT)
		return FALSE;
	// TODO fail here
	if (GetClientRect(t->hwnd, &client) == 0)
		logLastError("error getting client rect for Table painting");
	if (uMsg == WM_PAINT) {
		dc = BeginPaint(t->hwnd, &ps);
		if (dc == NULL)
			logLastError("error beginning Table painting");
		r = ps.rcPaint;
	} else {
		dc = (HDC) wParam;
		r = client;
	}
	draw(t, dc, r, client);
	if (uMsg == WM_PAINT)
		EndPaint(t->hwnd, &ps);
	// this is correct for WM_PRINTCLIENT; see http://stackoverflow.com/a/27362258/3408572
	*lResult = 0;
	return TRUE;
}

// TODO redraw selected row on focus change
// TODO here or in select.h?
