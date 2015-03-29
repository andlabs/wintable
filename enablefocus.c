// 17 march 2015
#include "tablepriv.h"

// TODO merge with some other files?

// TODO which WM_xBUTTONDOWNs?
// TODO move to the end of the file?
EVENTHANDLER(mouseDownFocusHandler)
{
	// TODO other mouse buttons?
	// don't check SetFocus()'s error (http://stackoverflow.com/questions/24073695/winapi-can-setfocus-return-null-without-an-error-because-thats-what-im-see)
	// TODO will this generate WM_SETFOCUS?
	SetFocus(t->hwnd);
	return TRUE;
}

// TODO what happens if any of these functions fail?
HANDLER(enableFocusHandlers)
{
	switch (uMsg) {
	case WM_ENABLE:
		// TODO figure out exactly how to redraw this properly
		// XP changes the bg color, necessitating a full redraw
		// TODO draw checkboxes as disabled too? listview doesn't
		// (listview does nothing with checkboxes though, just draws the unchecked and checked states and does none of the capture stuff that we do, toggling on mouse down...)
	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		break;
	default:
		return FALSE;
	}
	// TODO redraw only the selected row on focus change
	updateAll(t);
	*lResult = 0;
	return TRUE;
}
