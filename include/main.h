#ifndef MAIN_H
#define MAIN_H

#include <argp.h>

#include "base.h"
#include "connection.h"

typedef struct {
    bool isClient, isServer;
    bool scriptMode;
    string scriptPath, promptPath, configPath, socketPath;
} StartupParams;

const char* argp_program_version = "mekus v0.0.1";
const char* argp_program_bug_address = "<ki.putiatin@gmail.com>";
static constString doc =
    "Mekus -- shell assignment for STU, utilizes client-server architecture";
static constString args_doc = "[SCRIPT]";

static struct argp_option options[] = {
    {"socket", 'u', "SOCKET_PATH", 0,
        "Specify socket to connect to"},

    {"config", 'c', "CONFIG_PATH", OPTION_ARG_OPTIONAL,
        "Specify path to config file"},

    {"prompt", 'p', "PROMPT_PATH", OPTION_ARG_OPTIONAL,
        "Specify path to custom prompt script"},

    {"listen", 'l', 0, 0, "Start a client"},
    {"serve", 's', 0, 0, "Start a server"},
    { 0 },
};

struct arguments {
    string scriptPath, socketPath, configPath, promptPath;
    bool client, server;
};

static error_t parseOpt(i32 key, string arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'u': {
            arguments->socketPath = arg;
            break;
        }
        case 'c': {
            arguments->configPath = arg;
            break;
        }
        case 'p': {
            arguments->promptPath = arg;
            break;
        }
        case 's': {
            arguments->server = TRUE;
            break;
        }
        case 'l': {
            arguments->client = TRUE;
            break;
        }
        case ARGP_KEY_ARG: {
            // positional args
            if(state->arg_num > 1) {
                argp_usage(state);
            }
            arguments->scriptPath = arg;
            break;
        }
        case ARGP_KEY_END: {
            if(strlen(arguments->scriptPath) > 0) {
                // script mode
                if(arguments->server || arguments->client) {
                    argp_usage(state);
                }
            } else if(!(arguments->server) && !(arguments->client)) {
                // the user did not specify neither server, nor client
                argp_usage(state);
            } else if(strlen(arguments->socketPath) == 0) {
                // no socket path
                argp_usage(state);
            }
            break;
        }
        default: {
            return ARGP_ERR_UNKNOWN;
        }
    }
    return 0;
}

static struct argp argp = { options, parseOpt, args_doc, doc };

// argc - number of arguments
// argv - argument string
StartupParams parseArguments(i32 argc, string* argv);

// spawn the server, if needed
ConnectionParams bootstrap(StartupParams params);

#endif /* end of include guard: MAIN_H */
// vim: filetype=c
