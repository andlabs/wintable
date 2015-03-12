// 8 december 2014
#include "tablepriv.h"

// TODO make the API return error codes directly
// TODO and adjust all the below functions to boot
// TODO migrate in general

// TODO
#define panic(...) abort()

// TODO make return an HRESULT
static void addColumn(struct table *t, WPARAM wParam, LPARAM lParam)
{
	t->nColumns++;
	t->columnTypes = (int *) tableRealloc(t->columnTypes, t->nColumns * sizeof (int));
	// TODO return failure
	if (t->columnTypes == NULL)
		logMemoryExhausted("adding the new column type to the current Table's list of column types");
	t->columnTypes[t->nColumns - 1] = (int) wParam;
	// TODO make a panicNoErrCode() or panicArg() for this
	if (t->columnTypes[t->nColumns - 1] >= nTableColumnTypes)
		panic("invalid column type passed to tableAddColumn");
	headerAddColumn(t, (WCHAR *) lParam);
	update(t, TRUE);
	// TODO only redraw the part of the client area where the new client went, if any
	// (TODO when — if — adding autoresize, figure this one out)
}

// TODO what if m is NULL?
// TODO what happens if unsubscribing fails?
// TODO should we QueryInterface() or just assume this is a tableModel?
// TODO (related to unsubscribing fails, but not entirely covered by it) what should the model be set to on failure?
static HRESULT setModel(struct table *t, tableModel *m)
{
	HRESULT hr;

	if (t->model != &nullModel) {
		hr = tableModel_tableUnsubscribe(t->model, t->hwnd);
		if (hr != S_OK)
			return logHRESULT("error unsubscribing old Table model in setModel()", hr);
		tableModel_Release(t->model);
	}
	t->model = m;
	tableModel_AddRef(t->model);
	hr = tableModel_tableSubscribe(t->model, t->hwnd);
	if (hr != S_OK) {
		tableModel_Release(t->model);
		return logHRESULT("error subscribing to new Table model in setModel()", hr);
	}
	return S_OK;
}

// TODO what happens if any of these fail?
HANDLER(apiHandlers)
{
	switch (uMsg) {
	case WM_SETFONT:
		// don't free the old font; see http://blogs.msdn.com/b/oldnewthing/archive/2008/09/12/8945692.aspx
		t->font = (HFONT) wParam;
		SendMessageW(t->header, WM_SETFONT, wParam, lParam);
		// if we redraw, we have to redraw ALL of it; after all, the font changed!
		if (LOWORD(lParam) != FALSE)
			updateAll(t);			// DONE
		else
			update(t, FALSE);		// DONE
		*lResult = 0;
		return TRUE;
	case WM_GETFONT:
		*lResult = (LRESULT) (t->font);
		return TRUE;
	case tableAddColumn:
		addColumn(t, wParam, lParam);
		*lResult = 0;
		return TRUE;
	case tableSetModel:
		*lResult = (LRESULT) setModel(t, (tableModel *) lParam);
		return TRUE;
	}
	return FALSE;
}

LRESULT notify(struct table *t, UINT code, intmax_t row, intmax_t column, uintptr_t data)
{
	tableNM nm;

	ZeroMemory(&nm, sizeof (tableNM));
	nm.nmhdr.hwndFrom = t->hwnd;
	// TODO check for error from here? 0 is a valid ID (IDCANCEL)
	nm.nmhdr.idFrom = GetDlgCtrlID(t->hwnd);
	nm.nmhdr.code = code;
	nm.row = row;
	nm.column = column;
	nm.columnType = t->columnTypes[nm.column];
	nm.data = data;
	// TODO check for error from GetParent()?
	return SendMessageW(GetParent(t->hwnd), WM_NOTIFY, (WPARAM) (nm.nmhdr.idFrom), (LPARAM) (&nm));
}
