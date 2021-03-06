// 4 december 2014
#include "tablepriv.h"

BOOL runHandlers(const handlerfunc list[], struct table *t, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult)
{
	const handlerfunc *p;

	for (p = list; *p != NULL; p++)
		if ((*(*p))(t, uMsg, wParam, lParam, lResult))
			return TRUE;
	return FALSE;
}

// TODO move into events.h?
BOOL runEventHandlers(const eventhandlerfunc list[], struct table *t, struct metrics *m, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult, LRESULT returnWhat)
{
	const eventhandlerfunc *p;
	BOOL handled = FALSE;

	for (p = list; *p != NULL; p++)
		if ((*(*p))(t, m, uMsg, wParam, lParam))
			handled = TRUE;
	if (handled)
		*lResult = returnWhat;
	return handled;
}

// font selection

HRESULT selectFont(struct table *t, HDC dc, HFONT *newfont, HFONT *prevfont)
{
	// copy it in casse we get a WM_SETFONT before this call's respective deselectFont() call
	*newfont = t->font;
	if (*newfont == NULL) {
		// get it on demand in the (unlikely) event it changes while this Table is alive
		*newfont = GetStockObject(SYSTEM_FONT);
		if (*newfont == NULL)
			return logLastError("error getting default font for selecting into Table DC");
	}
	*prevfont = (HFONT) SelectObject(dc, *newfont);
	if (*prevfont == NULL)
		return logLastError("error selecting Table font into Table DC");
	return S_OK;
}

// TODO does this qualify as cleanup?
HRESULT deselectFont(HDC dc, HFONT prevfont, HFONT newfont)
{
	if (SelectObject(dc, prevfont) != newfont)
		return logLastError("error deselecting Table font from Table DC");
	// doin't delete newfont here, even if it is the system font (see http://msdn.microsoft.com/en-us/library/windows/desktop/dd144925%28v=vs.85%29.aspx)
	return S_OK;
}

// weird types here are to be parallel to the declaration of PtInRect() in Microsoft's headers
BOOL lParamInRect(const RECT *r, LPARAM lParam)
{
	POINT pt;

	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	return PtInRect(r, pt);
}

// TODO find a better place for this
HRESULT redrawCellAtLPARAM(struct table *t, struct metrics *m, LPARAM lParam)
{
	struct rowcol rc;
	RECT r;
	HRESULT hr;

	hr = lParamToRowColumn(t, m, lParam, &rc);
	if (hr != S_OK)
		return hr;
	if (rc.row == -1 || rc.column == -1)		// not in a cell
		return S_OK;
	hr = rowColumnToClientRect(t, m, rc, &r);
	if (hr != S_OK && hr != S_FALSE)
		return hr;
	if (hr == S_FALSE)					// nothing to draw
		return S_OK;
	if (InvalidateRect(t->hwnd, &r, TRUE) == 0)
		return logLastError("error redrawing Table cell in redrawCellAtLPARAM()");
	return S_OK;
}
