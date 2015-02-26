// 7 january 2015

#include "includethis.h"

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

// TODO all forward declarations

// debug.c
extern DWORD panicLastError(const char *);
extern HRESULT panicHRESULT(const char *, HRESULT);
