//
// Created by spl211 on 29/12/2020.
//

#ifndef CLIENT_KEYBOARDREADER_H
#define CLIENT_KEYBOARDREADER_H
#include "connectionHandler.h"
class keyboardReader{
private:
    ConnectionHandler& handler;

public:
    keyboardReader(ConnectionHandler& handler);
    void run();
};
#endif //CLIENT_KEYBOARDREADER_H
