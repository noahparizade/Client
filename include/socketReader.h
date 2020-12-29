//
// Created by spl211 on 29/12/2020.
//
#include "connectionHandler.h"
#ifndef CLIENT_SOCKETREADER_H
#define CLIENT_SOCKETREADER_H
class socketReader{
private:
    ConnectionHandler& handler;
public:
    socketReader(ConnectionHandler& handler);
    void run();
};
#endif //CLIENT_SOCKETREADER_H
