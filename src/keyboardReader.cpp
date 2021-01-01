//
// Created by spl211 on 29/12/2020.
//
#include "../include/keyboardReader.h"

using namespace std;
#include <iostream>

keyboardReader::keyboardReader(ConnectionHandler& handler): handler(handler){
}
void keyboardReader::run() {
    std::string prevline="";
    while (true) {
        const short bufsize = 1024;
        char buf[bufsize];
        if (prevline=="LOGOUT") {
            std::future<bool> future=handler.getTerminate().get_future();
            if (future.get())
                break;
            else{
                handler.resetFuture();
            }
        }
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        prevline=line;
        if (!handler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
    }
}