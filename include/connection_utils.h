#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H

#include "base.h"

// terminate the whole process
void terminateAll(exitCode code);
// terminate only this connection
void terminateClient();

#endif /* end of include guard: CONNECTION_UTILS_H */

// vim: filetype=c
