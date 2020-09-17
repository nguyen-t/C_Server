#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "server.h"
#include "ev.h"
#include "debug.h"

static int server_fd;

void clean_up(void) {
  ev_stop();
  ev_close();
  close(server_fd);
}

// Tells epoll how to handle each file descriptor
void manager(int fd) {
  if(fd == server_fd) {
    int client_fd;

    if((client_fd = client(server_fd, ip_log)) < 0) {
      return;
    }
    if(ev_add(client_fd) < 0) {
      return;
    }
  } else if(transaction(fd, response) < 0) {
    close(fd);
  }
}

// Initializes epoll and server
int epoll_server(short port, short concurrent) {
  int error;

  if((server_fd = server(port)) < 0) {
    return server_fd;
  }
  if((error = ev_init(0)) < 0) {
    return error;
  }
  if((error = ev_add(server_fd)) < 0) {
    return error;
  }

  return ev_loop(concurrent, manager);
}

int main(int argc, char** argv) {
  short port;
  short concurrent;

  if(argc != 3) {
    return -1;
  }

  port = strtol(argv[1], NULL, 10);
  concurrent = strtol(argv[2], NULL, 10);

  // Prevents crash from disconnected users
  signal(SIGPIPE, SIG_IGN);
  atexit(clean_up);

  return epoll_server(port, concurrent);
}
