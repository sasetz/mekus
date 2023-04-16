#include <unistd.h>

#include "builtins.h"
#include "connection_utils.h"
#include "settings.h"

bool callBuiltin(
    string* args,
    descriptor in,
    descriptor out,
    descriptor err
) {
    if(strcmp(args[0], QUIT_BUILTIN) == 0) {
        quit(in, out, err);
        return TRUE;
    }
    if(strcmp(args[0], HALT_BUILTIN) == 0) {
        halt(in, out, err);
        return TRUE;
    }
    if(strcmp(args[0], EXIT_BUILTIN) == 0) {
        halt(in, out, err);
        return TRUE;
    }
    if(strcmp(args[0], HELP_BUILTIN) == 0) {
        help(in, out, err);
        return TRUE;
    }
    if(strcmp(args[0], ECHO_BUILTIN) == 0) {
        echo(args, in, out, err);
        return TRUE;
    }
    return FALSE;
}

void quit(descriptor input, descriptor output, descriptor error) {
    write(output, DISCONNECT_MESSAGE, sizeof(DISCONNECT_MESSAGE));
    terminateClient();
}
void halt(descriptor input, descriptor output, descriptor error) {
    write(output, TERMINATE_MESSAGE, sizeof(TERMINATE_MESSAGE));
    terminateAll(EXIT_SUCCESS);
}
void help(descriptor input, descriptor output, descriptor error) {
    write(output, HELP_MESSAGE, sizeof(HELP_MESSAGE));
}

void echo(
    string* args,
    descriptor input,
    descriptor output,
    descriptor error
) {
    char buffer[255];
    i32 len;
    if(args[1] == NULL) {
        while(
            (len = read(input, buffer, 255)) > 0 &&
            write(output, buffer, len) > 0
        );
        return;
    }
    i32 i = 1;
    for(
        string currentString = args[i];
        currentString != NULL;
        i++, currentString = args[i]
    ) {
        len = strlen(currentString);
        write(output, currentString, len);
        write(output, " ", 1);
    }
    write(output, "\n", 1);
}

