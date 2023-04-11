#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "main.h"
#include "script.h"
#include "server.h"
#include "client.h"
#include "settings.h"

StartupParams parseArguments(i32 argc, string argv[]) {
    struct arguments arguments;

    arguments.scriptPath = "";
    arguments.promptPath = DEFAULT_PROMPT;
    arguments.configPath = DEFAULT_CONFIG;
    arguments.client = FALSE;
    arguments.server = FALSE;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return (StartupParams){
        arguments.client,
        arguments.server,
        strlen(arguments.scriptPath) != 0,
        arguments.scriptPath,
        arguments.promptPath,
        arguments.configPath,
        arguments.socketPath
    };
}

// creates and binds a socket to a path in the filesystem
descriptor createSocket(char socketPath[108], i32 queueLength) {
    // create a new socket
    descriptor socketDescriptor = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(socketDescriptor == -1) {
        perror("socket");
        exit(2);
    }

    // initialize the socket address
    struct sockaddr_un address;
    zeroStruct(address);
    address.sun_family = AF_LOCAL;
    strncpy(
        address.sun_path,
        socketPath,
        108
    );

    // get rid of possibly existing socket on that path
    unlink(socketPath);

    // try to bind the socket to filesystem
    if(bind(
        socketDescriptor,
        (struct sockaddr *) &address,
        sizeof(address.sun_path)
    ) == -1) {
        perror("bind");
        exit(2); // this is run before creating any threads, so it's safe
    }

    // set the socket as passive (that accepts connections)
    listen(socketDescriptor, queueLength);

    return socketDescriptor;
}

ConnectionParams bootstrap(StartupParams settings) {
    ConnectionParams output;
    zeroStruct(output);
    descriptor socketDescriptor;
    if(settings.isClient && settings.isServer) {
        socketDescriptor = createSocket(
            settings.socketPath,
            CONNECTION_QUEUE_LENGTH
        );
        descriptor pid = fork();
        if(pid == -1) {
            perror("fork");
            exit(2);
        }

        if(pid == 0) {
            // i am the child
            // become the server
            output.mode = SERVER;
        } else {
            // i am the parent
            // become the client
            output.mode = CLIENT;
            // we don't need the socket descriptor on this end now, close it
            close(socketDescriptor);
        }
    } else if(settings.isClient) {
        output.mode = CLIENT;
    } else if(settings.isServer) {
        output.mode = SERVER;
        socketDescriptor = createSocket(
            settings.socketPath,
            CONNECTION_QUEUE_LENGTH
        );
    }

    switch(output.mode) {
        case SCRIPT: {
            output.parameters.script.scriptPath = settings.scriptPath;
            break;
        }
        case SERVER: {
            output.parameters.server.configPath = settings.configPath;
            output.parameters.server.promptPath = settings.promptPath;
            output.parameters.server.socket = socketDescriptor;
            break;
        }
        case CLIENT: {
            output.parameters.client.socketPath = settings.socketPath;
            break;
        }
    }

    return output;
}

int main(i32 argc, string argv[]) {
    StartupParams settings = parseArguments(argc, argv);

    ConnectionParams connections = bootstrap(settings);

    switch(connections.mode) {
        case SCRIPT: {
            script(connections);
            break;
        }
        case SERVER: {
            server(connections);
            break;
        }
        case CLIENT: {
            client(connections);
            break;
        }
    }

    return 0;
}

