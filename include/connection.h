#ifndef CONNECTION_H
#define CONNECTION_H

#include "base.h"

// struct that contains information about the connection
// used to send/receive data
typedef struct {
    descriptor socket;
} SocketConnection;

// describes which use cases there are for individual processes
typedef enum Mode Mode;
enum Mode {
    SCRIPT, // run a script, then exit
    CLIENT, // run as client
    SERVER, // run as server
};

// parameters that determine what connections the program should make
typedef struct {
    union {
        struct {
            descriptor socket; // descriptor to bound socket
            string configPath;
            string promptPath;
        } server;
        struct {
            string socketPath; // path to the connection socket
        } client;
        struct {
            string scriptPath;
        } script;
    } parameters;
    Mode mode; // what this process is going to do
} ConnectionParams;

#endif /* end of include guard: CONNECTION_H */

// vim: filetype=c
