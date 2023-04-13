#ifndef SOCKET_TABLE_H
#define SOCKET_TABLE_H

#include <pthread.h>

#include "base.h"

// data structure that holds thread ids against socket descriptors
// i.e. each thread has a socket descriptor (or connection)
typedef struct {
    i32 length;
    pthread_t* threads;
    descriptor* sockets;
} SocketTable;

// those functions get, insert and delete sockets for the thread that runs them
void _initSocketTable(descriptor firstSocket);
void _destroySocketTable(); // close and destroy all sockets/connections
void _insertSocket(descriptor socket);
descriptor _getSocket();
void _deleteSocket();

#endif /* end of include guard: SOCKET_TABLE_H */

// vim: filetype=c
