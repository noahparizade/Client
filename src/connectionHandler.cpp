
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <unordered_map>
#include "../include/connectionHandler.h"
#include "boost/lexical_cast.hpp"
using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using boost::lexical_cast;
using boost::bad_lexical_cast;
 
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_), code_map(init_map()), terminate(){}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}
 
bool ConnectionHandler::connect() { //connect to the server similar to the echo client example

    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        return false;
    }

    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {

        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\0');
}

bool ConnectionHandler::sendLine(std::string& line) {
    short code=getCode(line); //finds the opCode of the message
    line=line.substr(line.find(" ")+1,line.length());
    char* bytes=new char[2];
    shortToBytes(code,bytes);
    bool result;//send error if false
    if ((code==4)|(code==11)) { //LOGOUT or MYCOURSES has no other parameters
        result=sendBytes(bytes, 2);
        delete[] bytes;
        return result;
    }
    if ((code==1)|(code==2)|(code==3)) {  //messages with 2 string parameters
        std::string word1 = line.substr(0, line.find(" "));
        std::string word2 = line.substr(line.find(" ") + 1, line.length());
        const char* word1bytes=word1.c_str();
        const char* word2bytes=word2.c_str();
        char* combined=new char[2+word1.length()+1+word2.length()+1];
        combined[0]=bytes[0];
        combined[1]=bytes[1];
        int size1=word1.length();
        for (int i=0;i<size1+1;i++){
            combined[i+2]=word1bytes[i];
        }
        int size2=word2.length();
        for (int i=0;i<size2+1;i++)
            combined[i+2+size1+1]=word2bytes[i];
        result=sendBytes(combined,4+size1+size2);
        delete[] combined;
    }
    else if ((code==5)|(code==6)|(code==7)|(code==9)|(code==10)){ //messages with one short parameter
        short num=boost::lexical_cast<short>(line); //casting from string to short
        char* numArr=new char[4];
        numArr[0]=bytes[0];
        numArr[1]=bytes[1];
        numArr[2] = ((num >> 8) & 0xFF);
        numArr[3] = (num & 0xFF);
        result=sendBytes(numArr,4);
        delete[] numArr;
    }
    else{
        char* combined=new char[2+line.length()+1]; //messages with one string parameter
        combined[0]=bytes[0];
        combined[1]=bytes[1];
        const char* word1bytes=line.c_str();
        int size=line.length();
        for (int i=0;i<size+1;i++){
            combined[i+2]=word1bytes[i];
        }
        result=sendBytes(combined,line.length()+3);
        delete[] combined;
    }
    delete[] bytes;
    return result;
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch='\n';
    int counter=0;
    short originalCode;
    char* start=new char[4];
    try {
        bool result;
        while (counter<4) {  //decode first 4 bytes to get the opCode and original message code
            result = getBytes(&ch, 1);
            start[counter]=ch;
            counter++;
        }
        if (!result) {
            delete[] start;
            return false;
        }
        else {
            short code = (short) ((start[0] & 0xff) << 8);
            code += (short) (start[1] & 0xff);
            originalCode = (short) ((start[2] & 0xff) << 8);
            originalCode += (short) (start[3] & 0xff);
            if (code == 13)//error message
            {
                frame.append("ERROR ");
                frame.append(std::to_string(originalCode));
                delete[] start;
                return true;
            } else {
                frame.append("ACK ");
                frame.append(std::to_string(originalCode));
            }
        }
        //has optional part
        if ((originalCode==11)|(originalCode==6)|(originalCode==7)|(originalCode==8)|(originalCode==9)) {
            while (delimiter != ch) {
                if (!getBytes(&ch, 1)) {
                    delete[] start;
                    return false;
                }
                if (ch != '\0')
                    frame.append(1, ch);
            }
        }
    }
    catch (std::exception& e) {
        delete[] start;
        return false;
    }
    delete[] start;
    return true;
}

 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {

    }
}

void ConnectionHandler::shortToBytes(short num, char* bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}


short ConnectionHandler::getCode(std::string &msg) {
    std::string code=msg.substr(0,msg.find(" "));
    return code_map.at(code);
}

std::map<std::string,short> ConnectionHandler::init_map() { //map between command and opCode
    std::map<std::string, short> new_map=std::map<std::string, short>();
    new_map.insert(std::pair<std::string,short>("ADMINREG",1));
    new_map.insert(std::pair<std::string,short>("STUDENTREG",2));
    new_map.insert(std::pair<std::string,short>("LOGIN",3));
    new_map.insert(std::pair<std::string,short>("LOGOUT",4));
    new_map.insert(std::pair<std::string,short>("COURSEREG",5));
    new_map.insert(std::pair<std::string,short>("KDAMCHECK",6));
    new_map.insert(std::pair<std::string,short>("COURSESTAT",7));
    new_map.insert(std::pair<std::string,short>("STUDENTSTAT",8));
    new_map.insert(std::pair<std::string,short>("ISREGISTERED",9));
    new_map.insert(std::pair<std::string,short>("UNREGISTER",10));
    new_map.insert(std::pair<std::string,short>("MYCOURSES",11));
    return new_map;
}

short ConnectionHandler::bytesToShort(char *bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

std::promise<bool>& ConnectionHandler::getTerminate() {return terminate;}

void ConnectionHandler::setTerminate(bool result) {terminate.set_value(result);}

void ConnectionHandler::resetFuture() {
    terminate=std::promise<bool>();
}







