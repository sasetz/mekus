#include <unistd.h>
#include <time.h>

#include "builtins.h"
#include "connection_utils.h"
#include "settings.h"
#include "socket_table.h"

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
    if(strcmp(args[0], PROMPT_BUILTIN) == 0) {
        prompt(in, out, err);
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
    bool lineFeed = TRUE;
    if(strcmp(args[1], "-n") == 0) {
        lineFeed = FALSE;
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
    if(lineFeed)
        write(output, "\n", 1);
}

void prompt(
    descriptor input,
    descriptor output,
    descriptor error
) {
    char buffer[256];
    struct tm timeStructure;
    time_t epoch = time(NULL);
    localtime_r(&epoch, &timeStructure);
    strftime(buffer, 255, "%k:%M", &timeStructure);
    buffer[255] = '\0';
    write(output, buffer, 5); // print time
    write(output, " ", 1);
    
    // critical section since getlogin() is not thread-safe
    lockPrompt();

    if(getlogin_r(buffer, 255) != 0) {
        strcpy(buffer, "user");
    }

    unlockPrompt();

    i32 length = strlen(buffer);
    write(output, buffer, length); // print username
    write(output, "@", 1);
    
    if(gethostname(buffer, 255) != 0) {
        strcpy(buffer, "computer");
    }
    length = strlen(buffer);
    write(output, buffer, length); // print computer name
    write(output, "# ", 2);
}

