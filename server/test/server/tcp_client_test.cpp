#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>


void send_something(std::string host, int port, std::string message)
{
	boost::asio::io_service ios;
			
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);

    //boost::asio::ip::tcp::socket socket(ios);
    boost::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(ios));

	(socket*).connect(endpoint);

	boost::array<char, 128> buf;
    std::copy(message.begin(),message.end(),buf.begin());
	boost::system::error_code error;
	socket.write_some(boost::asio::buffer(buf, message.size()), error); 

	boost::asio::streambuf buffed;
	
	while(1) {
		boost::asio::read_until(socket, buffed, "\n", error);
		std::istream str(&buffed); 
		std::string s; 
		std::getline(str, s);
		s = s + "\n\n";
		printf(s.c_str());
	}



    socket.close();
}



int main()
{
    send_something("127.0.0.1", 6000, "go infinite\n");
    return 0;
}