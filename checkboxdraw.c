// 16 august 2014
#include "tablepriv.h"

// TODO
#define panic(...) abort()

// TODO http://stackoverflow.com/a/22695333/3408572

static UINT dfcState(int cbstate)
{
	UINT ret;

	ret = DFCS_BUTTONCHECK;
	if ((cbstate & checkboxStateChecked) != 0)
		ret |= DFCS_CHECKED;
	if ((cbstate & checkboxStateHot) != 0)
		ret |= DFCS_HOT;
	if ((cbstate & checkboxStatePushed) != 0)
		ret |= DFCS_PUSHED;
	return ret;
}

static DWORD drawFrameControlCheckbox(HDC dc, RECT *r, int cbState)
{
	if (DrawFrameControl(dc, r, DFC_BUTTON, dfcState(cbState)) == 0)
		return panicLastError("error drawing Table checkbox image with DrawFrameControl()");
	return 0;
}

static HRESULT getFrameControlCheckboxSize(HDC dc, int *width, int *height)
{
	// there's no real metric around
	// let's use SM_CX/YSMICON and hope for the best
	*width = GetSystemMetrics(SM_CXSMICON);
	*height = GetSystemMetrics(SM_CYSMICON);
	return S_OK;
}

static int themestates[checkboxnStates] = {
	CBS_UNCHECKEDNORMAL,			// 0
	CBS_CHECKEDNORMAL,				// checked
	CBS_UNCHECKEDHOT,				// hot
	CBS_CHECKEDHOT,					// checked | hot
	CBS_UNCHECKEDPRESSED,			// pushed
	CBS_CHECKEDPRESSED,				// checked | pushed
	CBS_UNCHECKEDPRESSED,			// hot | pushed
	CBS_CHECKEDPRESSED,				// checked | hot | pushed
};

static HRESULT getStateSize(HDC dc, int cbState, HTHEME theme, SIZE *s)
{
	// TODO rename this and all future instances to hr
	HRESULT res;

	res = GetThemePartSize(theme, dc, BP_CHECKBOX, themestates[cbState], NULL, TS_DRAW, s);
	if (res != S_OK)
		return panicHRESULT("error getting theme part size for Table checkboxes", res);
	return res;
}

static HRESULT drawThemeCheckbox(HDC dc, RECT *r, int cbState, HTHEME theme)
{
	HRESULT res;

	res = DrawThemeBackground(theme, dc, BP_CHECKBOX, themestates[cbState], r, NULL);
	if (res != S_OK)
		return panicHRESULT("error drawing Table checkbox image from theme", res);
	return res;
}

static HRESULT getThemeCheckboxSize(HDC dc, int *width, int *height, HTHEME theme)
{
	HRESULT res;
	SIZE size;
	int cbState;

	res = getStateSize(dc, 0, theme, &size);
	if (res != S_OK)
		return res;
	for (cbState = 1; cbState < checkboxnStates; cbState++) {
		SIZE against;

		res = getStateSize(dc, cbState, theme, &against);
		if (res != S_OK)
			return res;
		if (size.cx != against.cx || size.cy != against.cy)
			panic("size mismatch in Table checkbox states");
	}
	*width = (int) size.cx;
	*height = (int) size.cy;
	return S_OK;
}

HRESULT drawCheckbox(struct table *t, HDC dc, RECT *r, int cbState)
{
	DWORD le;

	if (t->theme != NULL)
		return drawThemeCheckbox(dc, r, cbState, t->theme);
	le = drawFrameControlCheckbox(dc, r, cbState);
	if (le != 0)
		return HRESULT_FROM_WIN32(le);
	return S_OK;
}

// TODO really panic on failure?
HRESULT freeCheckboxThemeData(struct table *t)
{
	if (t->theme != NULL) {
		HRESULT res;

		res = CloseThemeData(t->theme);
		if (res != S_OK)
			return panicHRESULT("error closing Table checkbox theme", res);
		t->theme = NULL;
	}
	return S_OK;
}

// TODO really panic on failure to ReleaseDC()?
HRESULT loadCheckboxThemeData(struct table *t)
{
	HDC dc;
	HRESULT hr;

	hr = freeCheckboxThemeData(t);
	if (hr != S_OK)
		return hr;
	dc = GetDC(t->hwnd);
	if (dc == NULL)
		return panicLastErrorAsHRESULT("error getting Table DC for loading checkbox theme data");
	// ignore error; if it can't be done, we can fall back to DrawFrameControl()
	if (t->theme == NULL)		// try to open the theme
		t->theme = OpenThemeData(t->hwnd, L"button");
	if (t->theme != NULL)		// use the theme
		hr = getThemeCheckboxSize(dc, &(t->checkboxWidth), &(t->checkboxHeight), t->theme);
	else						// couldn't open; fall back
		hr = getFrameControlCheckboxSize(dc, &(t->checkboxWidth), &(t->checkboxHeight));
	if (hr != S_OK)
		return hr;
	if (ReleaseDC(t->hwnd, dc) == 0)
		return panicLastErrorAsHRESULT("error releasing Table DC for loading checkbox theme data");
	return S_OK;
}
