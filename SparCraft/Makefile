CC=g++
BWAPI_DIR=/home/dave/facebook/bwapi/bwapi
SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=-O3 -std=c++11 $(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL_image $(SDL_LDFLAGS)
INCLUDES=-I$(BWAPI_DIR)/include -I$(BWAPI_DIR)/include/BWAPI -I$(BWAPI_DIR)
SOURCES=$(wildcard $(BWAPI_DIR)/BWAPILIB/Source/*.cpp) $(wildcard $(BWAPI_DIR)/BWAPILIB/*.cpp) $(wildcard source/*.cpp) source/main/main.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all:SparCraft 

SparCraft:$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
