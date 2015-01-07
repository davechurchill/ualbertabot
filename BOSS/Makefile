CC=g++
SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=-O3 $(SDL_CFLAGS) -std=c++11
LDFLAGS=-lGL -lGLU -lglut -lSDL_image $(SDL_LDFLAGS)
INCLUDES=-Isource/bwapidata/include -Isource/rapidjson -Isource/SDL -Isource/GL -Isource
SOURCES=$(wildcard source/bwapidata/include/*.cpp) $(wildcard source/*.cpp) 
OBJECTS=$(SOURCES:.cpp=.o)

all:BOSS 

BOSS:$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@