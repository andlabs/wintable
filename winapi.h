// 7 january 2015
// TODO prune the include list when finished
#define UNICODE
#define _UNICODE
#define STRICT
#define STRICT_TYPED_ITEMIDS
#define CINTERFACE
#define COBJMACROS
// see https://github.com/golang/go/issues/9916#issuecomment-74812211
//TODO#define INITGUID
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
