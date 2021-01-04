//
// Created by spl211 on 29/12/2020.
//
#include "../include/socketReader.h"

using namespace std;
#include <iostream>
socketReader::socketReader(ConnectionHandler& handler):handler(handler) {}

void socketReader::run() {

    while (true) {
        std::string answer;
        if (!handler.getLine(answer)) { //if couldn't get message from server dut to network problem
            break;
        }

        std::cout << answer << std::endl;

        if (answer=="ACK 4"){ //if answer was successful logout,the future object resolved as true
            handler.setTerminate(true);
            break;
        }
        if (answer=="ERROR 4") { //if answer was unsuccessful logout, future object resolved as false
            handler.setTerminate(false);
        }
    }

}