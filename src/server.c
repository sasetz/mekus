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

bool scanMessage(descriptor socket, string buffer, i32 length) {
    struct pollfd fds = {
        socket,
        POLLIN,
        0
    };
    i32 readBytes = poll(&fds, 1, IDLE_TIMEOUT);
    if(readBytes == -1) {
        // an error occurred

        close(socket); // end the connection
        pthread_exit(EXIT_SUCCESS); // successful exit
    } else if(readBytes == 0) {
        // timeout occurred
        printMessage(socket, IDLE_MESSAGE);
        close(socket);
        pthread_exit(EXIT_SUCCESS);
    }
    readBytes = read(socket, buffer, length - 1);
    if(readBytes == -1) {
        // an error occurred

        close(socket); // end the connection
        pthread_exit(EXIT_SUCCESS); // successful exit
    }

    buffer[readBytes] = 0; // null-terminate the string
    return TRUE;
}

void* serveThread(void* dataPointer) {
    ClientData data = *(ClientData*)dataPointer;
    _insertSocket(data.socket);

    // print welcome message
    printMessage(data.socket, "Welcome to Mekus shell!\n");
    char buffer[513];
    bool result;
    result = script(
        data.params.parameters.server.configPath,
        data.socket,
        data.socket,
        data.socket
    );
    if(!result)
        printMessage(data.socket, NO_SCRIPT_MESSAGE);
    // endless cycle since the connection is terminated using connection_utils
    while(TRUE) {
        // run prompt script each time the user wants to do smth
        result = script(
            data.params.parameters.server.promptPath,
            data.socket,
            data.socket,
            data.socket
        );

        if(!result)
            interpret(
                "prompt", // internal command to print out default prompt
                data.socket,
                data.socket,
                data.socket
            );

        scanMessage(data.socket, buffer, 513);
        interpret(
            buffer,
            data.socket,
            data.socket,
            data.socket
        );
    }
    return 0;
}

void server(ConnectionParams connectionParams) {
    // close the communication to terminal, it can interfere with the client

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
