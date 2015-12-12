CC := g++
# CFLAGS :=-g -O0 $$(sdl2-config --cflags) -std=c++11 -Wno-multichar -fmax-errors=5
CFLAGS := -g -O $$(sdl2-config --cflags) -std=c++11 -Wall -Wuninitialized -Wshadow \
  -Wno-non-virtual-dtor -Wno-delete-non-virtual-dtor -Wno-multichar
LFLAGS := $$(sdl2-config --libs) -lSDL2_ttf
OBJ := shapes.o sdl-widgets.o testsw.o

.SUFFIXES=
.PHONY: all hello make-waves bouncy-tune archive

all: testsw hello make-waves bouncy-tune

%: %.o sdl-widgets.a 
	$(CC) $< sdl-widgets.a -o $@ $(LFLAGS)

%.o: %.cpp
	$(CC) -c $< $(CFLAGS)

archive: sdl-widgets.a

sdl-widgets.a: sdl-widgets.o shapes.o mingw32-specific.o
	ar -rs $@ sdl-widgets.o shapes.o mingw32-specific.o

hello make-waves bouncy-tune:
	make -C $@

clean:
	rm *.o sdl-widgets.a */*.o

sdl-widgets.o: sdl-widgets.h sw-pixmaps.h config.h shapes.h
testsw.o: sdl-widgets.h shapes.h
