// 4 december 2014
#include "tablepriv.h"

// wrappers for allocator of choice
// return NULL on memory exhausted, undefined on heap corruption or other unreliably-detected malady (see http://stackoverflow.com/questions/28761680/is-there-a-windows-api-memory-allocator-deallocator-i-can-use-that-will-just-giv)
// new memory is set to zero
// passing NULL to tableRealloc() acts like tableAlloc()
// passing NULL to tableFree() is a no-op

void *tableAlloc(size_t size)
{
	void *out;

	out = malloc(size);
	if (out != NULL)
		ZeroMemory(out, size);
	return out;
}

void *tableRealloc(void *p, size_t size)
{
	void *out;

	if (p == NULL)
		return tableAlloc(size);
	out = realloc(p, size);
	// TODO zero the extra memory
	return out;
}

void tableFree(void *p)
{
	if (p == NULL)
		return;
	free(p);
}
