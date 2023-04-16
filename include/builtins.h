#ifndef BUILTINS_H
#define BUILTINS_H

#include "base.h"

#define QUIT_BUILTIN "quit"
#define HALT_BUILTIN "halt"
#define EXIT_BUILTIN "exit" // halt synonym
#define HELP_BUILTIN "help"
#define ECHO_BUILTIN "echo"

// call builtin, TRUE if it runs, FALSE if it is not a builtin
bool callBuiltin(
    string* args,
    descriptor in,
    descriptor out,
    descriptor err
);

void quit(descriptor input, descriptor output, descriptor error);
void halt(descriptor input, descriptor output, descriptor error);
void help(descriptor input, descriptor output, descriptor error);
void echo(
    string* args,
    descriptor input,
    descriptor output,
    descriptor error
);

#endif /* end of include guard: BUILTINS_H */
// vim: filetype=c
