#include <iostream>
#include <stdio.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class UDPClient
{
  public:
    UDPClient();

    UDPClient(boost::asio::io_service& io_service, std::string host, std::string port);

    UDPClient(const UDPClient& client);

    ~UDPClient(void);

    void Send(std::string msg);

    std::string Recv();




    // TODO: Later change this to async_recieve_from and native_non_blocking: 
    // http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio/reference/ip__udp/socket.html

    // use boost::asio::error:
    // http://www.boost.org/doc/libs/1_62_0/boost/asio/error.hpp

    

  private:
    boost::asio::io_service& io_service_;
    udp::socket socket_;
    udp::endpoint endpoint_;
    udp::endpoint sender_endpoint;
  };