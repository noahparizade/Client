//
// Created by spl211 on 29/12/2020.
//
#include "../include/socketReader.h"

using namespace std;
#include <iostream>
socketReader::socketReader(ConnectionHandler& handler, std::mutex& mutex):handler(handler), mutex(mutex) {}

void socketReader::run() {
    mutex.lock();
    while (!handler.getTerminate()) {
        std::string answer;
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!handler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        int len = answer.length();
        // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
        // we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
        answer.resize(len - 1);
        if (answer=="ACK 4")
            handler.setTerminate();
        if (answer=="ERROR 4")

        std::cout << answer << std::endl;
        if (answer == "bye") {
            std::cout << "Exiting...\n" << std::endl;

        }
    }
    mutex.unlock();
}