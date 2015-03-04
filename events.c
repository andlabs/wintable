// 5 december 2014
#include "tablepriv.h"

// TODO make these function names consistent

static const eventhandlerfunc keyDownHandlers[] = {
	keyDownSelectHandler,
	NULL,
};

static const eventhandlerfunc keyUpHandlers[] = {
	NULL,
};

static const eventhandlerfunc charHandlers[] = {
	NULL,
};

static const eventhandlerfunc mouseMoveHandlers[] = {
//TODO	checkboxMouseMoveHandler,
	NULL,
};

// TODO which WM_xBUTTONDOWNs?
// TODO move to focus.c
EVENTHANDLER(mouseDownFocusHandler)
{
	// TODO other mouse buttons?
	// don't check SetFocus()'s error (http://stackoverflow.com/questions/24073695/winapi-can-setfocus-return-null-without-an-error-because-thats-what-im-see)
	// TODO will this generate WM_SETFOCUS?
	SetFocus(t->hwnd);
	return TRUE;
}

static const eventhandlerfunc lbuttonDownHandlers[] = {
	mouseDownFocusHandler,
	mouseDownSelectHandler,
//TODO	checkboxMouseDownHandler,
	NULL,
};

static const eventhandlerfunc lbuttonUpHandlers[] = {
//TODO	checkboxMouseUpHandler,
	NULL,
};

static const eventhandlerfunc captureChangedHandlers[] = {
//TODO	checkboxCaptureChangedHandler,
	NULL,
};

// TODO remove or something? depends on if we implement combobox and how
static const eventhandlerfunc mouseWheelHandlers[] = {
	NULL,
};

// TODO WM_MOUSEHOVER, other mouse buttons

// TODO what to do if metrics() fails?
HANDLER(eventHandlers)
{
	struct metrics *m;
	HRESULT hr;

	switch (uMsg) {
#define eventHandler(msg, array, returnWhat) \
	case msg: \
		hr = metrics(t, &m); \
		if (hr != S_OK) \
			return FALSE; \
		return runEventHandlers(array, t, m, uMsg, wParam, lParam, lResult, returnWhat);
	eventHandler(WM_KEYDOWN, keyDownHandlers, 0)
	eventHandler(WM_KEYUP, keyUpHandlers, 0)
	eventHandler(WM_CHAR, charHandlers, 0)
	eventHandler(WM_MOUSEMOVE, mouseMoveHandlers, 0)
	eventHandler(WM_LBUTTONDOWN, lbuttonDownHandlers, 0)
	eventHandler(WM_LBUTTONUP, lbuttonUpHandlers, 0)
	eventHandler(WM_CAPTURECHANGED, captureChangedHandlers, 0)
	eventHandler(WM_MOUSEWHEEL, mouseWheelHandlers, 0)
#undef eventHandler
	}
	return FALSE;
}
