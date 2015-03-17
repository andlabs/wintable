// 17 march 2015
#include "tablepriv.h"

// TODO merge with some other files?

// TODO should WM_ENABLE's handler really redraw everything?
// TODO what happens if any of these functions fail?
HANDLER(enableFocusHandlers)
{
	switch (uMsg) {
	case WM_ENABLE:
	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		break;
	default:
		return FALSE;
	}
	// TODO redraw only the selected row
	updateAll(t);
	*lResult = 0;
	return TRUE;
}
