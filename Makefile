#! /bin/make

CFLAGS= -g -Wall
CFLAGS += `pkg-config sdl2 --cflags`
CFLAGS += `pkg-config SDL2_ttf --cflags`
CFLAGS += `pkg-config --cflags fontconfig`
LIBS = `pkg-config sdl2 --libs`
LIBS += `pkg-config SDL2_ttf --libs`
LIBS += `pkg-config --libs fontconfig`
OBJS = sdlview.o sdlfont.o view.o

edt: program.o $(OBJS)
	g++ $(CFLAGS) $< $(OBJS) $(LIBS)

demo:
	g++ $(CFLAGS) -o fse main.cpp editor.cpp rawmode.cpp buffer.cpp

clean:
	rm *.o

.cpp.o:
	g++ -c $(CFLAGS) $< -o $@
