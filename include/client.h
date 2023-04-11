#ifndef CLIENT_H
#define CLIENT_H

#include "base.h"
#include "connection.h"

void client(ConnectionParams connectionParams);

i32 inputThread(descriptor* socket);
i32 outputThread(descriptor* socket);

#endif /* end of include guard: CLIENT_H */

// vim: filetype=c
