// 4 december 2014

// TODO migrate

// each of these functions do an implicit ZeroMemory()
// these also make tableRealloc(NULL, ...)/tableFree(NULL) act like realloc(NULL, ...)/free(NULL) (that is, same as tableAlloc(...)/malloc(...) and a no-op, respectively)
// we /would/ use LocalAlloc() here because
// - whether the malloc() family supports the last-error code is undefined
// - the HeapAlloc() family DOES NOT support the last-error code; you're supposed to use Windows exceptions, and I can't find a clean way to do this with MinGW-w64 that doesn't rely on inline assembly or external libraries (unless they added __try/__except blocks)
// - there's no VirtualReAlloc() to complement VirtualAlloc() and I'm not sure if we can even get the original allocated size back out reliably to write it ourselves (http://blogs.msdn.com/b/oldnewthing/archive/2012/03/16/10283988.aspx)
// alas, LocalAlloc() doesn't want to work on real Windows 7 after a few times, throwing up ERROR_NOT_ENOUGH_MEMORY after three (3) ints or so :|
// we'll use malloc() until then
// needless to say, TODO

static void *tableAlloc(size_t size, const char *panicMessage)
{
//	HLOCAL out;
	void *out;

//	out = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, size);
	out = malloc(size);
	if (out == NULL)
		panic(panicMessage);
	ZeroMemory(out, size);
	return (void *) out;
}

static void *tableRealloc(void *p, size_t size, const char *panicMessage)
{
//	HLOCAL out;
	void *out;

	if (p == NULL)
		return tableAlloc(size, panicMessage);
//	out = LocalReAlloc((HLOCAL) p, size, LMEM_ZEROINIT);
	out = realloc(p, size);
	if (out == NULL)
		panic(panicMessage);
	// TODO zero the extra memory
	return (void *) out;
}

static void tableFree(void *p, const char *panicMessage)
{
	if (p == NULL)
		return;
//	if (LocalFree((HLOCAL) p) != NULL)
//		panic(panicMessage);
	free(p);
}
