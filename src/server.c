#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>

#include "server.h"

void server(ConnectionParams connectionParams) {
    // 1. create socket
    // 2. wait for a connection
    // 3. spawn a new thread that handles the client
    // 4. go back to step 2
    descriptor socketDescriptor = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(socketDescriptor == -1) {
        perror("socket");
        exit(2);
    }

    struct sockaddr_un address;

    zeroStruct(address);
    address.sun_family = AF_LOCAL;
    // copy the socket address
    strncpy(
        address.sun_path,
        connectionParams.parameters.server.socketPath,
        108
    );

    // get rid of possibly existing socket
    unlink(connectionParams.parameters.server.socketPath);

    if(bind(
        socketDescriptor,
        (struct sockaddr *) &address,
        sizeof(address.sun_path)
    ) == -1) {
        perror("bind");
        exit(2);
    }

    listen(socketDescriptor, connectionParams.parameters.server.queueLength);

    // TODO: add connection handling using threads
}
