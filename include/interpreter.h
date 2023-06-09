#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "base.h"
#include "tokenizer.h"

// options of how the program should be run
typedef struct {
    // WARN: must be terminated by NULL
    string* args;

    // redirections
    string inputFilePath; // NULL if there is no need to open it
    string outputFilePath; // same
    string errorFilePath; // same
    descriptor output; // where to route stdout (1 is the default)
    descriptor error;  // where to route stderr (2 is the default)
} ProgramOptions;

// chain of programs and parameters of how to launch them (redirects)
typedef struct {
    ProgramOptions* options; // array of parameters to programs
    i32 length; // length of the array
} Pipeline;

// interpret a string of text
// io is conducted using the socket
void interpret(string data, descriptor in, descriptor out, descriptor err);

// runs a single command (the one that is divided by ; or \n), regardless of
// whether it is external or built-in
void runCommand(
    TokenList* args,
    descriptor in,
    descriptor out,
    descriptor err
);

void runPipeline(
    Pipeline pipeline,
    descriptor input,
    descriptor output,
    descriptor error
);


// run new process with
// parameters = [0] - program name (mandatory), [1..] - arguments
// in = stdin descriptor for the process
// out = stdout descriptor for the process
// err = stderr descriptor for the process
//
// NOTE: this function saves original 0-2 descriptors
pid runProgram(
    string* args,
    descriptor in,
    descriptor out,
    descriptor err
);

#define BUILTIN_PID ((pid)(-1))

// takes program parameters and runs it
// makes redirects according to the parameters, uses in, out and err as
// defaults
pid redirectAndRun(
    ProgramOptions options,
    descriptor in,
    descriptor out,
    descriptor err
);

#endif /* end of include guard: INTERPRETER_H */

// vim: filetype=c
