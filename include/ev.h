#ifndef EV_H
#define EV_H

int ev_init(int);
int ev_add(int);
int ev_del(int);
int ev_loop(int, void (*)(int));
void ev_pause(void);
void ev_stop(void);
void ev_close(void);

typedef enum {
  EV_STOPPED = 0,
  EV_RUNNING = 1,
  EV_PAUSED  = 2,
} EV_STATE;

typedef enum {
  EV_SUCCESS     = 0,
  EV_ERR_EPOLL   = -1,
  EV_DOUBLE_INIT = -2,
  EV_INVALID_FD  = -3,
  EV_IN_USE      = -4,
} EV_ERROR;

#endif
