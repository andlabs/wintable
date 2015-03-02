// 7 january 2015

// TODO rename to tableimpl.h?

// TODO prune the include list when finished
#define UNICODE
#define _UNICODE
#define STRICT
#define STRICT_TYPED_ITEMIDS
#define CINTERFACE
#define COBJMACROS
// see https://github.com/golang/go/issues/9916#issuecomment-74812211
#define INITGUID
// get Windows version right; right now Windows XP
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0501		/* according to Microsoft's winperf.h */
#define _WIN32_IE 0x0600			/* according to Microsoft's sdkddkver.h */
#define NTDDI_VERSION 0x05010000	/* according to Microsoft's sdkddkver.h */
#include <windows.h>
#include <commctrl.h>
#include <stdint.h>
#include <uxtheme.h>
#include <string.h>
#include <wchar.h>
#include <windowsx.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <stdarg.h>
#include <oleacc.h>

#include "table.h"

// forward declaration
struct tableAcc;

struct table {
	HWND hwnd;
	HWND header;
	HFONT font;
	intmax_t nColumns;
	int *columnTypes;
	intmax_t width;
	intmax_t headerHeight;
	intmax_t xOrigin;		// in logical units
	intmax_t hpagesize;		// in logical units
	intmax_t count;
	intmax_t yOrigin;		// in rows
	intmax_t vpagesize;		// in rows
	int hwheelCarry;
	int vwheelCarry;
	intmax_t selectedRow;
	intmax_t selectedColumn;
	HTHEME theme;
	int checkboxWidth;
	int checkboxHeight;
	BOOL checkboxMouseOverLast;
	LPARAM checkboxMouseOverLastPoint;
	BOOL checkboxMouseDown;
	intmax_t checkboxMouseDownRow;
	intmax_t checkboxMouseDownColumn;
	struct tableAcc *firstAcc;
};

typedef BOOL (*handlerfunc)(struct table *, UINT, WPARAM, LPARAM, LRESULT *);
#define HANDLER(name) BOOL name(struct table *t, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult)

// debug.c
extern HRESULT logLastError(const char *);
extern HRESULT logHRESULT(const char *, HRESULT);
extern void logMemoryExhausted(const char *);

// alloc.c
extern void *tableAlloc(size_t);
extern void *tableRealloc(void *, size_t);
extern void tableFree(void *);

// util.c
extern BOOL runHandlers(const handlerfunc[], struct table *, UINT, WPARAM, LPARAM, LRESULT *);
extern BOOL runEventHandlers(const handlerfunc[], struct table *, UINT, WPARAM, LPARAM, LRESULT *);
extern HRESULT selectFont(struct table *, HDC, HFONT *, HFONT *);
extern HRESULT deselectFont(HDC, HFONT, HFONT);

// metrics.c
extern HRESULT columnWidth(struct table *, intmax_t, LONG *);
extern HRESULT textHeight(struct table *, HDC, BOOL, LONG *);
#define tableImageWidth() GetSystemMetrics(SM_CXSMICON)
#define tableImageHeight() GetSystemMetrics(SM_CYSMICON)
extern HRESULT rowHeight(struct table *, HDC, BOOL, LONG *);
#define rowht(t, p) rowHeight(t, NULL, TRUE, p)

// coord.c
struct rowcol {
	intmax_t row;
	intmax_t column;
};
extern DWORD clientCoordToRowColumn(struct table *, POINT, struct rowcol *);
extern DWORD lParamToRowColumn(struct table *, LPARAM, struct rowcol *);
extern DWORD rowColumnToClientRect(struct table *, struct rowcol, RECT *, BOOL *);
extern void toCellContentRect(struct table *, RECT *, LRESULT, intmax_t, intmax_t);
#define toCheckboxRect(t, r, xoff) toCellContentRect(t, r, xoff, t->checkboxWidth, t->checkboxHeight)

// scroll.c
struct scrollParams {
	intmax_t *pos;
	intmax_t pagesize;
	intmax_t length;
	intmax_t scale;
	DWORD (*post)(struct table *);
	int *wheelCarry;
};
extern HRESULT scrollto(struct table *, int, struct scrollParams *, intmax_t);
extern HRESULT scrollby(struct table *, int, struct scrollParams *, intmax_t);
extern HRESULT scroll(struct table *, int, struct scrollParams *, WPARAM, LPARAM);
extern HRESULT wheelscroll(struct table *, int, struct scrollParams *, WPARAM, LPARAM);

// vscroll.h
extern HRESULT vscrollto(struct table *, intmax_t);
extern HRESULT vscrollby(struct table *, intmax_t);
extern HRESULT vscroll(struct table *, WPARAM, LPARAM);
extern HRESULT vwheelscroll(struct table *, WPARAM, LPARAM);
extern HANDLER(vscrollHandler);

// hscroll.c
extern HRESULT hscrollto(struct table *, intmax_t);
extern HRESULT hscrollby(struct table *, intmax_t);
extern HRESULT hscroll(struct table *, WPARAM, LPARAM);
extern HANDLER(hscrollHandler);

// children.c
extern HANDLER(childrenHandlers);

// header.c
extern HRESULT makeHeader(struct table *, HINSTANCE);
extern HRESULT destroyHeader(struct table *);
extern HRESULT repositionHeader(struct table *);
extern HRESULT headerAddColumn(struct table *, WCHAR *);
extern HANDLER(headerNotifyHandler);

// events.c
extern HANDLER(mouseDownFocusHandler);
extern HANDLER(eventHandlers);

// select.c
extern DWORD doselect(struct table *, intmax_t, intmax_t);
extern HANDLER(mouseDownSelectHandler);
extern HANDLER(keyDownSelectHandler);

// checkboxes.c
enum {
	checkboxStateChecked = 1 << 0,
	checkboxStateHot = 1 << 1,
	checkboxStatePushed = 1 << 2,
	checkboxnStates = 1 << 3,
};
extern HRESULT drawCheckbox(struct table *, HDC, RECT *, int);
extern HRESULT freeCheckboxThemeData(struct table *);
extern HRESULT loadCheckboxThemeData(struct table *);
extern HANDLER(checkboxMouseMoveHandler);
extern HANDLER(checkboxMouseDownHandler);
extern HANDLER(checkboxMouseUpHandler);
extern HANDLER(checkboxCaptureChangedHandler);

// resize.c
extern HANDLER(resizeHandler);

// api.c
extern HANDLER(apiHandlers);
extern LRESULT notify(struct table *, UINT, intmax_t, intmax_t, uintptr_t);

// update.c
extern DWORD update(struct table *, BOOL);
extern DWORD updateAll(struct table *);

// draw.c
extern HANDLER(drawHandlers);
