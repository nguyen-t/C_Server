#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define HTTP_LENGTH 8192 - 1

int server(short);
int client(int, void (*)(struct sockaddr_in*));
int transaction(int, void (*)(char*, int, int));

typedef enum {
  SERVER_SOCKET_ERR = -2,
  SERVER_BIND_ERR   = -3,
  SERVER_LISTEN_ERR = -4,
  SERVER_ALLOC_ERR  = -5,
  SERVER_READ_ERR   = -6,
} SERVER_ERR;

typedef enum {
  CLIENT_ACCEPT_ERR = -7,
  CLIENT_COMM_ERR   = -8,
} CLIENT_ERR;

#endif
