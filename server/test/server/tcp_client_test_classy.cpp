#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>




boost::shared_ptr<boost::asio::ip::tcp::socket> connect(std::string host, int port) {
	boost::asio::io_service ios;
			
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);

    //socketPtr = boost::asio::ip::tcp::socket socket(ios);
    boost::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(ios));

	(*socket).connect(endpoint);

	return socket;
}   

std::string read(boost::shared_ptr<boost::asio::ip::tcp::socket> socketPtr) {
    boost::asio::streambuf buffed;
    std::istream str(&buffed); 
    std::string s; 
    boost::system::error_code error;

    boost::asio::read_until((*socketPtr), buffed, "\n", error);
    std::getline(str, s);
    return s;
}


void send(std::string message, boost::shared_ptr<boost::asio::ip::tcp::socket> socketPtr) {
	boost::array<char, 128> buf;
    std::copy(message.begin(),message.end(),buf.begin());
	boost::system::error_code error;
	(*socketPtr).write_some(boost::asio::buffer(buf, message.size()), error);
}


void close(boost::shared_ptr<boost::asio::ip::tcp::socket> socketPtr) {
    (*socketPtr).close();
}


int main() {
    boost::shared_ptr<boost::asio::ip::tcp::socket> socketPtr = connect("127.0.0.1", 6000);
    send("go infinite\n", socketPtr);

    for(;;) {
        printf((read(socketPtr) + "\n\n").c_str());
    }

    close(socketPtr);
}

