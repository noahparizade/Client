//
// Created by spl211 on 29/12/2020.
//
#include "connectionHandler.h"
#include <mutex>
#ifndef CLIENT_SOCKETREADER_H
#define CLIENT_SOCKETREADER_H
class socketReader{
private:
    ConnectionHandler& handler;
    std::mutex& mutex;
public:
    socketReader(ConnectionHandler& handler, std::mutex& mutex);
    void run();
};
#endif //CLIENT_SOCKETREADER_H
