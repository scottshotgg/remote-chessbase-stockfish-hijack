#include <iostream>
#include <stdio.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>


#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#endif


using boost::asio::ip::tcp;

class TCPClient
{
  public:
    TCPClient();

    TCPClient(
      boost::asio::io_service& io_service, 
      const std::string& host, 
      const std::string& port
    );

    //TCPClient(const TCPClient& client);

    ~TCPClient(void);

    void Send(std::string msg);


    std::string Recv();

    //TCPClient operator=(const TCPClient&);




    // TODO: Later change this to async_recieve_from and native_non_blocking: 
    // http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio/reference/ip__tcp/socket.html

    // use boost::asio::error:
    // http://www.boost.org/doc/libs/1_62_0/boost/asio/error.hpp

    

  private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    tcp::endpoint endpoint_;
    tcp::endpoint sender_endpoint;
  };