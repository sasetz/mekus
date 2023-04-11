#include <unistd.h>
#include <stdlib.h>

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

ConnectionParams bootstrap(StartupParams settings) {
    ConnectionParams output;
    zeroStruct(output);
    if(settings.isClient && settings.isServer) {
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
        }
    } else if(settings.isClient) {
        output.mode = CLIENT;
    } else if(settings.isServer) {
        output.mode = SERVER;
    }

    switch(output.mode) {
        case SCRIPT: {
            output.parameters.script.scriptPath = settings.scriptPath;
            break;
        }
        case SERVER: {
            output.parameters.server.socketPath = settings.socketPath;
            output.parameters.server.configPath = settings.configPath;
            output.parameters.server.promptPath = settings.promptPath;
            output.parameters.server.queueLength = CONNECTION_QUEUE_LENGTH;
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

