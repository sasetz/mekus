#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <pthread.h>

#include "client.h"

i32 inputThread(descriptor* socket) {
    return 0;
}

i32 outputThread(descriptor* socket) {
    printf("Starting the shell...\n");
    char buffer[512];
    zero(buffer, 512);
    i32 charactersRead = 0;
    while((charactersRead = read(*socket, buffer, 512)) > 0) {
        write(1, buffer, charactersRead);
    }
    printf("The shell has exited...\n");
    return 0;
}

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

    i32 connectResult = connect(
        socketDescriptor,
        (struct sockaddr*)&address,
        sizeof(address.sun_path)
    );
    if(connectResult == -1) {
        perror("connect");
        exit(2);
    }
    printf("Connection successful!\n");

    pthread_t threadId;
    pthread_create(&threadId, NULL, &outputThread, (void*)&socketDescriptor);
    pthread_join(threadId, NULL);
}
