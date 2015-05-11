# TODOs so I don't forget:
# - make debugging an option
# - make 64 below an actual option
# - figure out why make test seems to rebuild the DLL
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

BASENAME = wintable
DLLFILE = $(OUTDIR)/$(BASENAME).dll
LIBFILE = $(OUTDIR)/$(BASENAME).lib
TESTEXEFILE = $(OUTDIR)/$(BASENAME).exe

CFILES = \
	acctable.c \
	alloc.c \
	api.c \
	checkboxdraw.c \
	checkboxevents.c \
	children.c \
	coord.c \
	debug.c \
	draw.c \
	enablefocus.c \
	events.c \
	header.c \
	hscroll.c \
	main.c \
	metrics.c \
	modelhelpers.c \
	modelnotify.c \
	nullmodel.c \
	resize.c \
	scroll.c \
	select.c \
	tooltip.c \
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

xCFLAGS = \
	--std=c99 \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	$(mflag) \
	$(CFLAGS)

xLDFLAGS = \
	-static-libgcc \
	-luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lole32 -loleaut32 -luiautomationcore -luuid -lmsimg32 \
	$(mflag) \
	$(LDFLAGS)

default:
	$(MAKE) clean
	$(MAKE) it
	$(MAKE) test

it: $(DLLFILE)

$(DLLFILE): $(OFILES)
	$(CC) -g -o $(DLLFILE) -shared -Wl,--out-implib,$(LIBFILE) $(OFILES) $(xLDFLAGS)

test: $(TESTEXEFILE)
# see http://stackoverflow.com/a/29021641/3408572
.PHONY: test

$(TESTEXEFILE): $(DLLFILE) $(TESTOFILES)
	$(CC) -g -o $(TESTEXEFILE) $(TESTOFILES) $(LIBFILE) $(xLDFLAGS)

$(OBJDIR)/%.o: %.c $(HFILES) | dirs
	$(CC) -g -o $@ -c $< $(xCFLAGS)

dirs:
	mkdir -p $(OBJDIR) $(OUTDIR)

clean:
	rm -rf $(OBJDIR) $(OUTDIR)
