// 17 february 2015

#ifndef __WINTABLE_INCLUDETHIS_H__
#define __WINTABLE_INCLUDETHIS_H__

#define tableWindowClass L"andlabs_wintable"

// start at WM_USER + 20 just in case for whatever reason we ever get the various dialog manager messages (see also http://blogs.msdn.com/b/oldnewthing/archive/2003/10/21/55384.aspx)
// each of these returns nothing unless otherwise indicated
enum {
	// wParam - 0
	// lParam - pointer to a tableColumn structure describing column
	// return - HRESULT error code cast to LRESULT; S_OK on success
	// (TODO later error on out of range columns...)
	// TODO indicate whether tableColumn is modified
	tableAddColumn = WM_USER + 20,
	// wParam - 0
	// lParam - pointer to tableModel to set model
	// return - HRESULT error code cast to LRESULT; S_OK on success
	tableSetModel,
	// wParam - 0
	// lParam - pointer to tableModelNotificationParams struct
	// no return (TODO?)
	tableModelNotify,
};

// notification codes
// note that these are positive; see http://blogs.msdn.com/b/oldnewthing/archive/2009/08/21/9877791.aspx
// each of these is of type tableNM
// all fields except data will always be set
// enum {
// };

typedef struct tableNM tableNM;

struct tableNM {
	NMHDR nmhdr;
	intmax_t row;
	intmax_t column;
	int columnType;
	uintptr_t data;
};

typedef struct tableColumn tableColumn;

struct tableColumn {
	// TODO change to headerText to avoid confusion? since the column isn't going to have a name to look up by (and we don't even save this copy of the name in our internal data structures; we'll need to change instances of name in api.c and header.c as well...
	WCHAR *name;
	intmax_t modelColumn;				// required; determines column type
	intmax_t bgcolorModelColumn;		// -1 for none
};

extern __declspec(dllexport) ATOM __stdcall tableInit(void);
extern __declspec(dllexport) HINSTANCE __stdcall tableHINSTANCE(void);

// TODO
#include "ModelPlan.h"
extern __declspec(dllexport) const IID IID_tableModel;

// table model helpers

typedef struct tableSubscriptions tableSubscriptions;
// TODO make these __stdcall
extern __declspec(dllexport) tableSubscriptions *tableNewSubscriptions(void);
extern __declspec(dllexport) void tableDeleteSubscriptions(tableSubscriptions *);
extern __declspec(dllexport) HRESULT tableSubscriptionsSubscribe(tableSubscriptions *, HWND);
extern __declspec(dllexport) HRESULT tableSubscriptionsUnsubscribe(tableSubscriptions *, HWND);
extern __declspec(dllexport) void tableSubscriptionsNotify(tableSubscriptions *, tableModelNotificationParams *);

extern __declspec(dllexport) HRESULT __stdcall tableDrawImageCell(HDC, HBITMAP, RECT *);

#endif
