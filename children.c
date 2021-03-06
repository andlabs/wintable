// 7 december 2014
#include "tablepriv.h"

static const handlerfunc commandHandlers[] = {
	NULL,
};

static const handlerfunc notifyHandlers[] = {
	headerNotifyHandler,
	tooltipNotifyHandler,
	NULL,
};

HANDLER(childrenHandlers)
{
	if (uMsg == WM_COMMAND)
		return runHandlers(commandHandlers, t, uMsg, wParam, lParam, lResult);
	if (uMsg == WM_NOTIFY)
		return runHandlers(notifyHandlers, t, uMsg, wParam, lParam, lResult);
	return FALSE;
}
