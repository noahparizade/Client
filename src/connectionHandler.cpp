
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
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
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
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
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
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\0');
}

bool ConnectionHandler::sendLine(std::string& line) {
    short code=getCode(line);
    line=line.substr(line.find(" ")+1,line.length());
    char* bytes=new char[2];
    shortToBytes(code,bytes);
    bool result=sendBytes(bytes,2); //send error if false
    if (code==4|code==11)
        return result;
    if (code==1|code==2|code==3) {
        std::string word1 = line.substr(0, line.find(" "));
        std::string word2 = line.substr(line.find(" ") + 1, line.length());
        result = result&&sendBytes(word1.c_str(), word1.length() + 1);
        result = result&&sendBytes(word2.c_str(), word2.length() + 1);
    }
    else if (code==5|code==6|code==7|code==9|code==10){
        short num=boost::lexical_cast<short>(line);
        char* numArr=new char[2];
        shortToBytes(num,numArr);
        result=result&&sendBytes(numArr,2);
    }
    else{
        result=result&&sendBytes(line.c_str(),line.length()+1);
    }
    return result;
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch='\n';
    int counter=0;
    short originalCode;
    char* start=new char[4];
    try {
        bool result = getBytes(start, 4);
        if (!result)
            return false;
        else {
            short code = (short) ((start[0] & 0xff) << 8);
            code += (short) (start[1] & 0xff);
            originalCode = (short) ((start[2] & 0xff) << 8);
            originalCode += (short) (start[3] & 0xff);
            if (code == 13)//error message
            {
                frame.append("ERROR ");
                frame.append(std::to_string(originalCode));
                return true;
            } else {
                frame.append("ACK ");
                frame.append(std::to_string(originalCode));
            }
        }
        if (originalCode==11|originalCode==6|originalCode==7|originalCode==8|originalCode==9) {
            while (delimiter != ch) { //todo:check if extra space after code is important
                if (!getBytes(&ch, 1))
                    return false;
                if (ch != '\0')
                    frame.append(1, ch);
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
	bool result=sendBytes(frame.c_str(),frame.length());
	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
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

std::map<std::string,short> ConnectionHandler::init_map() {
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







