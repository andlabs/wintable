TODO
- make panic messages consistent ("xxx in yyy()")
	- make sure all error messages involving InvalidateRect() are consistent with regards to "redrawing" and "queueing for redraw"
- make sure all cases of 'redrawing' that's really 'queueing to redraw' say that they are 'queueing to redraw'
- see whether HDITEMW.cxy == (ITEMRECT.right - ITEMRECT.left)
- WM_THEMECHANGED, etc.
- see if vertical centering is really what we want or if we just want to offset by a few pixels or so
- going right from column 0 to column 2 with the right arrow key deselects
	- reproduce
- checkbox columns don't clip to the column width
- send standard notification codes
- draw themed WS_EX_CLIENTEDGE (and WS_EX_STATICEDGE and WS_EX_WINDOWEDGE?)
- in-place tooltips for text columns
- see if we can get tableRealloc() to not be destructive on failure and change every use of tableRealloc() to boot
- make sure all arithmetic operations are strongly typed
- http://blogs.msdn.com/b/oldnewthing/archive/2006/01/03/508694.aspx both the double buffering and the RDC fix for it?
- use an array of column offsets from 0 instead of looping each time?
- see if there are any places metrics are computed more than once and fix them
- document exported methods; tableHINSTANCE() cannot fail.
- what happens if there is a tableSetModel message or or the table is destroyed while we have the mouse capture?
- add prefast annotations
	- enable prefast in the msvc makefile
- WM_THEMECHANGED etc.
	- what message was fo setting GWL_STYLE/GWL_EXSTYLE?
- http://blogs.msdn.com/b/oldnewthing/archive/2006/06/28/649680.aspx making sure mouse scrolling is consistent and that mouse event handlers are done in the right order
	- especially again after we add tooltips
- https://msdn.microsoft.com/en-us/library/dd162593%28v=vs.85%29.aspx?
- extern "C" table.h
- handle WS_EX_LAYOUTRTL, WS_EX_RTLREADING?
- use uxtheme APIs to draw the control in a list view style?
- comctl6 manifest
- https://msdn.microsoft.com/en-us/library/windows/desktop/dn312083%28v=vs.85%29.aspx
- http://blogs.msdn.com/b/oldnewthing/archive/2004/01/12/57833.aspx provide a DEF file
