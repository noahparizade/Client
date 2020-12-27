
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <unordered_map>
#include "../include/connectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_), code_map(init_map()){}
    
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
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
	do{
		if(!getBytes(&ch, 1))
		{
			return false;
		}
		if(ch!='\0')  
			frame.append(1, ch);
	}while (delimiter != ch);
    } catch (std::exception& e) {
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

void ConnectionHandler::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

char * ConnectionHandler::encode(std::string &line) {//todo: finish
    short code=getCode(line);
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
}



