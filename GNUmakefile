# TODOs so I don't forget:
# - make debugging an option
# - add MinGW-w64 building when they add UI Automation and isolation awareness
# - __declspec(dllimport)
# - subsystem versions

OBJDIR = .objs
OUTDIR = out

BASENAME = wintable
OUT = $(OUTDIR)/$(BASENAME).dll

CFILES = \
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

CXXFILES += \
	acctable.cpp

HFILES = \
	table.h \
	tablepriv.h

OFILES = \
	$(CFILES:%.c=$(OBJDIR)/%.o) \
	$(CXXFILES:%.cpp=$(OBJDIR)/%.o)

# TODO /Wall does too much
# TODO -Wno-switch equivalent
# TODO /sdl turns C4996 into an ERROR
# TODO loads of warnings in the system header files
# TODO /analyze requires us to write annotations everywhere
CFLAGS += \
	/Zi \
	/W4 \
	/wd4100 \
	/TC \
	/bigobj /nologo \
	/RTC1 /RTCc /RTCs /RTCu \

# TODO fine tune this
CXXFLAGS += $(subst /TC,/TP,$(CFLAGS))

# TODO warnings on undefined symbols
LDFLAGS += \
	/debug \
	/largeaddressaware /nologo /incremental:no

DLLLIBS = \
	user32.lib kernel32.lib gdi32.lib comctl32.lib uxtheme.lib ole32.lib oleaut32.lib uiautomationcore.lib uuid.lib msimg32.lib

$(OUT): $(OFILES) | $(OUTDIR)
	@link /out:$(OUT) $(OFILES) /dll $(LDFLAGS) $(DLLLIBS)
	@echo ====== Linked $(OUT)

$(OBJDIR)/%.o: %.c $(HFILES) | $(OBJDIR)
	@cl /Fo:$@ /c $< $(CFLAGS) /Fd$@.pdb
	@echo ====== Compiled $<

$(OBJDIR)/%.o: %.cpp $(HFILES) | $(OBJDIR)
	@cl /Fo:$@ /c $< $(CXXFLAGS) /Fd$@.pdb
	@echo ====== Compiled $<

$(OBJDIR) $(OUTDIR):
	@mkdir $@

clean:
	rm -rf $(OBJDIR) $(OUTDIR)

test: $(OUT) test.c
	@cl /Fo:$(OBJDIR)/test.o /c $< $(CFLAGS) /Fd$(OBJDIR)/test.o.pdb
	@echo ====== Compiled test.c
	@link /out:$(OUTDIR)/test.exe $(OBJDIR)/test.o $(LDFLAGS) $(OUTDIR)/wintable.lib
	@echo ====== Linked test.exe
