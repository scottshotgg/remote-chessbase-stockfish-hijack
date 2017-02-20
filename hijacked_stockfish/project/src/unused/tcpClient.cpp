#include "client.h"
#include <iostream>
#include <stdio.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
    

// TCPClient::TCPClient() {
// 	this->io_service_ = NULL;
// 	this->socket_ = NULL;
// 	this->endpoint = NULL;
// 	this->sender_endpoint = NULL;

// }

TCPClient::TCPClient(
      boost::asio::io_service& io_service, 
      const std::string& host, 
      const std::string& port
    ) : io_service_(io_service), socket_(io_service, tcp::endpoint(tcp::v4(), 0)) {
      tcp::resolver resolver(io_service_);
      tcp::resolver::query query(tcp::v4(), host, port);
      tcp::resolver::iterator iter = resolver.resolve(query);
      endpoint_ = *iter;
    }

// //TCPClient::TCPClient(const TCPClient& client) {
//   this->io_service = client.io_service_;
//   this->host = client.host;
//   this->port = client.port;
// }

// TCPClient::TCPClient operator=(const TCPClient&); {
//   this->io_service = client.io_service_;
//   this->host = client.host;
//   this->port = client.port;
// }



//TCPClient::TCPClient(const TCPClient& client): 


TCPClient::~TCPClient() {
  socket_.close();
}

void TCPClient::Send(std::string msg) {
  socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
}

std::string TCPClient::Recv() {
  unsigned char ma[1];
  char msg[1024];

  try {
    boost::system::error_code error;

    socket_.receive_from(boost::asio::buffer(ma, 1), sender_endpoint, 0, error);
    //printf("AMOUNT: %d\n", ma[0]);

    if (error && error != boost::asio::error::message_size) {
      std::cerr << "THREW AN ERROR" << std::endl;
      throw boost::system::system_error(error);
    }
  } 
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return std::string("exception ma");
  }

  if(ma[0] != 0) {
    try {
      boost::system::error_code error;

      socket_.receive_from(boost::asio::buffer(msg, ma[0]), sender_endpoint, 0, error);
      msg[ma[0]] = '\0';
      //printf("MESSAGE: %s\n", msg);

      if (error && error != boost::asio::error::message_size) {
        std::cerr << "THREW AN ERROR" << std::endl;
        throw boost::system::system_error(error);
      }

    } 
    catch (std::exception& e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return std::string("exception msg");
    }
  } else {
    msg[0] = '\0';
  }

  return std::string(msg);
}