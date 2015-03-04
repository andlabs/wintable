// 16 august 2014
#include "tablepriv.h"

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

static HRESULT drawFrameControlCheckbox(HDC dc, RECT *r, int cbState)
{
	if (DrawFrameControl(dc, r, DFC_BUTTON, dfcState(cbState)) == 0)
		return logLastError("error drawing Table checkbox image with DrawFrameControl()");
	return S_OK;
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
	HRESULT hr;

	hr = GetThemePartSize(theme, dc, BP_CHECKBOX, themestates[cbState], NULL, TS_DRAW, s);
	if (hr != S_OK)
		return logHRESULT("error getting theme part size for Table checkboxes", hr);
	return S_OK;
}

static HRESULT drawThemeCheckbox(HDC dc, RECT *r, int cbState, HTHEME theme)
{
	HRESULT hr;

	hr = DrawThemeBackground(theme, dc, BP_CHECKBOX, themestates[cbState], r, NULL);
	if (hr != S_OK)
		return logHRESULT("error drawing Table checkbox image from theme", hr);
	return S_OK;
}

static HRESULT getThemeCheckboxSize(HDC dc, int *width, int *height, HTHEME theme)
{
	HRESULT hr;
	SIZE size;
	int cbState;

	hr = getStateSize(dc, 0, theme, &size);
	if (hr != S_OK)
		return hr;
	for (cbState = 1; cbState < checkboxnStates; cbState++) {
		SIZE against;

		hr = getStateSize(dc, cbState, theme, &against);
		if (hr != S_OK)
			return hr;
		if (size.cx != against.cx || size.cy != against.cy)
			return logLastError("size mismatch in Table checkbox states");		// TODO
	}
	*width = (int) size.cx;
	*height = (int) size.cy;
	return S_OK;
}

HRESULT drawCheckbox(struct table *t, HDC dc, RECT *r, int cbState)
{
	if (t->theme != NULL)
		return drawThemeCheckbox(dc, r, cbState, t->theme);
	return drawFrameControlCheckbox(dc, r, cbState);
}

// TODO really panic on failure?
HRESULT freeCheckboxThemeData(struct table *t)
{
	if (t->theme != NULL) {
		HRESULT hr;

		hr = CloseThemeData(t->theme);
		if (hr != S_OK)
			return logHRESULT("error closing Table checkbox theme", hr);
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
		return logLastError("error getting Table DC for loading checkbox theme data");
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
		return logLastError("error releasing Table DC for loading checkbox theme data");
	return S_OK;
}
