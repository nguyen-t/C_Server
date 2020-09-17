#include <threads.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "ev.h"

// Epoll states
static int poll_fd = -1;
static _Atomic unsigned int running = 0;

// Create epoll file descriptor
int ev_init(int flags) {
  if(fcntl(poll_fd, F_GETFD) != -1) {
    return EV_DOUBLE_INIT;
  }

  return ((poll_fd = epoll_create1(flags)) < 0)
    ? -1
    : EV_SUCCESS;
}

// Add new file descriptor to monitor
int ev_add(int event_fd) {
  // Only monitoring inputs
  struct epoll_event add = {
    .events = EPOLLIN,
    .data.fd = event_fd
  };

  if(poll_fd < 0) {
    return EV_INVALID_FD;
  }

  return epoll_ctl(poll_fd, EPOLL_CTL_ADD, event_fd, &add);
}

// Remove file descriptor from epoll
int ev_del(int event_fd) {
  struct epoll_event del = {
    .events = EPOLLIN,
    .data.fd = event_fd
  };

  if(poll_fd < 0) {
    return EV_INVALID_FD;
  }

  return epoll_ctl(poll_fd, EPOLL_CTL_DEL, event_fd, &del);
}

// Monitors epoll
int ev_loop(int concurrent, void (*handler)(int)) {
  struct epoll_event ready[concurrent];

  if(running) {
    return EV_IN_USE;
  }

  // Set running state
  running = EV_RUNNING;

  while(running) {
    int count = epoll_wait(poll_fd, ready, concurrent, 0);

    for(int i = 0; i < count; i++) {
      int event = ready[i].events;
      int fd = ready[i].data.fd;

      // Gets rid of disconnected file descriptors
      if(event == EPOLLHUP || event == EPOLLERR) {
        ev_del(fd);
      } else {
        handler(fd);
      }
    }

    while(running == EV_PAUSED);
  }

  return EV_SUCCESS;
}

// Alternates between paused and unpaused states
void ev_pause(void) {
  running = (running == EV_RUNNING)
    ? EV_PAUSED
    : EV_RUNNING;
}

// Terminates ev_loop if running
void ev_stop(void) {
  running = EV_STOPPED;
}

// Close and resets epoll
void ev_close(void) {
  close(poll_fd);

  poll_fd = -1;
}
