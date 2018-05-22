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
  // specify local_properties.json filepath
  const char* path = "./local_properties.json";
  char buffer[256];
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
    reportError("CLIENT: unable to convert ip to address\n");
  }
  hostInfo = gethostbyaddr(&ip, sizeof(ip), AF_INET);
  // set up the socket
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) reportError("SERVER: error opening socket\n");
  // attempt to connect
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    reportError("CLIENT: error connecting\n");
  else {
    printf("CLIENT: connected to server\n");
    // request properties from server
    memset(buffer, '\0', sizeof(buffer));
    int charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
  	if (charsRead < 0)
      reportError("CLIENT: error recieving properties\n");
    // close connection
    close(socketFD);
    // print server_properties to local_properties
    FILE* local_properties = fopen (path, "w");
    fprintf(local_properties, "%s", buffer);
    fclose (local_properties);
  }
  return 0;
}
