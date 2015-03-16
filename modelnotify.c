// 15 march 2015
#include "tablepriv.h"

// TODO what to do on errors?
HANDLER(modelNotificationHandler)
{
	tableModelNotificationParams *p = (tableModelNotificationParams *) lParam;

	if (uMsg != tableModelNotify)
		return FALSE;
	switch (p->code) {
	case tableModelNotifyRowAdded:
		// TODO do no special processing?
		/* detect the following:
		- adding row 0 when not scrolled there
		- adding row 0 when scrolled there
		- adding row nRows when not scrolled there
		- adding row nRows when scrolled there
		- adding a row above the first visible
		- adding a row below the last visible
		- adding a row AT the first visible
		- adding a row AT the last visible
		- adding a row AT the last partially visible
		- adding a row above the selected visible
		- adding a row below the selected visible
		- adding a row AT the selected visible
		*/
		// and do we scroll to the new row?
		// this will just queue a rescroll and redraw
		// this will push rows down if adding in the visible
		updateAll(t);
		*lResult = 0;
		return TRUE;
	case tableModelNotifyRowDeleted:
		// TODO all of the above tests, PLUS
		// - what happens if the selected row is deleted?
		// - do we scroll to the deleted row?
		// this will just move rows up if the deleted row is visible
		updateAll(t);
		*lResult = 0;
		return TRUE;
	case tableModelNotifyCellChanged:
		// TODO queue redraw for that cell
		*lResult = 0;
		return TRUE;
	}
	// TODO handle this
	return FALSE;
}
