#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>

#include "client.h"

void client(ConnectionParams connectionParams) {
    // 1. connect to socket
    // 2. spawn a thread to read and send user input
    // 3. in the main thread, display input data

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
        connectionParams.parameters.client.socketPath,
        108
    );
    // TODO: connect to the server's socket
    // TODO: spawn a thread for asyncronous io
}
