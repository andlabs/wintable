

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Mar 12 16:57:12 2015
 */
/* Compiler settings for ModelPlan.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ModelPlan_h__
#define __ModelPlan_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __tableModel_FWD_DEFINED__
#define __tableModel_FWD_DEFINED__
typedef interface tableModel tableModel;

#endif 	/* __tableModel_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "stdint.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_ModelPlan_0000_0000 */
/* [local] */ 


enum __MIDL___MIDL_itf_ModelPlan_0000_0000_0001
    {
        tableModelColumnInvalid	= 0,
        tableModelColumnString	= ( tableModelColumnInvalid + 1 ) ,
        tableModelColumnImage	= ( tableModelColumnString + 1 ) ,
        tableModelColumnBool	= ( tableModelColumnImage + 1 ) ,
        tableModelColumnARGBColor	= ( tableModelColumnBool + 1 ) 
    } ;
typedef uint32_t tableARGBColor;

#define tableARGB(a, r, g, b) ((tableARGBColor) (a) << 24) | ((tableARGBColor) (r) << 16) | ((tableARGBColor) (b) << 8) | (tableARGBColor) (b))
#define tableRGBA(r, g, b, a) tableARGB((a), (r), (g), (b))

enum __MIDL___MIDL_itf_ModelPlan_0000_0000_0002
    {
        tableModelNotifyRowAdded	= 0,
        tableModelNotifyRowDeleted	= ( tableModelNotifyRowAdded + 1 ) ,
        tableModelNotifyCellChanged	= ( tableModelNotifyRowDeleted + 1 ) 
    } ;
typedef /* [public][public] */ struct __MIDL___MIDL_itf_ModelPlan_0000_0000_0003
    {
    int code;
    intmax_t row;
    intmax_t column;
    } 	tableModelNotificationParams;

typedef /* [public][public][public] */ struct __MIDL___MIDL_itf_ModelPlan_0000_0000_0004
    {
    int type;
    /* [switch_is] */ /* [switch_type] */ union __MIDL___MIDL_itf_ModelPlan_0000_0000_0006
        {
        /* [case()] */  /* Empty union arm */ 
        /* [case()] */ BSTR *stringVal;
        /* [case()] */  /* Empty union arm */ 
        /* [case()] */ BOOL boolVal;
        /* [case()] */ tableARGBColor color;
        } 	tagged_union;
    } 	tableCellValue;

#define tableModelErrorTableAlreadySubscribed MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200)
#define tableModelErrorTableNotSubscribed MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x201)
#define tableModelErrorWrongColumnType MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x202)


extern RPC_IF_HANDLE __MIDL_itf_ModelPlan_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ModelPlan_0000_0000_v0_0_s_ifspec;

#ifndef __tableModel_INTERFACE_DEFINED__
#define __tableModel_INTERFACE_DEFINED__

/* interface tableModel */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_tableModel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8f361d46-caab-489f-8d20-aeaaeaa9104f")
    tableModel : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE tableSubscribe( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableUnsubscribe( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual void STDMETHODCALLTYPE tableNotify( 
            /* [in] */ tableModelNotificationParams *p) = 0;
        
        virtual intmax_t STDMETHODCALLTYPE tableColumnCount( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableColumnType( 
            /* [in] */ intmax_t column,
            /* [retval][out] */ int *colType) = 0;
        
        virtual intmax_t STDMETHODCALLTYPE tableRowCount( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableCellValue( 
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column,
            /* [retval][out] */ tableCellValue *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableDrawImageCell( 
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column,
            /* [in] */ HDC hdc,
            /* [in] */ RECT *rDest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableIsColumnMutable( 
            /* [in] */ intptr_t column) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableSetCellValue( 
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column,
            /* [in] */ tableCellValue data) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE tableCellToggleBool( 
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct tableModelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            tableModel * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            tableModel * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            tableModel * This);
        
        HRESULT ( STDMETHODCALLTYPE *tableSubscribe )( 
            tableModel * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *tableUnsubscribe )( 
            tableModel * This,
            /* [in] */ HWND hwnd);
        
        void ( STDMETHODCALLTYPE *tableNotify )( 
            tableModel * This,
            /* [in] */ tableModelNotificationParams *p);
        
        intmax_t ( STDMETHODCALLTYPE *tableColumnCount )( 
            tableModel * This);
        
        HRESULT ( STDMETHODCALLTYPE *tableColumnType )( 
            tableModel * This,
            /* [in] */ intmax_t column,
            /* [retval][out] */ int *colType);
        
        intmax_t ( STDMETHODCALLTYPE *tableRowCount )( 
            tableModel * This);
        
        HRESULT ( STDMETHODCALLTYPE *tableCellValue )( 
            tableModel * This,
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column,
            /* [retval][out] */ tableCellValue *value);
        
        HRESULT ( STDMETHODCALLTYPE *tableDrawImageCell )( 
            tableModel * This,
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column,
            /* [in] */ HDC hdc,
            /* [in] */ RECT *rDest);
        
        HRESULT ( STDMETHODCALLTYPE *tableIsColumnMutable )( 
            tableModel * This,
            /* [in] */ intptr_t column);
        
        HRESULT ( STDMETHODCALLTYPE *tableSetCellValue )( 
            tableModel * This,
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column,
            /* [in] */ tableCellValue data);
        
        HRESULT ( STDMETHODCALLTYPE *tableCellToggleBool )( 
            tableModel * This,
            /* [in] */ intmax_t row,
            /* [in] */ intmax_t column);
        
        END_INTERFACE
    } tableModelVtbl;

    interface tableModel
    {
        CONST_VTBL struct tableModelVtbl *lpVtbl;
    };

    

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


#endif 	/* C style interface */




#endif 	/* __tableModel_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


