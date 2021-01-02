#include <stdlib.h>
#include "../include/connectionHandler.h"
#include <iostream>
#include "../include/keyboardReader.h"
#include "../include/socketReader.h"
#include <thread>
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) {
    if (argc < 3) {
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        return 1;
    }

    keyboardReader keyReader(connectionHandler);
    socketReader sockReader(connectionHandler);
    std::thread thread2(&socketReader::run,&sockReader);
    keyReader.run();
    thread2.join();
    return 0;
}
