#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

// Does all the server socket setup
int server(short port) {
  int fd;
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = INADDR_ANY,
    .sin_port = htons(port)
  };

  if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return SERVER_SOCKET_ERR;
  }
  if(bind(fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
    return SERVER_BIND_ERR;
  }
  if(listen(fd, 10) < 0) {
    return SERVER_LISTEN_ERR;
  }

  return fd;
}

// Does all the client socket setup
// Callback for logging purposes
int client(int server_fd, void (*callback)(struct sockaddr_in*)) {
  int fd;
  struct sockaddr_in address;
  socklen_t length = sizeof(address);

  if((fd = accept(server_fd, (struct sockaddr*) &address, &length)) < 0 ) {
    return SERVER_LISTEN_ERR;
  }
  if(callback) {
    callback(&address);
  }

  return fd;
}

// Handles client requests
int transaction(int client_fd, void (*callback)(char*, int, int)) {
  char* req = calloc(HTTP_LENGTH + 1, 1);
  int length = 0;

  if(req == NULL) {
    free(req);
    return SERVER_ALLOC_ERR;
  }
  if((length = read(client_fd, req, HTTP_LENGTH)) < 0) {
    free(req);
    return SERVER_READ_ERR;
  }
  if(length == 0) {
    free(req);
    close(client_fd);
    return CLIENT_COMM_ERR;
  }
  if(callback) {
    callback(req, length, client_fd);
  }

  free(req);

  return 0;
}
