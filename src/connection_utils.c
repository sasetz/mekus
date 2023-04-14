#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include "connection_utils.h"
#include "socket_table.h"

void terminateAll(exitCode code) {
    i32 status = 0;
    while(wait(&status) > 0); // wait until all processes finish their work

    // close all sockets
    _destroySocketTable();

    // terminate the whole process
    syscall(SYS_exit_group, code);
}

void terminateClient() {
    descriptor socket = _getSocket();
    close(socket);
    _deleteSocket();
    pthread_exit(EXIT_SUCCESS);
}

