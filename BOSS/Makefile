CC=g++
CFLAGS=-O4 
LDFLAGS=
INCLUDES=-Isource/rapidjson source/
SOURCES=$(wildcard source*.cpp ) 
OBJECTS=$(SOURCES_SWC:.cpp=.o)

all:BOSS  

BOSS:$(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o bin/BOSS

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
