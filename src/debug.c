#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "http.h"
#include "debug.h"

// Basic ip logging
void ip_log(struct sockaddr_in* connection) {
  char buffer[16];
  const char* result = inet_ntop(
    connection->sin_family,
    &connection->sin_addr.s_addr,
    buffer,
    sizeof(struct sockaddr_in)
  );

  if(result == NULL) {
    printf("Error!\n");
  } else {
    printf("%s:%d\n", buffer, connection->sin_port);
  }
}

// Handles requests
// Just a proof of concept
// Will be replaced by a more complex router
void response(char* raw, int length, int fd) {
  static FILE* html;
  HTTP_Message* req;
  HTTP_Message* res;
  char* send;

  // Check if valid HTTP_Request
  // If not close it
  if(http_verify(raw) != 0) {
    close(fd);
    return;
  }

  html = (html == NULL)
    ? fopen("resources/index.html", "r+")
    : html;
  req = http_format(raw, length);
  res = http_build("HTTP/1.1", "200", "OK");

  if(html == NULL) {
    printf("File open error\n");
  }

  http_attach_file(res, html);
  send = http_stringify(res);

  write(fd, send, strlen(send));

  free(send);
  http_destroy(res);
  http_destroy(req);
}
