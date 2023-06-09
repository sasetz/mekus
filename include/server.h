#ifndef SERVER_H
#define SERVER_H

#include "base.h"
#include "connection.h"

// describes a client
typedef struct {
    descriptor socket;
    ConnectionParams params;
} ClientData;

void server(ConnectionParams connectionParams);
void* serveThread(void* dataPointer);

#endif /* end of include guard: SERVER_H */

// vim: filetype=c
