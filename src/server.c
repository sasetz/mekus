#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>

#include "server.h"
#include "settings.h"

i32 printMessageN(descriptor socket, string message, i32 length) {
    return write(socket, message, length);
}

i32 printMessage(descriptor socket, constString message) {
    return write(socket, message, strlen(message));
}

i32 scanMessage(descriptor socket, string buffer, i32 length) {
    return read(socket, buffer, length);
}

void* serveThread(void* dataPointer) {
    ClientData data = *(ClientData*)dataPointer;

    // print welcome message
    printMessage(data.socket, "Welcome to Mekus shell!\n");
    printMessage(data.socket, "@# ");

    char buffer[16];
    i32 readBytes = 0;
    while(strcmp(buffer, "exit\n") != 0){
        if((readBytes = scanMessage(data.socket, buffer, 16)) < 1) {
            break;
        }
        printMessageN(data.socket, buffer, readBytes); // echo message
    }
    printMessage(data.socket, "Goodbye!\n");
    close(data.socket); // end the connection
    pthread_exit(0); // successful exit
    return 0;
}

void server(ConnectionParams connectionParams) {
    // close the communication to terminal, it can interfere with the client
    close(0); // close stdin
    close(1); // close stdout
    close(2); // close stderr

    descriptor socketDescriptor = connectionParams.parameters.server.socket;

    // fill in poll struct
    struct pollfd pollSettings = {
        socketDescriptor,
        POLLIN,
        0
    };

    descriptor clientSocket;
    pthread_t threadId;
    while(poll(&pollSettings, 1, 1000 * CONNECTION_TIMEOUT_S) != 0) {
        // accept a new connection
        clientSocket = accept(socketDescriptor, NULL, NULL);

        // create the struct on the heap, so that each thread has its own
        // data struct, and not the same one
        ClientData* dataPointer = (ClientData*) malloc(sizeof(ClientData));
        dataPointer->socket = clientSocket;
        dataPointer->params = connectionParams;

        // serve the client, spawning a new thread
        pthread_create(&threadId, NULL, &serveThread, (void*)dataPointer);
    }

    // no more users need serving

    pthread_exit(0); // terminate the connection thread
}
