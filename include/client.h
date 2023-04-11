#ifndef CLIENT_H
#define CLIENT_H

#include "base.h"
#include "connection.h"

void client(ConnectionParams connectionParams);

void* inputThread(void* socketPointer);
void* outputThread(void* socketPointer);

#endif /* end of include guard: CLIENT_H */

// vim: filetype=c
