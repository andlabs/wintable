// 7 december 2014
#include "tablepriv.h"

// TODO why doesn't this trigger on first show?
// TODO see if there's anything not metaphor related in the last bits of the scrollbar series
// TODO rename this to boot
// TODO merge with update.h?

// TODO what if one of these methods fails?
HANDLER(resizeHandler)
{
	WINDOWPOS *wp;

	if (uMsg != WM_WINDOWPOSCHANGED)
		return FALSE;
	wp = (WINDOWPOS *) lParam;
	if ((wp->flags & SWP_NOSIZE) != 0)
		return FALSE;
	// TODO redraw everything?
	update(t, TRUE);
	*lResult = 0;
	return TRUE;
}
