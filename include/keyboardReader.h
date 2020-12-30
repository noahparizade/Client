//
// Created by spl211 on 29/12/2020.
//

#ifndef CLIENT_KEYBOARDREADER_H
#define CLIENT_KEYBOARDREADER_H
#include "connectionHandler.h"
#include <mutex>
class keyboardReader{
private:
    ConnectionHandler& handler;
    std::mutex& mutex;

public:
    keyboardReader(ConnectionHandler& handler, std::mutex& mutex);
    void run();
};
#endif //CLIENT_KEYBOARDREADER_H
