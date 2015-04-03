// 8 december 2014
#include "tablepriv.h"

static HRESULT addColumn(struct table *t, tableColumn *tc)
{
	tableColumn *newc;
	intmax_t count;
	WCHAR *name;
	HRESULT hr;

	// store in a separate place to make an allocation failure nonlethal to the table itself
	count = t->nColumns + 1;
	newc = (tableColumn *) tableRealloc(t->columns, count * sizeof (tableColumn));
	if (newc == NULL)
		return logMemoryExhausted("adding the new column type to the current Table's list of column types");
	t->columns = newc;
	t->nColumns = count;

	// copy fields
	t->columns[t->nColumns - 1] = *tc;
	// don't keep a pointer to the name around, just to be safe
	// (especially if we will support deletion later)
	// TODO work this out more nicely, especially if we allow changing details (or even changing the name) later
	// we still need it for the headerAddColumn() call though
	name = t->columns[t->nColumns - 1].name;
	t->columns[t->nColumns - 1].name = NULL;

	hr = headerAddColumn(t, name);
	if (hr != S_OK)
		return hr;

	return update(t, TRUE);
	// TODO only redraw the part of the client area where the new client went, if any
	// (TODO when — if — adding autoresize, figure this one out)
}

// TODO what happens if unsubscribing fails?
// TODO should we QueryInterface() or just assume this is a tableModel?
// TODO (related to unsubscribing fails, but not entirely covered by it) what should the model be set to on failure?
// TODO update selection
// TODO ...actually should failure to do anything other than set the model return some sort of error value? in fact, should table unsubscribe even be allowed to fail?
static HRESULT setModel(struct table *t, tableModel *m)
{
	HRESULT hr;

	if (m == NULL)
		return E_INVALIDARG;
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
	return updateAll(t);
}

// TODO what happens if any of these fail?
HANDLER(apiHandlers)
{
	switch (uMsg) {
	case WM_SETFONT:
		// don't free the old font; see http://blogs.msdn.com/b/oldnewthing/archive/2008/09/12/8945692.aspx
		t->font = (HFONT) wParam;
		SendMessageW(t->header, WM_SETFONT, wParam, lParam);
		SendMessageW(t->tooltip, WM_SETFONT, wParam, lParam);
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
		*lResult = (LRESULT) addColumn(t, (tableColumn *) lParam);
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
	nm.data = data;
	// TODO check for error from GetParent()?
	return SendMessageW(GetParent(t->hwnd), WM_NOTIFY, (WPARAM) (nm.nmhdr.idFrom), (LPARAM) (&nm));
}
