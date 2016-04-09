
CC		= clang
CFLAGS		= -Wall
CPP		= clang++
CPPFLAGS	= -O2 -Wall -Wextra -Wno-unused-parameter -std=c++14 -stdlib=libc++
#CPPFLAGS	= -g -O0 -Wall -Wextra -O2 -std=c++14 -stdlib=libc++
INCLUDEDIRS	= -I/usr/local/include -I/usr/local/include/freetype2

all: swfexample

clean: clean-example clean-lib

#.c.o:
#	$(CC) $(CFLAGS) $(INCLUDEDIRS) -c $*.c -o $*.o

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDEDIRS) -c $*.cpp -o $*.o

################################### example

# lib dependencies:
# windows sdl: Winmm.lib, dxguid.lib
# windows sdl2: Winmm.lib, Imm32.lib, version.lib

EXAMPLELDIRS	= -L. -L/usr/lib -L/usr/local/lib
EXAMPLELIBS	= -lc++ -lswf -lxcb -lxcb-keysyms -lcurses -lSDL2 -lfreetype
EXAMPLESRCS	= \
	example/Example.cpp

swfexample: libswf.a $(EXAMPLESRCS:.cpp=.o)
	$(CPP) -o $@ $(EXAMPLESRCS:.cpp=.o) $(EXAMPLELDIRS) $(EXAMPLELIBS)

clean-example:
	rm -f swfexample $(EXAMPLESRCS:.cpp=.o)

################################### lib

LIBSRCS	= \
	core/Button.cpp \
	core/Component.cpp \
	core/Container.cpp \
	core/ContainerList.cpp \
	core/Context.cpp \
	core/FrontendIn.cpp \
	core/FrontendOut.cpp \
	core/Widget.cpp \
	frontend/in/CursesIn.cpp \
	frontend/in/Sdl1In.cpp \
	frontend/in/Sdl2In.cpp \
	frontend/in/XcbIn.cpp \
	frontend/out/CursesOut.cpp \
	frontend/out/Sdl1Out.cpp \
	frontend/out/Sdl2Out.cpp \
	frontend/out/XcbOut.cpp

LIBHDRS	= \
	core/Button.hpp \
	core/Component.hpp \
	core/Container.hpp \
	core/ContainerList.hpp \
	core/Context.hpp \
	core/FrontendIn.hpp \
	core/FrontendOut.hpp \
	core/Widget.hpp \
	frontend/in/CursesIn.hpp \
	frontend/in/Sdl1In.hpp \
	frontend/in/Sdl2In.hpp \
	frontend/in/XcbIn.hpp \
	frontend/out/CursesOut.hpp \
	frontend/out/Sdl1Out.hpp \
	frontend/out/Sdl2Out.hpp \
	frontend/out/XcbOut.hpp

libswf.a: $(LIBSRCS:.cpp=.o)
	ar -c -r $@ $(LIBSRCS:.cpp=.o)

$(LIBSRCS:.cpp=.o): $(LIBHDRS)

clean-lib:
	rm -f *.core libswf.a $(LIBSRCS:.cpp=.o)

