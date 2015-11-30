// 11 may 2015
// TODO split out winapi includes
#include "tablepriv.h"
// before we start, why is this file C++? because the following is not a valid C header file!
// namely, UIAutomationCoreApi.h forgets to typedef enum xxx { ... } xxx; and typedef struct xxx xxx; so it just uses xxx incorrectly
// and because these are enums, we can't just do typedef enum xxx xxx; here ourselves, as that's illegal in both C and C++!
// thanks, Microsoft!
// (if it was just structs I would just typedef them here but even that's not really futureproof)
#include <uiautomation.h>
// well if we're stuck with C++, we might as well make the most of it

// For the top-level table, we implement the List control type because the standard Windows list view does so.

// TODOs
// - make sure E_POINTER is correct; examples use E_INVALIDARG
// - children must implement IRawElementProviderFragment

#define eDisconnected UIA_E_ELEMENTNOTAVAILABLE
#define ePointer E_POINTER

class tableAcc :
	public IRawElementProviderSimple,
	public IRawElementProviderFragmentRoot {
	struct table *t;
	ULONG refcount;
public:
	tableAcc(struct table *);

	// internal methods
	void tDisconnect(void);
	// TODDO event support - see what events the host provider provides for us (from the same link that shows host properties?)

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// IRawElementProviderSimple
	STDMETHODIMP GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal);
	STDMETHODIMP GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal);
	STDMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple **pRetVal);
	STDMETHODIMP get_ProviderOptions(ProviderOptions *pRetVal);

	// IRawElementProviderFragmentRoot
	STDMETHODIMP ElementProviderFromPoint(double x, double y, IRawElementProviderFragment **pRetVal);
	STDMETHODIMP GetFocus(IRawElementProviderFragment **pRetVal);
};

tableAcc::tableAcc(struct table *t)
{
	this->t = t;
	this->refcount = 1;		// first instance goes to the table
}

// TODO are the static_casts necessary or will a good old C cast do?
// note that this first one is not to IUnknown, as QueryInterface() requires us to always return the same pointer to IUnknown
#define IU(x) (static_cast<tableAcc *>(x))
#define IREPS(x) (static_cast<IRawElementProviderSimple *>(x))
#define IREPFR(x) (static_cast<IRawElementProviderFragmentRoot *>(x))

// this will always be called before the tableAcc is destroyed because there's one ref given to the table itself
void tableAcc::tDisconnect(void)
{
	HRESULT hr;

	hr = UiaDisconnectProvider(IREPS(this));
	if (hr != S_OK)
		logHRESULT("error disconnecting UI Automation root provider for table in tableAcc::tDisconnect()", hr);
	this->t = NULL;
}

STDMETHODIMP tableAcc::QueryInterface(REFIID riid, void **ppvObject)
{
	if (ppvObject == NULL)
		return ePointer;
	if (IsEqualIID(riid, IID_IUnknown)) {
		this->AddRef();
		*ppvObject = IU(this);
		return S_OK;
	}
	if (IsEqualIID(riid, IID_IRawElementProviderSimple)) {
		this->AddRef();
		*ppvObject = IREPS(this);
		return S_OK;
	}
	if (IsEqualIID(riid, IID_IRawElementProviderFragmentRoot)) {
		this->AddRef();
		*ppvObject = IREPFR(this);
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) tableAcc::AddRef(void)
{
	// http://blogs.msdn.com/b/oldnewthing/archive/2005/09/27/474384.aspx
	if (this->refcount == 0)
		logLastError("tableAcc::AddRef() called during destruction");
	this->refcount++;
	return this->refcount;
}

STDMETHODIMP_(ULONG) tableAcc::Release(void)
{
	this->refcount--;
	if (this->refcount == 0) {
		delete this;
		return 0;
	}
	return this->refcount;
}

STDMETHODIMP tableAcc::GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal)
{
	if (pRetVal == NULL)
		return ePointer;
	// TODO which patterns should we provide? inspect the listview control to find out
	*pRetVal = NULL;
	return S_OK;
}

// TODO https://msdn.microsoft.com/en-us/library/windows/desktop/ee671615%28v=vs.85%29.aspx specifies which properties we can ignore
STDMETHODIMP tableAcc::GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal)
{
	BSTR bstr;

	if (pRetVal == NULL)
		return ePointer;
	pRetVal->vt = VT_EMPTY;		// behavior on unknown property is to keep it VT_EMPTY and return S_OK
	if (this->t == NULL)
		return eDisconnected;
	switch (propertyId) {
	case UIA_ControlTypePropertyId:
		pRetVal->vt = VT_I4;
//TODO		pRetVal->lVal = UIA_ListControlTypeId;
		break;
	case UIA_NamePropertyId:
		// TODO remove this once everything works; we only use it to test
		bstr = SysAllocString(L"test string");
		if (bstr == NULL)
			return E_OUTOFMEMORY;
		pRetVal->vt = VT_BSTR;
		pRetVal->bstrVal = bstr;
		break;
#if 0
	case UIA_NativeWindowHandlePropertyId:
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
	if (this->t == NULL) {
		if (pRetVal == NULL)
			return ePointer;
		*pRetVal = NULL;
		return eDisconnected;
	}
	// according to https://msdn.microsoft.com/en-us/library/windows/desktop/ee671597%28v=vs.85%29.aspx this is correct for the top-level provider
	return UiaHostProviderFromHwnd(this->t->hwnd, pRetVal);
}

STDMETHODIMP tableAcc::get_ProviderOptions(ProviderOptions *pRetVal)
{
	if (pRetVal == NULL)
		return ePointer;
	*pRetVal = ProviderOptions_ServerSideProvider;
	return S_OK;
}

STDMETHODIMP tableAcc::ElementProviderFromPoint(double x, double y, IRawElementProviderFragment **pRetVal)
{
	// TODO
	return E_NOTIMPL;
	// note: x and y are in screen coordinates
}

STDMETHODIMP tableAcc::GetFocus(IRawElementProviderFragment **pRetVal)
{
	// TODO
	return E_NOTIMPL;
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
	a->tDisconnect();
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
	*lResult = UiaReturnRawElementProvider(t->hwnd, wParam, lParam, IREPS(t->tableAcc));
	return TRUE;
}
