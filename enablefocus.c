// 17 march 2015
#include "tablepriv.h"

// TODO merge with some other files?

// TODO what happens if any of these functions fail?
HANDLER(enableFocusHandlers)
{
	switch (uMsg) {
	case WM_ENABLE:
		// TODO figure out exactly how to redraw this properly
		// XP changes the bg color, necessitating a full redraw
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
