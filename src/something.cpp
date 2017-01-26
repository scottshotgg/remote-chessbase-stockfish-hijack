
#include <sys/types.h> /* See NOTES */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "happyhttp.h"
#include <string>

int counted = 0;

void OnBegin(const happyhttp::Response* r, void* userdata)
{
  printf("BEGIN (%d %s)\n\n", r->getstatus(), r->getreason());
  counted = 0;
}

void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
{
  fwrite(data,1,n, stdout);
  counted += n;
}

void OnComplete( const happyhttp::Response* r, void* userdata)
{
  printf("\n\nCOMPLETE (%d bytes)\n", counted);
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
}

int main() {
	tunnelString();
}