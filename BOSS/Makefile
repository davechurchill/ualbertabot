CC=g++
SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=-O3 $(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL_image $(SDL_LDFLAGS)
INCLUDES=-Isource/rapidjson -Isource/
SOURCES=$(wildcard source/*.cpp) 
OBJECTS=$(SOURCES:.cpp=.o)

all:BOSS 

BOSS:$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@