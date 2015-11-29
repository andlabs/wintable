// 11 may 2015
// TODO split out winapi includes
#include "tablepriv.h"
// before we start, why is this file C++? because the following is not a valid C header file!
// namely, UIAutomationCoreApi.h forgets to typedef enum xxx { ... } xxx; and typedef struct xxx xxx; so it just uses xxx incorrectly
// and because these are enums, we can't just do typedef enum xxx xxx; here ourselves, as that's illegal in both C and C++!
// thanks, Microsoft!
// (if it was just structs I would just typedef them here but even that's not really futureproof)
#include <uiautomation.h>

// TODOs
// - make sure RPC_E_DISCONNECTED is correct; source it
// - make sure E_POINTER is correct

// well if we're stuck with C++, we might as well make the most of it
class tableAcc : public IRawElementProviderSimple {
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
};

tableAcc::tableAcc(struct table *t)
{
	this->t = t;
	this->refcount = 1;		// first instance goes to the table
}

void tableAcc::Invalidate(void)
{
	// this will always be called before the tableAcc is destroyed because there's one ref given to the table itself
	this->t = NULL;
}

// TODO are the static_casts necessary or will a good old C cast do?
STDMETHODIMP tableAcc::QueryInterface(REFIID riid, void **ppvObject)
{
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
	if (IsEqualIID(riid, IID_IGridProvider)) {
		this->AddRef();
		*ppvObject = static_cast<IGridProvider *>(this);
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

// TODO again with static_casts
STDMETHODIMP tableAcc::GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
#if 0
// TODO
	if (patternId == UIA_ListPatternId) {
		this->AddRef();
		*pRetVal = static_cast<IRawElementProviderSimple *>(this);
		return S_OK;
	}
#endif
	// TODO datagrid pattern?
	*pRetVal = NULL;
	return S_OK;
}

STDMETHODIMP tableAcc::GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal)
{
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
		pRetVal->lVal = UIA_ListControlTypeId;
		break;
	case UIA_NamePropertyId:
		// TODO do we specify this ourselves? or let the parent window provide it?
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
			return E_POINTER;
		// TODO correct?
		*pRetVal = NULL;
		return RPC_E_DISCONNECTED;
	}
	// according to https://msdn.microsoft.com/en-us/library/windows/desktop/ee671597%28v=vs.85%29.aspx this is correct for the top-level provider
	return UiaHostProviderFromHwnd(this->t->hwnd, pRetVal);
}

STDMETHODIMP tableAcc::get_ProviderOptions(ProviderOptions *pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	// TODO ProviderOptions_UseClientCoordinates?
	*pRetVal = ProviderOptions_ServerSideProvider;
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
