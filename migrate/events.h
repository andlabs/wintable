// 5 december 2014

// TODO handler functions don't work here because you can't have more than one for the mouse ones...

static const handlerfunc keyDownHandlers[] = {
	keyDownSelectHandler,
	NULL,
};

static const handlerfunc keyUpHandlers[] = {
	NULL,
};

static const handlerfunc charHandlers[] = {
	NULL,
};

static const handlerfunc mouseMoveHandlers[] = {
	checkboxMouseMoveHandler,
	NULL,
};

// TODO which WM_xBUTTONDOWNs?
// TODO move to focus.h
HANDLER(mouseDownFocusHandler)
{
	// TODO other mouse buttons?
	// don't check SetFocus()'s error (http://stackoverflow.com/questions/24073695/winapi-can-setfocus-return-null-without-an-error-because-thats-what-im-see)
	// TODO NotifyWinEvent() here?
	// TODO will this generate WM_SETFOCUS?
	SetFocus(t->hwnd);
	*lResult = 0;
	return TRUE;
}

static const handlerfunc lbuttonDownHandlers[] = {
	mouseDownFocusHandler,
	mouseDownSelectHandler,
	NULL,
};

static const handlerfunc lbuttonUpHandlers[] = {
	checkboxMouseUpHandler,
	NULL,
};

static const handlerfunc captureChangedHandlers[] = {
	checkboxCaptureChangedHandler,
	NULL,
};

// TODO remove or something? depends on if we implement combobox and how
static const handlerfunc mouseWheelHandlers[] = {
	NULL,
};

// TODO WM_MOUSEHOVER, other mouse buttons

HANDLER(eventHandlers)
{
	switch (uMsg) {
#define eventHandler(msg, array) \
	case msg: \
		return runEventHandlers(array, t, uMsg, wParam, lParam, lResult);
	eventHandler(WM_KEYDOWN, keyDownHandlers)
	eventHandler(WM_KEYUP, keyUpHandlers)
	eventHandler(WM_CHAR, charHandlers)
	eventHandler(WM_MOUSEMOVE, mouseMoveHandlers)
	eventHandler(WM_LBUTTONDOWN, lbuttonDownHandlers)
	eventHandler(WM_LBUTTONUP, lbuttonUpHandlers)
	eventHandler(WM_CAPTURECHANGED, captureChangedHandlers)
	eventHandler(WM_MOUSEWHEEL, mouseWheelHandlers)
#undef eventHandler
	}
	return FALSE;
}
