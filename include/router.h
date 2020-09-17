#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"

// TODO: Implement
void route_add(const char*, void (*)(HTTP*, int));
void route_del(const char*);
void route(const char*, HTTP*, int);

#endif
