CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system

all: BGRSclient
	g++ -o bin/BGRSclient bin/connectionHandler.o bin/Client.o $(LDFLAGS)

BGRSclient: bin/connectionHandler.o bin/Client.o
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) -o bin/Client.o src/Client.cpp
	
.PHONY: clean
clean:
	rm -f bin/*
