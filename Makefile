
CC		= clang
CFLAGS		= -Wall
CPP		= clang++
CPPFLAGS	= -Wall -Wextra -O2 -std=c++11 -stdlib=libc++
INCLUDEDIRS	= -I/usr/local/include -I/usr/local/include/freetype2

all: swfexample

clean: clean-example clean-core

#.c.o:
#	$(CC) $(CFLAGS) $(INCLUDEDIRS) -c $*.c -o $*.o

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDEDIRS) -c $*.cpp -o $*.o

################################### example

EXAMPLELDIRS	= -L. -L/usr/lib -L/usr/local/lib
EXAMPLELIBS	= -lc++ -lswfcore -lxcb -lcurses -lSDL -lfreetype
EXAMPLESRCS	= \
	example/Example.cpp

swfexample: libswfcore.a $(EXAMPLESRCS:.cpp=.o)
	$(CPP) -o $@ $(EXAMPLESRCS:.cpp=.o) $(EXAMPLELDIRS) $(EXAMPLELIBS)

clean-example:
	rm -f swfexample $(EXAMPLESRCS:.cpp=.o)

################################### core

CORESRCS	= \
	core/Component.cpp \
	core/Container.cpp \
	core/DisplayCurses.cpp \
	core/DisplaySdl.cpp \
	core/DisplayXcb.cpp \
	core/Display.cpp \
	core/RootContainer.cpp \
	core/Widget.cpp
COREHDRS	= \
	core/Component.hpp \
	core/Container.hpp \
	core/DisplayCurses.hpp \
	core/DisplaySdl.hpp \
	core/DisplayXcb.hpp \
	core/Display.hpp \
	core/RootContainer.hpp \
	core/Widget.hpp

libswfcore.a: $(CORESRCS:.cpp=.o)
	ar -c -r $@ $(CORESRCS:.cpp=.o)

$(CORESRCS:.cpp=.o): $(COREHDRS)

clean-core:
	rm -f libswfcore.a $(CORESRCS:.cpp=.o)

