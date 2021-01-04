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
        if (prevline=="LOGOUT") { //if LOGOUT was previous line, check if termination is needed
            std::future<bool> future=handler.getTerminate().get_future(); //gets the future object with the result
            if (future.get()) //wait until the future object is resolved
                break; //if true, logout was successful so termination is needed
            else{
                handler.resetFuture(); //logout was unsuccessful, so new future object is created
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