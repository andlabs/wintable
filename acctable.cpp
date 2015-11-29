// 11 may 2015
// TODO split out winapi includes
#include "tablepriv.h"
// before we start, why is this file C++? because the following is not a valid C header file!
// namely, UIAutomationCoreApi.h forgets to typedef enum xxx { ... } xxx; and typedef struct xxx xxx; so it just uses xxx incorrectly
// and because these are enums, we can't just do typedef enum xxx xxx; here ourselves, as that's illegal in both C and C++!
// thanks, Microsoft!
// (if it was just structs I would just typedef them here but even that's not really futureproof)
#include <uiautomation.h>

// TODO
//#include <stdio.h>
extern "C" int printf(const char *,...);

// TODOs
// - make sure RPC_E_DISCONNECTED is correct; source it
// - make sure E_POINTER is correct

// well if we're stuck with C++, we might as well make the most of it
// TODO do we want to be a table or something else?
class tableAcc : public IRawElementProviderSimple, public ITableProvider, public IGridProvider {
	struct table *t;
	ULONG refcount;
public:
	tableAcc(struct table *);

	// internal methods
	void Invalidate(void);

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// IRawElementProviderSimple
	STDMETHODIMP GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal);
	STDMETHODIMP GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal);
	STDMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple **pRetVal);
	STDMETHODIMP get_ProviderOptions(ProviderOptions *pRetVal);

	// ITableProvider
	STDMETHODIMP GetColumnHeaders(SAFEARRAY **pRetVal);
	STDMETHODIMP GetRowHeaders(SAFEARRAY **pRetVal);
	STDMETHODIMP get_RowOrColumnMajor(RowOrColumnMajor *pRetVal);

	// IGridProvider
	STDMETHODIMP GetItem(int row, int column, IRawElementProviderSimple **pRetVal);
	STDMETHODIMP get_ColumnCount(int *pRetVal);
	STDMETHODIMP get_RowCount(int *pRetVal);
};

tableAcc::tableAcc(struct table *t)
{
printf("construct\n");
	this->t = t;
	this->refcount = 1;		// first instance goes to the table
}

void tableAcc::Invalidate(void)
{
printf("invalidate\n");
	// this will always be called before the tableAcc is destroyed because there's one ref given to the table itself
	this->t = NULL;
}

// TODO are the static_casts necessary or will a good old C cast do?
STDMETHODIMP tableAcc::QueryInterface(REFIID riid, void **ppvObject)
{
printf("query interface\n");
	if (ppvObject == NULL)
		return E_POINTER;
	if (IsEqualIID(riid, IID_IUnknown)) {
		this->AddRef();
		*ppvObject = static_cast<tableAcc *>(this);
		return S_OK;
	}
	if (IsEqualIID(riid, IID_IRawElementProviderSimple)) {
		this->AddRef();
		*ppvObject = static_cast<IRawElementProviderSimple *>(this);
		return S_OK;
	}
	if (IsEqualIID(riid, IID_ITableProvider)) {
		this->AddRef();
		*ppvObject = static_cast<ITableProvider *>(this);
		return S_OK;
	}
	if (IsEqualIID(riid, IID_IGridProvider)) {
		this->AddRef();
		*ppvObject = static_cast<IGridProvider *>(this);
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) tableAcc::AddRef(void)
{
printf("add ref\n");
	// http://blogs.msdn.com/b/oldnewthing/archive/2005/09/27/474384.aspx
	if (this->refcount == 0)
		logLastError("tableAcc::AddRef() called during destruction");
	this->refcount++;
	return this->refcount;
}

STDMETHODIMP_(ULONG) tableAcc::Release(void)
{
printf("release\n");
	this->refcount--;
	if (this->refcount == 0) {
printf("destroy\n");
		delete this;
		return 0;
	}
	return this->refcount;
}

// TODO again with static_casts
STDMETHODIMP tableAcc::GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal)
{
printf("get pattern provider\n");
	if (pRetVal == NULL)
		return E_POINTER;
	if (patternId == UIA_TablePatternId) {
		this->AddRef();
		*pRetVal = static_cast<ITableProvider *>(this);
		return S_OK;
	}
	// TODO grid pattern?
	*pRetVal = NULL;
	return S_OK;
}

STDMETHODIMP tableAcc::GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal)
{
printf("get property value %d\n", (int)propertyId);
	BSTR bstr;

	if (pRetVal == NULL)
		return E_POINTER;
	// TODO keep this on error?
	pRetVal->vt = VT_EMPTY;		// behavior on unknown property is to keep it VT_EMPTY and return S_OK
	if (this->t == NULL)
		return RPC_E_DISCONNECTED;
	switch (propertyId) {
	case UIA_ControlTypePropertyId:
		pRetVal->vt = VT_I4;
		pRetVal->lVal = UIA_TableControlTypeId;
		break;
	case UIA_NamePropertyId:
		// TODO do we specify this ourselves? or let the parent window provide it?
printf("getting name\n");
		bstr = SysAllocString(L"test string");
		if (bstr == NULL)
			return E_OUTOFMEMORY;
		pRetVal->vt = VT_BSTR;
		pRetVal->bstrVal = bstr;
		break;
#if 0
	case UIA_NativeWindowHandlePropertyId:
printf("getting window handle\n");
		// TODO the docs say VT_I4
		// a window handle is a pointer
		// 64-bit issue?
		break;
#endif
	}
	return S_OK;
}

STDMETHODIMP tableAcc::get_HostRawElementProvider(IRawElementProviderSimple **pRetVal)
{
printf("get host raw element provider\n");
	if (this->t == NULL) {
		if (pRetVal == NULL)
			return E_POINTER;
		// TODO correct?
		*pRetVal = NULL;
		return RPC_E_DISCONNECTED;
	}
	// TODO wait should we?
//	return UiaHostProviderFromHwnd(this->t->hwnd, pRetVal);
	*pRetVal = NULL;
	return S_OK;
}

STDMETHODIMP tableAcc::get_ProviderOptions(ProviderOptions *pRetVal)
{
printf("get provider options\n");
	if (pRetVal == NULL)
		return E_POINTER;
	// TODO ProviderOptions_UseClientCoordinates?
	*pRetVal = ProviderOptions_ServerSideProvider;
	return S_OK;
}

STDMETHODIMP tableAcc::GetColumnHeaders(SAFEARRAY **pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	*pRetVal = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP tableAcc::GetRowHeaders(SAFEARRAY **pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	*pRetVal = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP tableAcc::get_RowOrColumnMajor(RowOrColumnMajor *pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	*pRetVal = RowOrColumnMajor_RowMajor;
	return S_OK;
}

STDMETHODIMP tableAcc::GetItem(int row, int column, IRawElementProviderSimple **pRetVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP tableAcc::get_ColumnCount(int *pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	if (this->t == NULL)
		return RPC_E_DISCONNECTED;
	*pRetVal = this->t->nColumns;
	return S_OK;
}

STDMETHODIMP tableAcc::get_RowCount(int *pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	if (this->t == NULL)
		return RPC_E_DISCONNECTED;
	*pRetVal = t->model->tableRowCount();
	return S_OK;
}

void initTableAcc(struct table *t)
{
	tableAcc *a;

	a = new tableAcc(t);
	t->tableAcc = a;
}

void uninitTableAcc(struct table *t)
{
	tableAcc *a;

	a = (tableAcc *) (t->tableAcc);
	a->Invalidate();
	a->Release();
	t->tableAcc = NULL;
}

HANDLER(accessibilityHandler)
{
	if (uMsg != WM_GETOBJECT)
		return FALSE;
	// OBJID_CLIENT evaluates to an expression of type LONG
	// the documentation for WM_GETOBJECT says to cast "it" to a DWORD before comparing
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd373624%28v=vs.85%29.aspx casts them both to DWORDs; let's do that
	// its two siblings only cast lParam, resulting in an erroneous DWORD to LONG comparison
	// The Old New Thing book does not cast anything
	// Microsoft's MSAA sample casts lParam to LONG instead!
	// (As you can probably tell, the biggest problem with MSAA is that its documentation is ambiguous and/or self-contradictory...)
	// and https://msdn.microsoft.com/en-us/library/windows/desktop/ff625912%28v=vs.85%29.aspx casts them both to long!
	// Note that we're not using Active Accessibility, but the above applies even more, because UiaRootObjectId is *NEGATIVE*!
	if (((DWORD) lParam) != ((DWORD) UiaRootObjectId))
		return FALSE;
	*lResult = UiaReturnRawElementProvider(t->hwnd, wParam, lParam, (IRawElementProviderSimple *) (t->tableAcc));
	return TRUE;
}
