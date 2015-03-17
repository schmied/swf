
CC		= clang
CFLAGS		= -Wall
CPP		= clang++
CPPFLAGS	= -O2 -Wall -Wextra -std=c++14 -stdlib=libc++
#CPPFLAGS	= -g -O0 -Wall -Wextra -O2 -std=c++14 -stdlib=libc++
INCLUDEDIRS	= -I/usr/local/include -I/usr/local/include/freetype2

all: swfexample

clean: clean-example clean-core

#.c.o:
#	$(CC) $(CFLAGS) $(INCLUDEDIRS) -c $*.c -o $*.o

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDEDIRS) -c $*.cpp -o $*.o

################################### example

# lib dependencies:
# windows sdl: Winmm.lib, dxguid.lib
# windows sdl2: Imm32.lib, version.lib

EXAMPLELDIRS	= -L. -L/usr/lib -L/usr/local/lib
EXAMPLELIBS	= -lc++ -lswfcore -lxcb -lxcb-keysyms -lcurses -lSDL2 -lfreetype
EXAMPLESRCS	= \
	example/Example.cpp

swfexample: libswfcore.a $(EXAMPLESRCS:.cpp=.o)
	$(CPP) -o $@ $(EXAMPLESRCS:.cpp=.o) $(EXAMPLELDIRS) $(EXAMPLELIBS)

clean-example:
	rm -f swfexample $(EXAMPLESRCS:.cpp=.o)

################################### core

CORESRCS	= \
	core/Button.cpp \
	core/Component.cpp \
	core/Container.cpp \
	core/ContainerList.cpp \
	core/Context.cpp \
	core/Display.cpp \
	core/DisplayCurses.cpp \
	core/DisplaySdl.cpp \
	core/DisplaySdl2.cpp \
	core/DisplayXcb.cpp \
	core/Widget.cpp

COREHDRS	= \
	core/Button.hpp \
	core/Component.hpp \
	core/Container.hpp \
	core/ContainerList.hpp \
	core/Context.hpp \
	core/Display.hpp \
	core/DisplayCurses.hpp \
	core/DisplaySdl.hpp \
	core/DisplaySdl2.hpp \
	core/DisplayXcb.hpp \
	core/Widget.hpp

libswfcore.a: $(CORESRCS:.cpp=.o)
	ar -c -r $@ $(CORESRCS:.cpp=.o)

$(CORESRCS:.cpp=.o): $(COREHDRS)

clean-core:
	rm -f *.core libswfcore.a $(CORESRCS:.cpp=.o)

