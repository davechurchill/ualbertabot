CC=g++
SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=-O3 $(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL_image $(SDL_LDFLAGS)
INCLUDES=-Ibwapidata/include -Isource/glfont
SOURCES=$(wildcard bwapidata/include/*.cpp) $(wildcard source/*.cpp) 
GLFONTSOURCE=$(wildcard source/glfont/*.cc)
OBJECTS=$(GLFONTSOURCE:.cc=.o) $(SOURCES:.cpp=.o)

all:SparCraft 

SparCraft:$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
