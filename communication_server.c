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
  // specify server_properties filepath
  const char* path = "./server_properties.json";
  // read contents of server_properties into buffer
  char* buffer = 0;
  long length;
  FILE* server_properties = fopen (path, "rb");
  if (server_properties) {
    fseek (server_properties, 0, SEEK_END);
    length = ftell (server_properties);
    fseek (server_properties, 0, SEEK_SET);
    buffer = malloc (length);
    if (buffer) {
      fread (buffer, 1, length, server_properties);
    }
    fclose (server_properties);
  }
  // check for error parsing
  if (!buffer) {
    reportError("SERVER: error parsing server_properties");
  }
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
	if (establishedConnectionFD < 0) reportError("SERVER: error connecting to client");
  printf("SERVER: connected to client\n");
  // transmit server_properties to client
  int charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0);
	if (charsWritten < 0)
    reportError("SERVER: error transmitting server_properties");
	if (charsWritten < strlen(buffer))
    printf("SERVER ==warning==: server_properties transmissison incomplete\n");
  // close connection
  close(establishedConnectionFD);
  close(listenSocketFD);
  free(buffer);
  return 0;
}
