#! /bin/make

CFLAGS= -g -Wall

demo:
	g++ $(CFLAGS) -o fse main.cpp editor.cpp rawmode.cpp buffer.cpp
