#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__
                                           
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <future>
using boost::asio::ip::tcp;

class ConnectionHandler {
private:
	const std::string host_;
	const short port_;
	boost::asio::io_service io_service_;   // Provides core I/O functionality
	tcp::socket socket_;
	const std::map<std::string,short> code_map;
    std::promise<bool> terminate;
 
public:
    ConnectionHandler(std::string host, short port);
    virtual ~ConnectionHandler();
 
    // Connect to the remote machine
    bool connect();
 
    // Read a fixed number of bytes from the server - blocking.
    // Returns false in case the connection is closed before bytesToRead bytes can be read.
    bool getBytes(char bytes[], unsigned int bytesToRead);
 
	// Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);
	

    // Returns false in case connection closed before a newline can be read.
    bool getLine(std::string& line);
	

    // Returns false in case connection closed before all the data is sent.
    bool sendLine(std::string& line);
 
    // Get Ascii data from the server until the delimiter character
    // Returns false in case connection closed before null can be read.
    bool getFrameAscii(std::string& frame, char delimiter);

	
    // Close down the connection properly.
    void close();

    void shortToBytes(short num, char* bytesArr);


    short getCode(std::string& msg);

    std::map<std::string, short> init_map();

    short bytesToShort(char* bytesArr);

    std::promise<bool>& getTerminate();

    void setTerminate(bool result);

    void resetFuture();
 
}; //class ConnectionHandler
 
#endif