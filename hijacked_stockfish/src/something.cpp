
#include <sys/types.h> /* See NOTES */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "happyhttp.h"
#include <string>
#include <json/reader.h>
#include <iostream>

int counted = 0;
std::string responseData;

void OnBegin(const happyhttp::Response* r, void* userdata)
{
  printf("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
  counted = 0;
}

void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
{
  //fwrite(data, 1, n, stdout);
  responseData = (char*)data;
  //printf("%s", responseData);


  counted += n;
}

void OnComplete( const happyhttp::Response* r, void* userdata)
{
  printf("\nCOMPLETE (%d bytes)\n\n", counted);
}

void tunnelString() {

  happyhttp::Connection conn("127.0.0.1", 5000);
  conn.setcallbacks(OnBegin, OnData, OnComplete, 0);

  // Making request
  printf("making request\n");
  const char* cmd = "go depth 10";
  conn.request("PUT", "/cmd", 0, (const unsigned char*)cmd, strlen(cmd));
  //conn.request("PUT", "/something", 0, 0, 0);
  
	while(conn.outstanding())
		conn.pump();

  //printf("This is the response: \n%s\n", responseData.c_str());

}

int main() {
	tunnelString();

  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(responseData.c_str(), root);

  printf(root.get("output", "UTF-8").asString().c_str());
}