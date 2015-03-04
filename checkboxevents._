// 16 august 2014
#include "tablepriv.h"

// TODO wait did we really need WM_MOUSELEAVE after all?

// TODO http://stackoverflow.com/a/22695333/3408572

// TODO what happens if any of these functions fail?
EVENTHANDLER(checkboxMouseMoveHandler)
{
	// TODO see http://stackoverflow.com/a/28731761/3408572
	// TODO when rewriting, handle captures/the possibility that the mouse may not be in the client rect

$	if the mouse was previously in the client rect
$		redraw the cell at the previous mouse position
$	mark the mouse as being in the client rect
$	redraw the cell at the current mouse position
	return TRUE;
}

// TODO if we click on a partially invisible checkbox, should the mouse be moved up along with the scroll?
// TODO what happens if any of these fail?
EVENTHANDLER(checkboxMouseDownHandler)
{
$	determine if the mouse landed on a checkbox
$	if so
$		SetCapture()
$		mark that we are clicking a checkbox
	return TRUE;
}

// TODO what happens if any of these fail?
EVENTHANDLER(checkboxMouseUpHandler)
{
$	if we weren't clicking a checkbox
		return FALSE;
$	leave checkbox clicking mode
$	ReleaseCapture()
$	if the mouse was released in the same checkbox
$		toggle it
$	TODO else return FALSE?
$	TODO redraw the checkbox in question
}

// TODO what happens if any of these fail?
EVENTHANDLER(checkboxCaptureChangedHandler)
{
$	if we weren't in checkbox clicking mode
		return FALSE;
$	leave checkbox clicking mode
$	TODO redraw the checkbox in question
	return TRUE;
}
