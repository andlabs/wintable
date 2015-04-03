// 12 march 2015
#include "tablepriv.h"

// the null Table model simplifies the implementation of Table itself by providing a default model implementation that represents a blank Table
// this is the model that the Table uses before the programmer sets one themselves
// it is DELIBERATELY NOT a valid COM object!
// - AddRef() and Release() return 1 without doing any refcounting or memory management (the null model must always be live)
// - QueryInterface() returns E_NOTIMPL
// and it breaks standard tableModel rules
// - only tableColumnCount() and tableRowCount() return anything
// - all other methods return E_NOTIMPL
// the code in Table to unsubscribe specifically checks for the null model and you can't change back to the null model (TODO should we allow it?) so there's no harm in that
// anything else will be picked up by error checking code so we can catch implementation bugs

static HRESULT STDMETHODCALLTYPE nullModelQueryInterface(tableModel *this, REFIID riid, void **ppvObject)
{
	return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE nullModelAddRef(tableModel *this)
{
	return 1;
}

static ULONG STDMETHODCALLTYPE nullModelRelease(tableModel *this)
{
	return 1;
}

static HRESULT STDMETHODCALLTYPE nullModeltableSubscribe(tableModel *this, HWND hwnd)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE nullModeltableUnsubscribe(tableModel *this, HWND hwnd)
{
	return E_NOTIMPL;
}

static void STDMETHODCALLTYPE nullModeltableNotify(tableModel *this, tableModelNotificationParams *p)
{
	// TODO make some noise here
}

static intmax_t STDMETHODCALLTYPE nullModeltableColumnCount(tableModel *this)
{
	return 0;
}

static HRESULT STDMETHODCALLTYPE nullModeltableColumnType(tableModel *this, intmax_t column, int *colType)
{
	return E_NOTIMPL;
}

static intmax_t STDMETHODCALLTYPE nullModeltableRowCount(tableModel *this)
{
	return 0;
}

static HRESULT STDMETHODCALLTYPE nullModeltableCellValue(tableModel *this, intmax_t row, intmax_t column, int expectedColumnType, tableCellValue *value)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE nullModeltableDrawImageCell(tableModel *this, intmax_t row, intmax_t column, HDC hdc, RECT *rDest)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE nullModeltableIsColumnMutable(tableModel *this, intptr_t column)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE nullModeltableSetCellValue(tableModel *this, intmax_t row, intmax_t column, tableCellValue data)
{
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE nullModeltableCellToggleBool(tableModel *this, intmax_t row, intmax_t column)
{
	return E_NOTIMPL;
}

static const tableModelVtbl nullModelVtbl = {
	.QueryInterface = nullModelQueryInterface,
	.AddRef = nullModelAddRef,
	.Release = nullModelRelease,
	.tableSubscribe = nullModeltableSubscribe,
	.tableUnsubscribe = nullModeltableUnsubscribe,
	.tableNotify = nullModeltableNotify,
	.tableColumnCount = nullModeltableColumnCount,
	.tableColumnType = nullModeltableColumnType,
	.tableRowCount = nullModeltableRowCount,
	.tableCellValue = nullModeltableCellValue,
	.tableDrawImageCell = nullModeltableDrawImageCell,
	.tableIsColumnMutable = nullModeltableIsColumnMutable,
	.tableSetCellValue = nullModeltableSetCellValue,
	.tableCellToggleBool = nullModeltableCellToggleBool,
};

const tableModel nullModel = {
	.lpVtbl = &nullModelVtbl,
};
