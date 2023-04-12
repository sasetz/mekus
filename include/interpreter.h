#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "base.h"
#include "tokenizer.h"

// takes a string as input, tokenizes it and runs some commands
// data = string to interpret
// input = input descriptor, reads from there
// output = output descriptor, writes data there
void interpret(string data, descriptor input, descriptor output);

// runs a single command, regardless of whether it is external or built-in
void runCommand(Token* tokens, descriptor input, descriptor output);

string builtins[] = {
    "halt",
    "quit",
    "exit",
    "help",
};

#endif /* end of include guard: INTERPRETER_H */

// vim: filetype=c
