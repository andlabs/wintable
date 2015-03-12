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
	nullmodel.c \
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

# TODO get rid of IDL baggage
OFILES = $(CFILES:%.c=$(OBJDIR)/%.o) $(OBJDIR)/ModelPlan_i.o
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
	-luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lole32 -loleaut32 -loleacc -luuid -lmsimg32 \
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

$(TESTEXEFILE): $(DLLFILE) $(TESTOFILES)
	$(CC) -g -o $(TESTEXEFILE) $(TESTOFILES) $(LIBFILE) $(xLDFLAGS)

$(OBJDIR)/%.o: %.c $(HFILES) dirs
	$(CC) -g -o $@ -c $< $(xCFLAGS)

# TODO
$(OBJDIR)/ModelPlan_i.o: idlout/ModelPlan_i.c $(HFILES) dirs
	$(CC) -g -o $@ -c $< $(xCFLAGS)
#$(OBJDIR)/ModelPlan_p.o: idlout/ModelPlan_p.c $(HFILES) dirs
#	$(CC) -g -o $@ -c $< $(xCFLAGS)
#$(OBJDIR)/dlldata.o: idlout/dlldata.c $(HFILES) dirs
#	$(CC) -g -o $@ -c $< $(xCFLAGS)

dirs:
	mkdir -p $(OBJDIR) $(OUTDIR)

clean:
	rm -rf $(OBJDIR) $(OUTDIR)
