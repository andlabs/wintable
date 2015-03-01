// 25 february 2015
#include "tablepriv.h"

// uncomment the following line to enable debug messages
#define tableDebug
// uncomment the following line to halt on a debug message
#define tableDebugStop

#ifdef tableDebug

#include <stdio.h>

DWORD logLastError(const char *context)
{
	DWORD le;
	WCHAR *msg;
	int parenthesize = 0;

	le = GetLastError();
	fprintf(stderr, "%s: ", context);
	if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, le, 0, (LPWSTR) (&msg), 0, NULL) != 0) {
		fprintf(stderr, "%S (", msg);
		// TODO check error
		LocalFree(msg);
		parenthesize = 1;
	}
	fprintf(stderr, "GetLastError() == %I32u", le);
	if (parenthesize)
		fprintf(stderr, ")");
	fprintf(stderr, "\n");
#ifdef tableDebugStop
	DebugBreak();
#endif
	SetLastError(le);
	return le;
}

HRESULT logHRESULT(const char *context, HRESULT hr)
{
	WCHAR *msg;
	int parenthesize = 0;

	fprintf(stderr, "%s: ", context);
	// this isn't technically documented, but everyone does it, including Microsoft (see the implementation of _com_error::ErrorMessage() in a copy of comdef.h that comes with the Windows DDK)
	if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD) hr, 0, (LPWSTR) (&msg), 0, NULL) != 0) {
		fprintf(stderr, "%S (", msg);
		// TODO check error
		LocalFree(msg);
		parenthesize = 1;
	}
	fprintf(stderr, "HRESULT == 0x%I32X", hr);
	if (parenthesize)
		fprintf(stderr, ")");
	fprintf(stderr, "\n");
#ifdef tableDebugStop
	DebugBreak();
#endif
	return hr;
}

void logMemoryExhausted(const char *reason)
{
	fprintf(stderr, "memory exhausted %s\n", reason);
#ifdef tableDebugStop
	DebugBreak();
#endif
}

#else

DWORD logLastError(const char *reason)
{
	DWORD le;

	// technically (I think? TODO) we don't need to do this, but let's do this anyway just to be safe
	le = GetLastError();
	SetLastError(le);
	return le;
}

HRESULT logHRESULT(const char *reason, HRESULT hr)
{
	return hr;
}

void logMemoryExhausted(const char *reason)
{
}

#endif
