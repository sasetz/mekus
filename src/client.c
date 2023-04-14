#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "client.h"

void* inputThread(void* socketPointer) {
    descriptor socket = *(descriptor*)socketPointer;
    char buffer[512];
    zero(buffer, 512);
    i32 charactersRead = 0;
    bool writeSuccessful = TRUE;
    while(writeSuccessful && (charactersRead = read(0, buffer, 512)) > 0) {
        writeSuccessful = write(socket, buffer, charactersRead) > 0;
    }
    printf("The server has ended the connection\n");
    syscall(SYS_exit_group, EXIT_SUCCESS); // terminate the whole program
    return EXIT_SUCCESS;
}

void* outputThread(void* socketPointer) {
    descriptor socket = *(descriptor*)socketPointer;
    char buffer[512];
    zero(buffer, 512);
    i32 charactersRead = 0;
    while((charactersRead = read(socket, buffer, 512)) > 0) {
        write(1, buffer, charactersRead);
    }
    printf("The server has ended the connection\n");
    syscall(SYS_exit_group, EXIT_SUCCESS); // terminate the whole program
    return EXIT_SUCCESS;
}

void client(ConnectionParams connectionParams) {
    // 1. connect to socket
    // 2. spawn a thread to read and send user input
    // 3. in the main thread, display input data

    descriptor socketDescriptor = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(socketDescriptor == -1) {
        perror("socket");
        pthread_exit((void*)EXIT_FAILURE);
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
        pthread_exit((void*)EXIT_FAILURE);
    }
    printf("Connection successful!\n");

    pthread_t controlThreadId, pawnThreadId;
    pthread_create(
        &controlThreadId,
        NULL,
        &outputThread,
        (void*)&socketDescriptor
    );
    pthread_create(
        &pawnThreadId,
        NULL,
        &inputThread,
        (void*)&socketDescriptor
    );
    pthread_exit(EXIT_SUCCESS); // finish with the main thread
}
