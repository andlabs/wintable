// 4 april 2015
#include "tablepriv.h"

/* list view tooltip behavior:
- after hovering over an ellipsized item, the tooltip appears in place
- the tooltip goes away when:
	- vertically scrolling
	- [TODO] horizontally scrolling
	- changing selection to something else
		- in which case, [TODO] unlike the others, tooltips don't come back until [TODO] something else happens
	- [TODO] clicking on the hovered-over cell, regardless of whether it is selected
	- moving the mouse to another cell
	- moving the mouse outside the control
	- [TODO] moving the mouse onto the nonclient area
	- [TODO] activating the system menu or some other control
- tooltips do NOT go away when
	- toggling the list view checkbox
	- pressing other unhandled keys
- tooltips are INHIBITED during capture (marquee selection in the case of the list view, checkbox mouse down in the case of Table)
- [TODO] if I move to another cell and back, the tooltip comes back
- [TODO] the rect that controls whether the tooltip is triggered includes the margin of the cell to the left of the first pixel of its text
- the tooltip shows up with the top left corner of the text would be if we had the cell fully visible, even if we''re scrolled right
- the tooltip does not seem to honor the 80-character limit and will show even absurdly long single-word tooltips
	- [TODO] multi-word labels
*/
