## SPaASM 2023

### Zadanie 2

#### author: Kirill Putiatin

---

## Introduction

In this documentation, I will briefly explain how the program works, what
decisions I made and what tools I used. This program is written in C, compiled
using gcc (Makefile configuration included).

## Program structure

The `base.h` header is the most generic through the project. It contains
several types of data, such as `i32` being 32-bit long signed integer,
some type definitions and constants. These are used across the whole project
and have a separate place there.

The shell starts with `main()` function in `main.c` source file. This function
sets up the app - parses command line arguments, determines what settings it
should use, etc. To parse arguments, I used GNU's argp, which speeds up this
process and is really easy to use. It wouldn't be a problem to manually write
something to parse arguments and display help messages, but this is quite
trivial and time consuming, so I chose to use the library.

`main.c` also contains function called `bootstrap()`, this function reads the
arguments and if the user chose server and client simultaneously, it forks
and separate processes work with each other like they were launched separately.

The program can function in 3 modes: non-interactive script mode, client mode
and server mode.

Client mode (-l) only transmits data between user console and the socket of the
server running it.

Server (-s) listens to the specified socket and interprets received lines.
It uses the same functions as in script mode, but uses socket descriptor as
input/output.

Server launches a separate thread using `pthread.h` library for each client.
This is why there is `socket_table.c`, which contains an array list of sockets,
combined with thread numbers. It is used to efficiently dispose of clients and
correctly close their sockets, even when only one client disconnects.

## Interpreting

The main function for interpreting a line is `interpret()` in `interpreter.c`.
First, it divides the line into tokens using the tokenizer. It gets rid of all
unnecessary characters, such as spaces. It also communicates to the other
functions which of the tokens are 'control' - >, &, ;, etc.

The tokenizer is also responsible for comments, escape characters and quotes.
It is parsed here and then given to other functions.

There are a lot of functions that deal with the token list that is constructed
while parsing the input. First, the input is separated by newline characters
and `;` characters - **pipelines**. Each pipeline can contain multiple commands
that are rerouted to next ones. The pipeline structure is then constructed,
determining what IO should be rerouted where. When all of that is resolved,
raw descriptors are passed to the `runProgram()` function. Those descriptors
are already final, this means, if the output is needed to be piped to the next
program - the pipe already exists and the function doesn't need to know about
this.

`runProgram()` then forks and executes another program. This program has
all the replaced standard descriptors changed to whatever was needed, and it 
doesn't have the access to other open sockets, since the program closed them
before changing its code segment. Also, only the calling thread gets into the
child process, so no need to worry about those.

## Builtins

Internal commands are performed in the `runProgram()` function, which checks
whether the run program is one of the builtins. If it is, it is run and no
additional processes are created. There are 5 total builtins:

1. quit
2. halt/exit
3. help
4. echo
5. prompt

### quit

Quit terminates the connection to the current client. This also closes the
client, since it runs only until the socket is open.

### halt/exit

Halt, or its synonym exit, terminates the whole process and disconnects all
other clients from it. It uses system call `exit_group()` which doesn't have
library wrapper, so I used `syscall()` to run it.

### help

Prints a help message with all builtins and their description

### echo

Prints its arguments or stdin, if there is no arguments provided to it. If the
`-p` argument is specified, the new line symbol at the end is not printed. This
is useful when crafting your own prompt using the `--prompt` argument

### prompt

Prints the default prompt, used when no prompt file can be found in the system.
Note: this does not set the prompt, only prints the default one. To specify a
prompt, use the `--prompt` argument with the script path.

## List of points

- 1. script mode
- 8. output can be redirected to any file descriptor using `&`
- 9. at all times, the client is separate from the server in interactive mode
- 10. double quotes `"` are functional
- 14. prompt is configurable using external script
- 18. if no program is currently running, after one minute the user will be
disconnected
- 28. Makefile included, it conains targets all, debug and exe. debug target
is used for GDB debugger
- 30. this documentation

Total: 22 (20) points

