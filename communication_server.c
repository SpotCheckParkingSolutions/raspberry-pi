#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void reportError(char* errorMsg) {
  perror(errorMsg);
  exit(1);
}

int main(int argc, char** argv) {
  struct sockaddr_in serverAddress, clientAddress;
  // set server address and port
  int port = atoi(argv[1]);
  memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
  // set up the socket
	int listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocketFD < 0) reportError("SERVER: error opening socket");
  // enable listening on the socket
  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		reportError("SERVER: error binding socket");
  // begin listening on the socket
	listen(listenSocketFD, 5);
  // block until a connection is established
	socklen_t sizeOfClientInfo = sizeof(clientAddress);
	int establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	if (establishedConnectionFD < 0) reportError("SERVER: error connecting to server");
  printf("SERVER: connected to client\n");
  // close connection
  close(establishedConnectionFD);
  close(listenSocketFD);
  return 0;
}
