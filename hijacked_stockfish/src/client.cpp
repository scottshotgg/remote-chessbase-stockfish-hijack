#include <iostream>
#include <stdio.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class UDPClient
{
public:
  UDPClient(
    boost::asio::io_service& io_service, 
    const std::string& host, 
    const std::string& port
  ) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
    udp::resolver resolver(io_service_);
    udp::resolver::query query(udp::v4(), host, port);
    udp::resolver::iterator iter = resolver.resolve(query);
    endpoint_ = *iter; 
  }

  ~UDPClient()
  {
    socket_.close();
  }

  void send(const std::string& msg) {
    socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
  }


  // TODO: Later change this to async_recieve_from and native_non_blocking: 
  // http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio/reference/ip__udp/socket.html

  // use boost::asio::error:
  // http://www.boost.org/doc/libs/1_62_0/boost/asio/error.hpp

  std::string recv() {
    unsigned char ma[1];
    char msg[1024];

    try {
      boost::system::error_code error;

      socket_.receive_from(boost::asio::buffer(ma, 2), sender_endpoint, 0, error);

     if (error && error != boost::asio::error::message_size)
      throw boost::system::system_error(error);

    } 
    catch (std::exception& e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return std::string("exception ma");
    }

    try {
      boost::system::error_code error;

      socket_.receive_from(boost::asio::buffer(msg, int(ma[0])), sender_endpoint, 0, error);
      msg[ma[0]] = '\0';

     if (error && error != boost::asio::error::message_size)
      throw boost::system::system_error(error);

    } 
    catch (std::exception& e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return std::string("exception msg");
    }

    return std::string(msg);
  }

private:
  boost::asio::io_service& io_service_;
  udp::socket socket_;
  udp::endpoint endpoint_;
  udp::endpoint sender_endpoint;
};