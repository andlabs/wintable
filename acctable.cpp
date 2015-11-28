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
// TODO make struct tableAcc hold a C++ class or something

struct tableAcc {
	IRawElementProviderSimpleVtbl *lpVtbl;
	ULONG refcount;
};

// TODO
#define this hthis

static HRESULT STDMETHODCALLTYPE tableAccQueryInterface(IRawElementProviderSimple *this, REFIID riid, void **ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;
	if (IsEqualIID(riid, IID_IUnknown) ||
		IsEqualIID(riid, IID_IRawElementProviderSimple)) {
		IRawElementProviderSimple_AddRef(this);
		*ppvObject = this;
		return S_OK;
	}
	return E_NOINTERFACE;
}

#define tableAcc(x) ((struct tableAcc *) (x))

static ULONG STDMETHODCALLTYPE tableAccAddRef(IRawElementProviderSimple *this)
{
	struct tableAcc *t = tableAcc(this);

	t->refcount++;
	return t->refcount;
}

static ULONG STDMETHODCALLTYPE tableAccRelease(IRawElementProviderSimple *this)
{
	struct tableAcc *t = tableAcc(this);

	t->refcount--;
	if (t->refcount == 0) {
		// TODO
		return 0;
	}
	return t->refcount;
}

static HRESULT STDMETHODCALLTYPE tableAccGetPatternProvider(IRawElementProviderSimple *this, PATTERNID patternId, IUnknown **pRetVal)
{
	if (pRetVal == NULL)
		return E_POINTER;
	// TODO
	*pRetVal = NULL;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE tableAccGetPropertyValue(IRawElementProviderSimple *this, PROPERTYID propertyId, VARIANT *pRetVal)
{
	BSTR bstr;

	if (pRetVal == NULL)
		return E_POINTER;
	pRetVal->vt = VT_EMPTY;		// default behavior
	switch (propertyId) {
	case UIA_NamePropertyId:
		bstr = SysAllocString(L"test string");
		if (bstr == NULL)
			return E_OUTOFMEMORY;
		pRetVal->vt = VT_BSTR;
		pRetVal->bstrVal = bstr;
		break;
	}
	return S_OK;
}

static IRawElementProviderSimpleVtbl vtbl = {
// TOOD msvc
#if 0
	.QueryInterface = tableAccQueryInterface,
	.AddRef = tableAccAddRef,
	.Release = tableAccRelease,
	.GetPatternProvider = tableAccGetPatternProvider,
	.GetPropertyValue = tableAccGetPropertyValue,
#endif
};

static struct tableAcc ta;

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
	if (((DWORD) lParam) != ((DWORD) UiaRootObjectId))
		return FALSE;
	ta.lpVtbl = &vtbl;
	*lResult = UiaReturnRawElementProvider(t->hwnd, wParam, lParam, (IRawElementProviderSimple *) (&ta));
	return TRUE;
}
