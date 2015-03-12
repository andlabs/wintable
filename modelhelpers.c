// 6 march 2015
#include "tablepriv.h"

// TODO qsort/binsearch?
// TODO alert on not a table
// TODO invalid window handles (NULL, HWND_BROADCAST)

// tableSubscriptions is a type and some functions that you can use to implement tableModel::Subscribe(), tableModel::Unsubscribe(), and tableModel::Notify().

struct tableSubscriptions {
	HWND *hwnds;
	size_t len;
	size_t cap;
};

#define initialCapacity 32
#define capacityIncrement 32

__declspec(dllexport) tableSubscriptions *tableNewSubscriptions(void)
{
	tableSubscriptions *s;

	s = (tableSubscriptions *) tableAlloc(sizeof (tableSubscriptions));
	if (s == NULL)
		return NULL;
	s->len = 0;
	s->cap = initialCapacity;
	s->hwnds = (HWND *) tableAlloc(s->cap * sizeof (HWND));
	if (s->hwnds == NULL) {
		tableFree(s);
		return NULL;
	}
	return s;
}

__declspec(dllexport) void tableDeleteSubscriptions(tableSubscriptions *s)
{
	tableFree(s->hwnds);
	tableFree(s);
}

__declspec(dllexport) HRESULT tableSubscriptionsSubscribe(tableSubscriptions *s, HWND hwnd)
{
	HWND *hwnds;
	size_t cap;
	size_t i;

	if (hwnd == NULL)
		return E_INVALIDARG;
	for (i = 0; i < s->len; i++)
		if (hwnd == s->hwnds[i])
			return tableModelErrorTableAlreadySubscribed;
	if (s->len >= s->cap) {
		// grow linearly to conserve memory
		// there shouldn't be /too/ many subscribers anyway
		// TODO will this ensure s is not invalid on failure?
		cap = s->cap + capacityIncrement;
		hwnds = (HWND *) tableRealloc(s->hwnds, cap);
		if (hwnds == NULL)
			return E_OUTOFMEMORY;
		s->hwnds = hwnds;
		s->cap = cap;
	}
	s->len++;
	s->hwnds[s->len - 1] = hwnd;
	return S_OK;
}

__declspec(dllexport) HRESULT tableSubscriptionsUnsubscribe(tableSubscriptions *s, HWND hwnd)
{
	size_t i;

	if (hwnd == NULL)
		return E_INVALIDARG;
	for (i = 0; i < s->len; i++)
		if (s->hwnds[i] == hwnd) {
			s->len--;
			// TODO move higher hwnds back
			return S_OK;
		}
	return tableModelErrorTableNotSubscribed;
}

__declspec(dllexport) void tableSubscriptionsNotify(tableSubscriptions *s, tableModelNotificationParams *p)
{
	size_t i;

	for (i = 0; i < s->len; i++)
		SendMessageW(s->hwnds[i], tableModelNotify, 0, (LPARAM) p);
}

// this is an implementation of tableModel::tableDrawImageCell()
// TODO formally document
// TODO which argument first?
// TODO really panic on cleanup failure?
__declspec(dllexport) HRESULT __stdcall tableDrawImageCell(HDC dc, HBITMAP bitmap, RECT *r)
{
	BITMAP bi;
	HDC idc;
	HBITMAP previbitmap;
	BLENDFUNCTION bf;

	ZeroMemory(&bi, sizeof (BITMAP));
	if (GetObject(bitmap, sizeof (BITMAP), &bi) == 0)
		return logLastError("error getting Table cell image dimensions for drawing");
	// is it even possible to enforce the type of bitmap we need here based on the contents of the BITMAP (or even the DIBSECTION) structs?

	idc = CreateCompatibleDC(dc);
	if (idc == NULL)
		return logLastError("error creating compatible DC for Table image cell drawing");
	previbitmap = SelectObject(idc, bitmap);
	if (previbitmap == NULL)
		return logLastError("error selecting Table cell image into compatible DC for image drawing");

	ZeroMemory(&bf, sizeof (BLENDFUNCTION));
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;			// per-pixel alpha values
	bf.AlphaFormat = AC_SRC_ALPHA;
	if (AlphaBlend(dc, r->left, r->top, r->right - r->left, r->bottom - r->top,
		idc, 0, 0, bi.bmWidth, bi.bmHeight, bf) == FALSE)
		return logLastError("error drawing image into Table cell");

	if (SelectObject(idc, previbitmap) != bitmap)
		return logLastError("error deselecting Table cell image for drawing image");
	if (DeleteDC(idc) == 0)
		return logLastError("error deleting Table compatible DC for image cell drawing");

	return S_OK;
}
