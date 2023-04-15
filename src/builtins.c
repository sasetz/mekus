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

