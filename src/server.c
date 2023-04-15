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
#include "script.h"
#include "interpreter.h"
#include "socket_table.h"

i32 printMessageN(descriptor socket, string message, i32 length) {
    return write(socket, message, length);
}

i32 printMessage(descriptor socket, string message) {
    return write(socket, message, strlen(message));
}

void scanMessage(descriptor socket, string buffer, i32 length) {
    i32 readBytes = read(socket, buffer, length - 1);
    if(readBytes == -1) {
        // an error occurred

        close(socket); // end the connection
        pthread_exit(EXIT_SUCCESS); // successful exit
    }
    buffer[readBytes] = 0; // null-terminate the string
}

void* serveThread(void* dataPointer) {
    ClientData data = *(ClientData*)dataPointer;
    _insertSocket(data.socket);

    // print welcome message
    printMessage(data.socket, "Welcome to Mekus shell!\n");
    char buffer[513];
    // endless cycle since the connection is terminated using connection_utils
    while(TRUE) {
        // run prompt script each time the user wants to do smth
        bool result = script(
            data.params.parameters.server.promptPath,
            data.socket,
            data.socket,
            data.socket
        );

        if(!result)
            printMessage(data.socket, "prompt: ");

        scanMessage(data.socket, buffer, 513);
        interpret(
            buffer,
            data.socket,
            data.socket,
            data.socket
        );
    }

    close(data.socket); // end the connection
    pthread_exit(EXIT_SUCCESS); // successful exit
    return EXIT_SUCCESS; // just in case
}

void server(ConnectionParams connectionParams) {
    // close the communication to terminal, it can interfere with the client
    // TODO: add logging to a file
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

    _insertSocket(socketDescriptor);
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

    pthread_exit(EXIT_SUCCESS); // terminate the connection thread
}
