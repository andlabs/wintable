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

	if (hwnd == NULL || hwnd == HWND_BROADCAST)
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

	if (hwnd == NULL || hwnd == HWND_BROADCAST)
		return E_INVALIDARG;
	for (i = 0; i < s->len; i++)
		if (s->hwnds[i] == hwnd) {
			s->len--;
			// TODO move higher hwnds back
			return S_OK;
		}
	return tableModelErrorTableNotSubscribed;
}

__declspec(dllexport) void tableSubscriptionsNotify(tableSubscriptions *s, int notification, intmax_t row, intmax_t column)
{
	struct modelNotification mn;

	mn.code = notification;
	mn.row = row;
	mn.column = column;
	for (i = 0; i < s->len; i++)
		SendMessageW(s->hwnds[i], tableModelNotify, 0, (LPARAM) (&mn));
}
