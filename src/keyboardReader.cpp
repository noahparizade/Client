//
// Created by spl211 on 29/12/2020.
//
#include "../include/keyboardReader.h"

using namespace std;
#include <iostream>

keyboardReader::keyboardReader(ConnectionHandler& handler, std::mutex& mutex): handler(handler), mutex(mutex){
}
void keyboardReader::run() {
    std::string prevline="";
    while (!handler.getTerminate()) {
        const short bufsize = 1024;
        char buf[bufsize];
      //  if (prevline=="LOGOUT"){
        //    mutex.lock();
        //}
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        prevline=line;
        int len = line.length();
        if (!handler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
        std::cout << "Sent " << len + 1 << " bytes to server" << std::endl;
    }
}