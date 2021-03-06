// 7 january 2015

#include "winapi.h"
#include "table.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO make this unnecessary here
struct rowcol {
	intmax_t row;
	intmax_t column;
};

struct table {
	HWND hwnd;
	HWND header;
	HINSTANCE hInstance;
	tableModel *model;		// TODO move to the end of the structure?
	HFONT font;
	intmax_t nColumns;
	tableColumn *columns;
	intmax_t width;
	intmax_t headerHeight;
	intmax_t xOrigin;		// in logical units
	intmax_t hpagesize;		// in logical units
	intmax_t yOrigin;		// in rows
	intmax_t vpagesize;		// in rows
	int hwheelCarry;
	int vwheelCarry;
	intmax_t selectedRow;
	intmax_t selectedColumn;
	HTHEME theme;

	BOOL mouseMoved;
	LPARAM mouseMoveLPARAM;
	BOOL lastMouseMoved;
	LPARAM lastMouseMoveLPARAM;

	BOOL checkboxMouseDown;
	struct rowcol checkboxMouseDownRowColumn;

	HWND tooltip;
	BOOL tooltipVisible;
	struct rowcol tooltipCurrentRowColumn;
	RECT tooltipAdjustedRect;

	void *tableAcc;
};

struct metrics;		// needed for below; TODO make this unnecessary

// timer IDs
// TODO move somewhere else in this file?
enum {
	tooltipTimer = 1,
};

// TODO move into the appropriate sections below
typedef BOOL (*handlerfunc)(struct table *, UINT, WPARAM, LPARAM, LRESULT *);
#define HANDLER(name) BOOL name(struct table *t, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult)
typedef BOOL (*eventhandlerfunc)(struct table *, struct metrics *, UINT, WPARAM, LPARAM);
#define EVENTHANDLER(name) BOOL name(struct table *t, struct metrics *m, UINT uMsg, WPARAM wParam, LPARAM lParam)

// debug.c
extern HRESULT logLastError(const char *);
extern HRESULT logHRESULT(const char *, HRESULT);
extern HRESULT logMemoryExhausted(const char *);

// alloc.c
extern void *tableAlloc(size_t);
extern void *tableRealloc(void *, size_t);
extern void tableFree(void *);

// util.c
extern BOOL runHandlers(const handlerfunc[], struct table *, UINT, WPARAM, LPARAM, LRESULT *);
extern BOOL runEventHandlers(const eventhandlerfunc[], struct table *, struct metrics *, UINT, WPARAM, LPARAM, LRESULT *, LRESULT);
extern HRESULT selectFont(struct table *, HDC, HFONT *, HFONT *);
extern HRESULT deselectFont(HDC, HFONT, HFONT);
extern BOOL lParamInRect(const RECT *, LPARAM);
extern HRESULT redrawCellAtLPARAM(struct table *, struct metrics *, LPARAM);

// metrics.c
struct metrics {
	RECT client;
	LONG rowHeight;
	LONG textHeight;
	int imageWidth;
	int imageHeight;
	int checkboxWidth;
	int checkboxHeight;
};
extern HRESULT columnWidth(struct table *, intmax_t, LONG *);
extern HRESULT getMetrics(struct table *, HDC, BOOL, struct metrics *);
#define metrics(t, m) getMetrics(t, NULL, TRUE, m)

// coord.c
extern HRESULT adjustRect(struct table *, struct metrics *, RECT *, intmax_t *);
extern HRESULT unadjustRect(struct table *, struct metrics *, RECT *);
extern HRESULT adjustPoint(struct table *, struct metrics *, POINT *);
// TODO struct rowcol should go here
extern HRESULT clientCoordToRowColumn(struct table *, struct metrics *, POINT, struct rowcol *);
extern HRESULT lParamToRowColumn(struct table *, struct metrics *, LPARAM, struct rowcol *);
extern HRESULT rowColumnToClientRect(struct table *, struct metrics *, struct rowcol, RECT *);
extern void toCellContentRect(struct table *, RECT *, LRESULT, intmax_t, intmax_t);
extern BOOL rowcolEqual(struct rowcol, struct rowcol);

// scroll.c
struct scrollParams {
	intmax_t *pos;
	intmax_t pagesize;
	intmax_t length;
	intmax_t scale;
	HRESULT (*post)(struct table *);
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
extern BOOL mouseHovering(struct table *);
extern HANDLER(eventHandlers);

// select.c
extern HRESULT doselect(struct table *, struct metrics *, intmax_t, intmax_t);
extern EVENTHANDLER(mouseDownSelectHandler);
extern EVENTHANDLER(keyDownSelectHandler);

// checkboxdraw.c
enum {
	checkboxStateChecked = 1 << 0,
	checkboxStateHot = 1 << 1,
	checkboxStatePushed = 1 << 2,
	checkboxStateDisabled = 1 << 3,
	checkboxnStates = 1 << 4,
};
extern HRESULT drawCheckbox(struct table *, HDC, RECT *, int);
extern HRESULT getCheckboxSize(struct table *, HDC, int *, int *);
extern HRESULT freeCheckboxThemeData(struct table *);
extern HRESULT loadCheckboxThemeData(struct table *);

// checkboxevents.c
extern EVENTHANDLER(checkboxMouseMoveHandler);
extern EVENTHANDLER(checkboxMouseDownHandler);
extern EVENTHANDLER(checkboxMouseUpHandler);
extern EVENTHANDLER(checkboxCaptureChangedHandler);

// resize.c
extern HANDLER(resizeHandler);

// api.c
extern HANDLER(apiHandlers);
extern LRESULT notify(struct table *, UINT, intmax_t, intmax_t, uintptr_t);

// update.c
extern HRESULT update(struct table *, BOOL);
extern HRESULT updateAll(struct table *);

// draw.c
extern HANDLER(drawHandlers);

// visibility.c
extern intmax_t firstVisible(struct table *);
extern HRESULT ensureVisible(struct table *, struct metrics *, struct rowcol);
extern HRESULT queueRedrawRow(struct table *, struct metrics *, intmax_t);

// nullmodel.c
extern tableModel *nullModel;

// modelnotify.c
extern HANDLER(modelNotificationHandler);

// enablefocus.c
extern EVENTHANDLER(mouseDownFocusHandler);
extern HANDLER(enableFocusHandlers);

// tooltips.c
extern HRESULT popTooltip(struct table *, BOOL);
extern EVENTHANDLER(tooltipMouseMoveHandler);
extern EVENTHANDLER(tooltipMouseLeaveHandler);
extern EVENTHANDLER(tooltipTimerHandler);
extern HANDLER(tooltipNotifyHandler);

// acctable.cpp
extern void initTableAcc(struct table *);
extern void uninitTableAcc(struct table *);
extern HANDLER(accessibilityHandler);

#ifdef __cplusplus
}
#endif
