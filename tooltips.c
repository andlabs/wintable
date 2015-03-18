// 18 march 2015
#include "tablepriv.h"

// TODO rename to tooltip.c?

// TODO set font
HRESULT makeTooltip(struct table *t, HINSTANCE hInstance)
{
	TOOLINFOW ti;

	// TODO verify extended styles and WS_POPUP
	// TODO TTS_NOANIMATE and TTS_NOFADE? check list view control after changing transition animation
	// TODO TTS_ALWAYSTIP? check list view control
	t->tooltip = CreateWindowExW(WS_EX_TOOLWINDOW,
		TOOLTIPS_CLASSW, L"",
		WS_POPUP | TTS_NOPREFIX,
		0, 0,
		0, 0,		// TODO really?
		// TODO really NULL control ID?
		t->hwnd, NULL, hInstance, NULL);
	if (t->tooltip == NULL)
		return logLastError("error creating Table tooltip");
	ZeroMemory(&ti, sizeof (TOOLINFOW));
	ti.cbSize = sizeof (TOOLINFOW);
	// TODO TTF_TRANSPARENT?
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = t->hwnd;
	ti.uId = (UINT_PTR) (t->hwnd);
	ti.hinst = hInstance;		// TODO
	if (SendMessageW(t->tooltip, TTM_ADDTOOL, 0, (LPARAM) (&ti)) == FALSE)
		return logLastError("error setting up Table tooltip");
	return S_OK;
}

// TODO really error out here? it's cleanup...
HRESULT destroyTooltip(struct table *t)
{
	if (DestroyWindow(t->tooltip) == 0)
		return logLastError("error destroying Table tooltip");
	return S_OK;
}
