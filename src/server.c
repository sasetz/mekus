#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <pthread.h>

#include "server.h"

i32 clientService(ClientData* data) {
    // all terminal data actually flows to the client
    dup2(data->socket, 1);
    dup2(data->socket, 2);
    dup2(data->socket, 0);

    // test message
    printf("Welcome to Mekus shell!\n");
    close(data->socket); // end the connection
    return 0;
}

void server(ConnectionParams connectionParams) {
    // 1. create socket
    // 2. wait for a connection
    // 3. spawn a new thread that handles the client
    // 4. go back to step 2

    close(1); // we don't need the output for server
    close(2);

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

    descriptor clientSocket = accept(socketDescriptor, NULL, NULL);
    pthread_t threadId;
    ClientData data = {
        clientSocket,
        connectionParams
    };
    pthread_create(&threadId, NULL, &clientService, (void*)&data);
    pthread_join(threadId, NULL);
    // TODO: add connection handling using threads
}
