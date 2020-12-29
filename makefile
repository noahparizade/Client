CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude -lpthread
LDFLAGS:=-lboost_system -lpthread

all: BGRSclient
	g++ -o bin/BGRSclient bin/connectionHandler.o bin/Client.o bin/keyboardReader.o bin/socketReader.o $(LDFLAGS)

BGRSclient: bin/connectionHandler.o bin/Client.o bin/keyboardReader.o bin/socketReader.o
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) -o bin/Client.o src/Client.cpp

bin/keyboardReader.o: src/keyboardReader.cpp
	g++ $(CFLAGS) -o bin/keyboardReader.o src/keyboardReader.cpp

bin/socketReader.o: src/socketReader.cpp
	g++ $(CFLAGS) -o bin/socketReader.o src/socketReader.cpp
	
.PHONY: clean
clean:
	rm -f bin/*
