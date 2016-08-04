CC=g++
BWAPI_DIR=/home/dave/facebook/bwapi/bwapi
SDL_LDFLAGS=`sdl2-config --libs` 
SDL_CFLAGS=`sdl2-config --cflags` 
CFLAGS=-O3 -std=c++11 $(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL2_image $(SDL_LDFLAGS)
INCLUDES=-I$(BWAPI_DIR)/include -I$(BWAPI_DIR)/include/BWAPI -I$(BWAPI_DIR)
SOURCES=$(wildcard $(BWAPI_DIR)/BWAPILIB/Source/*.cpp) $(wildcard $(BWAPI_DIR)/BWAPILIB/*.cpp) $(wildcard source/*.cpp) $(wildcard source/main/*.cpp) $(wildcard source/gui/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all:SparCraft 

SparCraft:$(OBJECTS) 
	$(CC) $(OBJECTS) -o bin/$@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
