
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
        delete bytes; //todo:check if needed
        return false;
    }
    delete bytes;
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    char* bytes=encode(line);
    return sendBytes(bytes,strlen(bytes));
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    int counter=0;
    char* bytesArr=new char[4];
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
	do{
		if(!getBytes(&ch, 1))
		{
			return false;
		}
		if (counter>3){
		    if (counter==4)
		        frame.append(1,'\n');
            if(ch!='\0')
                frame.append(1, ch);
            else
                frame.append(1,' ');
		}
		else {
		    bytesArr[counter]=ch;
		    counter++;
		    if (counter==3){
		        short num1=bytesToShort(bytesArr);
		        bytesArr[0]=bytesArr[2];
		        bytesArr[1]=bytesArr[3];
		        short num2=bytesToShort(bytesArr);
		        if (num1==13)
		            frame.append("ERROR ");
		        else
                    frame.append("ACK ");
                frame.append(std::to_string(num2));
		    }
		}
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

void ConnectionHandler::shortToBytes(short num, char* bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

char * ConnectionHandler::encode(std::string &line) {//todo: finish
    short code=getCode(line);
    line=line.substr(2,line.length());
    if (code==4|code==1){
        char* bytes=new char[2];
        shortToBytes(code, bytes);
        return bytes;
    }
    else if (code==5|code==6|code==7|code==9|code==10){
        char* bytes=new char[4];
        shortToBytes(code,bytes);
        short num=boost::lexical_cast<short>(line);
        bytes[2]=((num >> 8) & 0xFF);
        bytes[3]=(num & 0xFF);
        return bytes;
    }
    else if (code==8){
        char* bytes=new char[2];
        shortToBytes(code,bytes);
        const char* stringArr=line.c_str();
        char* combined=new char[2+strlen(stringArr)+1];
        combined[0]=bytes[0];
        combined[1]=bytes[1];
        for (int i=2;i<strlen(combined)-1;i++){
            combined[i]=stringArr[i-2];
        }
        combined[strlen(combined)-1]='\0';
        return combined;
    }
    else{
        std::string word1=line.substr(0,line.find(" "));
        std::string word2=line.substr(line.find(" ")+1,line.length());
        char* bytes=new char[2];
        shortToBytes(code,bytes);
        const char* stringArr1=word1.c_str();
        const char* stringArr2=word2.c_str();
        char* combined=new char[2+strlen(stringArr1)+strlen(stringArr2)+2];
        combined[0]=bytes[0];
        combined[1]=bytes[1];
        for (int i=0;i<strlen(stringArr1);i++){
            combined[i+2]=stringArr1[i];
        }
        combined[2+strlen(stringArr1)]='\0';
        for (int i=0;i<strlen(stringArr2);i++){
            combined[i+3+strlen(stringArr1)]=stringArr2[i];
        }
        combined[strlen(combined)-1]='\0';
        return combined;
    }

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







