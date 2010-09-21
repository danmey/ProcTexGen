texgen: main.o perlin.o tga.o tga.o smath.o parser2.o bmp.o
	g++ -o texgen /usr/lib/libSDL.so /usr/lib/libSDL_image.so /usr/lib/libGL.so /usr/lib/libGLU.so perlin.o tga.o main.o smath.o parser2.o bmp.o

main.o: main.cpp
	g++ -c -o main.o main.cpp

tga.o: tga.c tga.h
	g++ -c -o tga.o tga.c

perlin.o: perlin.cxx perlin.h 
	g++ -c -o perlin.o perlin.cxx

ui: ui.ml
	ocamlc -I +lablgtk2 -o ui lablgtk.cma gtkInit.cmo ui.ml

smath.o: smath.cpp
	g++ -c -o smath.o smath.cpp

parser.o: parser.cxx
	g++ -c -o parser.o parser.cxx

parser2.o: parser2.cxx perlin.h
	g++ -c -o parser2.o parser2.cxx

bmp.o: bmp.cxx bmp.h
	g++ -c -o bmp.o bmp.cxx