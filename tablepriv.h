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
	intmax_t hscrollpos;		// in logical units
	intmax_t hpagesize;		// in logical units
	intmax_t count;
	intmax_t vscrollpos;		// in rows
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

// TODO all forward declarations

// debug.c
extern DWORD panicLastError(const char *);
extern HRESULT panicHRESULT(const char *, HRESULT);

// alloc.c
extern void *tableAlloc(size_t);
extern void *tableRealloc(void *, size_t);
extern void tableFree(void *);

// util.c
extern BOOL runHandlers(const handlerfunc[], struct table *, UINT, WPARAM, LPARAM, LRESULT *);
extern BOOL runEventHandlers(const handlerfunc[], struct table *, UINT, WPARAM, LPARAM, LRESULT *);
extern DWORD selectFont(struct table *, HDC, HFONT *, HFONT *);
extern DWORD deselectFont(HDC, HFONT, HFONT);

// metrics.c
extern DWORD columnWidth(struct table *, intmax_t, LONG *);
extern DWORD textHeight(struct table *, HDC, BOOL, LONG *);
#define tableImageWidth() GetSystemMetrics(SM_CXSMICON)
#define tableImageHeight() GetSystemMetrics(SM_CYSMICON)
extern DWORD rowHeight(struct table *, HDC, BOOL, LONG *);
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
	void (*post)(struct table *);
	int *wheelCarry;
};
extern DWORD scrollto(struct table *, int, struct scrollParams *, intmax_t);
extern DWORD scrollby(struct table *, int, struct scrollParams *, intmax_t);
extern DWORD scroll(struct table *, int, struct scrollParams *, WPARAM, LPARAM);
extern DWORD wheelscroll(struct table *, int, struct scrollParams *, WPARAM, LPARAM);

// vscroll.h
extern DWORD vscrollto(struct table *, intmax_t);
extern DWORD vscrollby(struct table *, intmax_t);
extern DWORD vscroll(struct table *, WPARAM, LPARAM);
extern DWORD vwheelscroll(struct table *, WPARAM, LPARAM);
extern HANDLER(vscrollHandler);

// hscroll.c
extern DWORD hscrollto(struct table *, intmax_t);
extern DWORD hscrollby(struct table *, intmax_t);
extern DWORD hscroll(struct table *, WPARAM, LPARAM);
extern HANDLER(hscrollHandler);
