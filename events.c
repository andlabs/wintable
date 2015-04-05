// 5 december 2014
#include "tablepriv.h"

// general handlers for mouse events
// these run before the other mouse events, which use their results
// they also handle mouse tracking
// we DON'T go from LPARAM to row/column here because scrolling will change that
// or should we? it'd make some calculations later easier, and we're not going to update the hover state when scrolling anyway (real list view doesn't) TODO

// TODO rename to generalXxx everywhere
EVENTHANDLER(globalMouseMoveHandler)
{
	HRESULT hr;

	t->lastMouseMoved = t->mouseMoved;
	t->lastMouseMoveLPARAM = t->mouseMoveLPARAM;
	t->mouseMoved = TRUE;
	t->mouseMoveLPARAM = lParam;

	// when we capture the mouse, _TrackMouseEvent() won't work
	// (see http://blogs.msdn.com/b/oldnewthing/archive/2010/12/06/10100644.aspx)
	// for this case, we have to do the check ourselves
	if (lParamInRect(&(m->client), t->mouseMoveLPARAM) == 0)
		t->mouseMoved = FALSE;

	if (!t->lastMouseMoved) {
		TRACKMOUSEEVENT tm;

		ZeroMemory(&tm, sizeof (TRACKMOUSEEVENT));
		tm.cbSize = sizeof (TRACKMOUSEEVENT);
		tm.dwFlags = TME_LEAVE;
		tm.hwndTrack = t->hwnd;
		if (_TrackMouseEvent(&tm) == 0)
			;	// TODO
	} else {
		// redraw the old cell unconditionally
		hr = redrawCellAtLPARAM(t, m, t->lastMouseMoveLPARAM);
		if (hr != S_OK)
			;	// TODO
	}

	// redraw the new cell unconditionally
	// TODO optimize the redrawing somehow?
	hr = redrawCellAtLPARAM(t, m, t->mouseMoveLPARAM);
	if (hr != S_OK)
		;	// TODO

	// TODO really?
	return TRUE;
}

EVENTHANDLER(globalMouseLeaveHandler)
{
	t->lastMouseMoved = t->mouseMoved;
	t->lastMouseMoveLPARAM = t->mouseMoveLPARAM;
	t->mouseMoved = FALSE;
	// TODO really?
	return TRUE;
}

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
	globalMouseMoveHandler,
	tooltipMouseMoveHandler,
	NULL,
};

static const eventhandlerfunc mouseLeaveHandlers[] = {
	globalMouseLeaveHandler,
	tooltipMouseLeaveHandler,
	NULL,
};

static const eventhandlerfunc lbuttonDownHandlers[] = {
	mouseDownFocusHandler,
	mouseDownSelectHandler,
	checkboxMouseDownHandler,
	NULL,
};

static const eventhandlerfunc lbuttonUpHandlers[] = {
	checkboxMouseUpHandler,
	NULL,
};

static const eventhandlerfunc captureChangedHandlers[] = {
	checkboxCaptureChangedHandler,
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
	struct metrics m;
	HRESULT hr;

	// note that the metrics computing is in each case statement, not here
	// this is just in case the message isn't an event message, since we can't send the metrics back out
	switch (uMsg) {
#define eventHandler(msg, array, returnWhat) \
	case msg: \
		hr = metrics(t, &m); \
		if (hr != S_OK) \
			return FALSE; \
		return runEventHandlers(array, t, &m, uMsg, wParam, lParam, lResult, returnWhat);
	eventHandler(WM_KEYDOWN, keyDownHandlers, 0)
	eventHandler(WM_KEYUP, keyUpHandlers, 0)
	eventHandler(WM_CHAR, charHandlers, 0)
	eventHandler(WM_MOUSEMOVE, mouseMoveHandlers, 0)
	eventHandler(WM_MOUSELEAVE, mouseLeaveHandlers, 0)
	eventHandler(WM_LBUTTONDOWN, lbuttonDownHandlers, 0)
	eventHandler(WM_LBUTTONUP, lbuttonUpHandlers, 0)
	eventHandler(WM_CAPTURECHANGED, captureChangedHandlers, 0)
	eventHandler(WM_MOUSEWHEEL, mouseWheelHandlers, 0)
#undef eventHandler
	}
	return FALSE;
}
