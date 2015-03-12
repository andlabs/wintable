// 17 february 2015

#ifndef __WINTABLE_INCLUDETHIS_H__
#define __WINTABLE_INCLUDETHIS_H__

#define tableWindowClass L"andlabs_wintable"

// TODO
#include "idlout/ModelPlan.h"

// start at WM_USER + 20 just in case for whatever reason we ever get the various dialog manager messages (see also http://blogs.msdn.com/b/oldnewthing/archive/2003/10/21/55384.aspx)
// each of these returns nothing unless otherwise indicated
enum {
	// wParam - one of the type constants
	// lParam - column name as a Unicode string
	// TODO make return an HRESULT?
	tableAddColumn = WM_USER + 20,
	// wParam - 0
	// lParam - pointer to tableModel to set model
	// return - HRESULT error code cast to LRESULT; S_OK on success
	tableSetModel,
};

enum {
	tableColumnText,
	tableColumnImage,
	tableColumnCheckbox,
	nTableColumnTypes,
};

// notification codes
// note that these are positive; see http://blogs.msdn.com/b/oldnewthing/archive/2009/08/21/9877791.aspx
// each of these is of type tableNM
// all fields except data will always be set
enum {
	// data parameter is always 0
	// for tableColumnText return should be WCHAR *
	// for tableColumnImage return should be HBITMAP
	// for tableColumnCheckbox return is nonzero for checked, zero for unchecked
	tableNotificationGetCellData,
	// data parameter is pointer, same as tableNotificationGetCellData
	// not sent for checkboxes
	// no return
	tableNotificationFinishedWithCellData,
	// data is zero
	// no return
	tableNotificationCellCheckboxToggled,
	// sent even on deselection (in that case, row == -1 and column == -1)
	// data is zero
	// no return
	tableNotificationSelectionChanged,
};

typedef struct tableNM tableNM;

struct tableNM {
	NMHDR nmhdr;
	intmax_t row;
	intmax_t column;
	int columnType;
	uintptr_t data;
};

extern __declspec(dllexport) ATOM __stdcall tableInit(void);
extern __declspec(dllexport) HINSTANCE __stdcall tableHINSTANCE(void);

// table model helpers

typedef struct tableSubscriptions tableSubscriptions;
// TODO make these __stdcall
extern __declspec(dllexport) tableSubscriptions *tableNewSubscriptions(void);
extern __declspec(dllexport) void tableDeleteSubscriptions(tableSubscriptions *);
extern __declspec(dllexport) HRESULT tableSubscriptionsSubscribe(tableSubscriptions *, HWND);
extern __declspec(dllexport) HRESULT tableSubscriptionsUnsubscribe(tableSubscriptions *, HWND);
extern __declspec(dllexport) void tableSubscriptionsNotify(tableSubscriptions *, int, intmax_t, intmax_t);

#endif
