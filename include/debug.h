#ifndef DEBUG_H
#define DEBUG_H

void ip_log(struct sockaddr_in*);
void req_log(char*, int, int);
void response(char*, int, int);

#endif
