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
BOOL runEventHandlers(const handlerfunc list[], struct table *t, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult)
{
	const handlerfunc *p;
	BOOL handled = FALSE;

	// TODO make sure lResult is consistent
	for (p = list; *p != NULL; p++)
		if ((*(*p))(t, uMsg, wParam, lParam, lResult))
			handled = TRUE;
	return handled;
}

// font selection

DWORD selectFont(struct table *t, HDC dc, HFONT *newfont, HFONT *prevfont)
{
	// copy it in casse we get a WM_SETFONT before this call's respective deselectFont() call
	*newfont = t->font;
	if (*newfont == NULL) {
		// get it on demand in the (unlikely) event it changes while this Table is alive
		*newfont = GetStockObject(SYSTEM_FONT);
		if (*newfont == NULL)
			return panicLastError("error getting default font for selecting into Table DC");
	}
	*prevfont = (HFONT) SelectObject(dc, *newfont);
	if (*prevfont == NULL)
		return panicLastError("error selecting Table font into Table DC");
	return 0;
}

DWORD deselectFont(HDC dc, HFONT prevfont, HFONT newfont)
{
	if (SelectObject(dc, prevfont) != newfont)
		return panicLastError("error deselecting Table font from Table DC");
	// doin't delete newfont here, even if it is the system font (see http://msdn.microsoft.com/en-us/library/windows/desktop/dd144925%28v=vs.85%29.aspx)
	return 0;
}
