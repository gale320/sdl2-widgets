CC := g++
CFLAGS := -g $(shell sdl2-config --cflags) -std=c++11
LFLAGS := $(shell sdl2-config --libs) -lSDL2_ttf

.SUFFIXES=

hello: hello.o ../sdl-widgets.a
	$(CC) $^ -o $@ $(LFLAGS)

%.o: %.cpp
	$(CC) -c $< -I.. $(CFLAGS)

hello.o: ../sdl-widgets.h
