# TODOs so I don't forget:
# - make debugging an option
# - clean up compiler flag construction
# - __declspec(dllimport)

ifeq ($(MAKECMDGOALS),64)
	CC = x86_64-w64-mingw32-gcc
	RC = x86_64-w64-mingw32-windres
	mflag = -m64
else
	CC = i686-w64-mingw32-gcc
	RC = i686-w64-mingw32-windres
	mflag = -m32
endif

OBJDIR = .objs
OUTDIR = out

CFILES = \
	alloc.c \
	api.c \
	checkboxdraw.c \
	checkboxevents.c \
	children.c \
	coord.c \
	debug.c \
	draw.c \
	events.c \
	header.c \
	hscroll.c \
	main.c \
	metrics.c \
	resize.c \
	scroll.c \
	select.c \
	update.c \
	util.c \
	visibility.c \
	vscroll.c

HFILES = \
	table.h \
	tablepriv.h

TESTCFILES = \
	test.c

OFILES = $(CFILES:%.c=$(OBJDIR)/%.o)
TESTOFILES = $(TESTCFILES:%.c=$(OBJDIR)/%.o)

neededCFLAGS = --std=c99 -Wall -Wextra -Wno-unused-parameter

neededLDFLAGS = -static-libgcc -luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lole32 -loleaut32 -loleacc -luuid -lmsimg32

all: clean $(OFILES)
	$(CC) -g -o $(OUTDIR)/wintable.dll -shared -Wl,--out-implib,$(OUTDIR)/wintable.lib $(OFILES) $(LDFLAGS) $(neededLDFLAGS) $(mflag)

test: all $(TESTOFILES)
	$(CC) -g -o $(OUTDIR)/wintable.exe $(TESTOFILES) $(LDFLAGS) $(neededLDFLAGS) $(OUTDIR)/wintable.lib $(mflag)

$(OBJDIR)/%.o: %.c $(HFILES) dirs
	$(CC) -g -o $@ -c $< $(CFLAGS) $(neededCFLAGS) $(mflag)

dirs:
	mkdir -p $(OBJDIR) $(OUTDIR)

clean:
	rm -rf $(OBJDIR) $(OUTDIR)
