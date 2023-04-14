#include <malloc.h>
#include <unistd.h>

#include "socket_table.h"

static SocketTable* socketTable;
static pthread_mutex_t mutex;
static bool isInitialized = FALSE;

static void _initSocketTableParams(descriptor socket) {
    socketTable->length = 1;
    socketTable->threads = (pthread_t*) malloc(sizeof(pthread_t));
    socketTable->threads[0] = pthread_self();
    socketTable->sockets = (descriptor*) malloc(sizeof(descriptor));
    socketTable->sockets[0] = socket;
}

void _initSocketTable(descriptor firstSocket) {
    if(socketTable != NULL)
        return;

    if(pthread_mutex_init(&mutex, NULL) != 0)
        return;

    pthread_mutex_lock(&mutex);

    socketTable = (SocketTable*) malloc(sizeof(SocketTable));

    _initSocketTableParams(firstSocket);
    
    pthread_mutex_unlock(&mutex);
}

void _insertSocket(descriptor socket) {
    pthread_mutex_lock(&mutex);
    if(socketTable->length == 0) {
        _initSocketTableParams(socket);
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_t* oldThreads = socketTable->threads;
    descriptor* oldSockets = socketTable->sockets;

    socketTable->threads = (pthread_t*) malloc(
        sizeof(pthread_t) * (socketTable->length + 1)
    );
    socketTable->sockets = (descriptor*) malloc(
        sizeof(descriptor) * (socketTable->length + 1)
    );
    for (i32 i = 0; i < socketTable->length; i++) {
        socketTable->threads[i] = oldThreads[i];
        socketTable->sockets[i] = oldSockets[i];
    }

    free(oldThreads);
    free(oldSockets);

    socketTable->threads[socketTable->length] = pthread_self();
    socketTable->sockets[socketTable->length] = socket;
    socketTable->length++;
    pthread_mutex_unlock(&mutex);
}

static i32 _getThreadIndex() {
    pthread_t self = pthread_self();
    for (i32 i = 0; i < socketTable->length; i++) {
        if(socketTable->threads[i] == self)
            return i;
    }
    return -1;
}

descriptor _getSocket() {
    if(!isInitialized || socketTable == NULL) {
        return -1;
    }
    pthread_mutex_lock(&mutex);
    descriptor output = socketTable->sockets[_getThreadIndex()];
    pthread_mutex_unlock(&mutex);
    return output;
}

void _deleteSocket() {
    if(!isInitialized || socketTable == NULL)
        return;
    pthread_mutex_lock(&mutex);
    if(socketTable->length == 1) {
        socketTable->length = 0;

        free(socketTable->threads);
        free(socketTable->sockets);

        socketTable->threads = NULL;
        socketTable->sockets = NULL;
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_t currentThread = pthread_self();

    pthread_t* oldThreads = socketTable->threads;
    descriptor* oldSockets = socketTable->sockets;

    socketTable->threads = (pthread_t*) malloc(
        sizeof(pthread_t) * (socketTable->length - 1)
    );
    socketTable->sockets = (descriptor*) malloc(
        sizeof(descriptor) * (socketTable->length - 1)
    );

    i32 offset = 0; // offset in the new array compared to the old one
    for(i32 i = 0; i < socketTable->length; i++) {
        if(socketTable->threads[i] == currentThread) {
            offset = -1;
            continue;
        }
        socketTable->threads[i + offset] = oldThreads[i];
        socketTable->sockets[i + offset] = oldSockets[i];
    }

    free(oldThreads);
    free(oldSockets);
    pthread_mutex_unlock(&mutex);
}

void _destroySocketTable() {
    if(!isInitialized || socketTable == NULL)
        return;
    pthread_mutex_lock(&mutex);
    for(i32 i = 0; i < socketTable->length; i++) {
        close(socketTable->sockets[i]);
    }
    free(socketTable->sockets);
    free(socketTable->threads);
    free(socketTable);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    socketTable = NULL;
}

