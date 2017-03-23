SHELL=cmd.exe
USE_DEBUG = NO

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -ggdb -O
LFLAGS=-mwindows
else
CFLAGS=-Wall -O2
LFLAGS=-s -mwindows
endif
CFLAGS += -Wno-write-strings

CPPSRC=ClearIconTray.cpp ClearIcon.cpp systray.cpp hyperlinks.cpp about.cpp config.cpp \
common_funcs.cpp winmsgs.cpp

OBJS = $(CPPSRC:.cpp=.o) rc.o

BIN=ClearIconTray

%.o: %.cpp
	g++ $(CFLAGS) -c $<

#**************************************************************
#  generic build rules
#**************************************************************
all: $(BIN).exe

clean:
	rm -f $(BIN).exe *.o *.bak *.zip

depend:
	makedepend $(CPPSRC)

wc:
	wc -l *.cpp *.rc

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp *.rc $(CPPSRC)"

#**************************************************************
#  build rules for executables                           
#**************************************************************
$(BIN).exe: $(OBJS)
	g++ $(CFLAGS) $(OBJS) $(LFLAGS) -o $@ -lcomctl32 -liphlpapi -lpdh

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
rc.o: $(BIN).rc
	windres -O COFF $^ -o $@

# DO NOT DELETE

ClearIconTray.o: resource.h common.h systray.h ClearIconTray.h winmsgs.h
ClearIcon.o: common.h ClearIconTray.h
systray.o: resource.h common.h systray.h ClearIconTray.h
hyperlinks.o: hyperlinks.h
about.o: resource.h hyperlinks.h
config.o: common.h ClearIconTray.h
common_funcs.o: common.h
