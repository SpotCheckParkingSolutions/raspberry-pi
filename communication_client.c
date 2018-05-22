#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void reportError(char* errorMsg) {
  perror(errorMsg);
  exit(1);
}

int main (int argc, char** argv) {
  // specify server ip
  const char* ipStr = "10.0.0.127";
  // server address stuff
  struct in_addr ip;
  struct hostent* hostInfo;
  struct sockaddr_in serverAddress;
  // set server address and port
  int port = atoi(argv[1]);
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
  // convert ip to proper form
  if (inet_aton(ipStr, &ip) == 0) {
    reportError("CLIENT: unable to convert ip to address");
  }
  hostInfo = gethostbyaddr(&ip, sizeof(ip), AF_INET);
  // memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)hostInfo->h_addr, hostInfo->h_length);
  // set up the socket
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) reportError("SERVER: error opening socket");
  // attempt to connect
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    reportError("CLIENT: error connecting");
  else {
    printf("CLIENT: connected to server\n");
    close(socketFD);
  }
  return 0;
}
