// 27 february 2015

// TODO merge

// UUIDs were generated with uuidgen from util-linux 2.25.1 on Ubuntu GNOME 14.10 on 6 March 2015

// TODO make sure all parameter attributes are correct

// table model column types
// TODO define behavior on a table if invalid types are used for column data
enum {
	tableModelColumnInvalid,		// for errors; should not be used in a Table
	tableModelColumnString,			// shown as text in a Table
	tableModelColumnImage,		// shown as an image in a Table
	tableModelColumnBool,			// shown as a checkbox in a Table
	tableModelColumnARGBColor,		// not shown in a Table; can be used for cell background colors
};

// tableARGBColor is an explicit type name for an ARGB color.
// Each color component is 8 bits wide, with A in the most significant bits, then R, then G, and then B in the least significant bits.
// Colors are assumed to be alpha-premultiplied.
// tableARGB() and tableRGBA() produce tableARGBColors from the given component values, specified in the order in the macros.
typedef uint32_t tableARGBColor;
// TODO
#define tableARGB(a, r, g, b) ((tableARGBColor) (a) << 24) | ((tableARGBColor) (r) << 16) | ((tableARGBColor) (b) << 8) | (tableARGBColor) (b))
#define tableRGBA(r, g, b, a) tableARGB((a), (r), (g), (b))

// tableModel::tableNotify() notifications
enum {
	// row - index of added row
	// column - ignored
	tableModelNotifyRowAdded,
	// row - index of deleted row
	// column - ignored
	tableModelNotifyRowDeleted,
	// row - index of changed row
	// column - index of changed column
	tableModelNotifyCellChanged,
};

typedef struct tableModelNotificationParams tableModelNotificationParams;
typedef struct tableCellValue tableCellValue;

struct tableModelNotificationParams {
	int code;
	intmax_t row;
	intmax_t column;
};

struct tableCellValue {
	int type;
	union {
		BSTR stringVal;
		BOOL boolVal;
		tableARGBColor color;
	};
};

// tableModel errors
#define tableModelErrorTableAlreadySubscribed MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200)
#define tableModelErrorTableNotSubscribed MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x201)
#define tableModelErrorWrongColumnType MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x202)
#define tableModelErrorColumnNotMutable MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x203)

// tableModel is the interface that any COM object that wishes
// to provide data to a Table must implement. The methods of
// tableModel are specifically intended to drive a Table; they are
// not intended to be (and indeed, cannot be used to) provide
// data to the model in any other context.
// 
// Table itself assumes the model is an in-process, STA object.
#undef INTERFACE
#define INTERFACE tableModel
DECLARE_INTERFACE_IID_(tableModel, IUnknown, 8f361d46-caab-489f-8d20-aeaaeaa9104f) {
	BEGIN_INTERFACE

	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// tableSubscribe() adds the Table with the given window
	// handle to a list of Tables to notify of any changes to the
	// tableModel. It is called by the Table itself when you issue
	// a tableSetModel message; you do not need to invoke
	// this method directly yourself.
	// 
	// It should neither take a reference to the tableModel nor
	// assume that one has been taken beforehand/will be taken
	// afterward; the Table will do that itself.
	// 
	// It should return S_OK on success or a COM error code
	// on failure; in particular, it should return:
	// - E_INVALIDARG if hwnd is NULL
	// - E_OUTOFMEMORY if a memory allocation failed
	// - tableModelErrorTableAlreadySubscribed if the given
	//     Table is already subscribed
	// 
	// You may use the tableSubscribtions system provided by
	// this DLL to implement this method for you. These functions
	// perform all necessary error checking; you can implement
	// your tableSubscribe() by merely calling this function.
	// TODO should this check if hwnd is actually a Table?
	STDMETHOD(tableSubscribe)(THIS_
		HWND hwnd
	) PURE;

	// tableUnsubscribe() removes the Table with the given
	// window handle from a list of Tables to notify of any
	// changes to the tableModel. It is called by the Table itself
	// when you issue a tableSetModel message or when the
	// Table is destroyed; you do not need to invoke this method
	// directly yourself.
	// 
	// It should neither release a reference to the tableModel nor
	// assume that one has been released beforehand/will be
	// released afterward; the Table will do that itself.
	// 
	// It should return S_OK on success or a COM error code
	// on failure; in particular, it should return:
	// - E_INVALIDARG if hwnd is NULL
	// - tableModelErrorTableNotSubscribed if the given Table is
	//     not subscribed
	// 
	// You may use the tableSubscribtions system provided by
	// this DLL to implement this method for you. These functions
	// perform all necessary error checking; you can implement
	// your tableUnsubscribe() by merely calling this function.
	STDMETHOD(tableUnsubscribe)(THIS_
		HWND hwnd
	) PURE;

	// tableNotify() notifies all the Tables currently subscribed
	// to the tableModel that something about the tableModel
	// has changed. It should send a tableModelNotification
	// message to each Table with the LPARAM set to the given
	// tableModelNotificationParams pointer. If no Tables are
	// currently subscribed, this function does nothing.
	// 
	// You may use the tableSubscribtions system provided by
	// this DLL to implement this method for you.
	// TODO return an error?
	STDMETHOD_(void, tableNotify)(THIS_
		tableModelNotificationParams *p
	) PURE;

	// tableColumnCount() returns the number of columns in
	// the tableModel. This value should be a constant; it should
	// never change after object creation.
	// (TODO would this go against CoCreateInstance()? maybe "after the first subscription" instead?)
	STDMETHOD_(intmax_t, tableColumnCount)(THIS) PURE;

	// tableColumnType() returns the data type for a given column.
	// This should be one of the tableColumnXxxx constants.
	// It should return S_OK on success or a COM error code
	// on failure; in particular, it should return:
	// - E_POINTER if colType is NULL
	// - E_INVALIDARG if column is out of range
	// On failure, *colType is set to tableModelColumnInvalid;
	// on success, *colType is set to anything else.
	// The value of *colType on success should be a constant;
	// it should never change after object creation.
	// (TODO would this go against CoCreateInstance()? maybe "after the first subscription" instead?)
	STDMETHOD(tableColumnType)(THIS_
		intmax_t column,
		int *colType
	) PURE;

	// tableRowCount() returns the number of rows in the tableModel.
	// TODO HRESULT?
	STDMETHOD_(intmax_t, tableRowCount)(THIS) PURE;

	// tableCellValue() returns the value of a given tableModel cell.
	// It should return S_OK on success or a COM error code
	// on failure; in particular, it should return:
	// - E_POINTER if value is NULL
	// - E_INVALIDARG if row or column is out of range
	// On failure, value->type will be set to tableModelColumnInvalid.
	// On success, value->type will be set to the column
	// type and the appropriate field for that type will be
	// set to the actual value.
	// TODO image cells
	STDMETHOD(tableCellValue)(THIS_
		intmax_t row,
		intmax_t column,
		tableCellValue *value
	) PURE;

	// tableDrawImageCell() draws the image belonging to the
	// given cell into the given HDC. The image shall be resized
	// to fit the given rectangle.
	// 
	// It should return S_OK on success or a COM error code
	// on failure; in particular, it should return:
	// - E_INVALIDARG if row or column is out of range
	//     or if the given HDC is NULL
	// - E_POINTER if the given destination rectangle is NULL
	// - tableModelErrorWrongColumnType if the given column
	//     is not of type tableModelColumnImage
	// - HRESULT_FROM_WIN32(GetLastError()) if a Windows
	//     API function that returns a last error code fails
	// 
	// You may use the tableDrawImageCell() function provided by
	// this DLL to implement this method for you. Be aware that
	// this function expects images to be 32-bit alpha-premultiplied
	// ARGB bitmaps (TODO DIB setions?) as expected by the
	// Windows API AlphaBlend() function. (TODO what errors are already provided?)
	STDMETHOD(tableDrawImageCell)(THIS_
		intmax_t row,
		intmax_t column,
		HDC hdc,
		RECT *rDest
	) PURE;

	// tableIsColumnMutable returns S_OK if the given column
	// is mutable, S_FALSE if not, and E_INVALIDARG if an invalid
	// column number is specified. The value returned on success
	// should be a constant; it should never change after object
	// creation.
	// TODO really not just a simple bool?
	STDMETHOD(tableIsColumnMutable)(THIS_
		intptr_t column
	) PURE;

	// TODO document
	STDMETHOD(tableSetCellValue)(THIS_
		intmax_t row,
		intmax_t column,
		tableCellValue data
	) PURE;

	// TODO document
	STDMETHOD(tableCellToggleBool)(THIS_
		intmax_t row,
		intmax_t column
	) PURE;

	END_INTERFACE
};

// TODO this is straight from the MIDL output; do a proper migration
#ifdef COBJMACROS


#define tableModel_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define tableModel_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define tableModel_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define tableModel_tableSubscribe(This,hwnd)	\
    ( (This)->lpVtbl -> tableSubscribe(This,hwnd) ) 

#define tableModel_tableUnsubscribe(This,hwnd)	\
    ( (This)->lpVtbl -> tableUnsubscribe(This,hwnd) ) 

#define tableModel_tableNotify(This,p)	\
    ( (This)->lpVtbl -> tableNotify(This,p) ) 

#define tableModel_tableColumnCount(This)	\
    ( (This)->lpVtbl -> tableColumnCount(This) ) 

#define tableModel_tableColumnType(This,column,colType)	\
    ( (This)->lpVtbl -> tableColumnType(This,column,colType) ) 

#define tableModel_tableRowCount(This)	\
    ( (This)->lpVtbl -> tableRowCount(This) ) 

#define tableModel_tableCellValue(This,row,column,value)	\
    ( (This)->lpVtbl -> tableCellValue(This,row,column,value) ) 

#define tableModel_tableDrawImageCell(This,row,column,hdc,rDest)	\
    ( (This)->lpVtbl -> tableDrawImageCell(This,row,column,hdc,rDest) ) 

#define tableModel_tableIsColumnMutable(This,column)	\
    ( (This)->lpVtbl -> tableIsColumnMutable(This,column) ) 

#define tableModel_tableSetCellValue(This,row,column,data)	\
    ( (This)->lpVtbl -> tableSetCellValue(This,row,column,data) ) 

#define tableModel_tableCellToggleBool(This,row,column)	\
    ( (This)->lpVtbl -> tableCellToggleBool(This,row,column) ) 

#endif /* COBJMACROS */

// TODO trees?
// TODO E_HANDLE for NULL HWNDs and HDCs instead of E_POINTER or E_INVALIDARG?
// TODO invalid windows other than NULL?
// TODO allow methods to return any other errors where appropriate
// TODO forbid E_NOTIMPL?
// TODO dc or hdc?
// TODO tableModelNotification or tableModelNotify?
