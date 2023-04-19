#ifndef SETTINGS_H
#define SETTINGS_H

#define DEFAULT_CONFIG "~/.config/mekus/config.mek"
#define DEFAULT_PROMPT "~/.config/mekus/prompt.mek"

#define CONNECTION_QUEUE_LENGTH 5
#define CONNECTION_TIMEOUT_S 120

#define IDLE_TIMEOUT 60000
#define IDLE_MESSAGE "You have been idling for too long. Thank you for using Mekus!\n"

#define DISCONNECT_MESSAGE "Thank you for using Mekus!\n"
#define TERMINATE_MESSAGE "Thank you for using Mekus! Terminating others...\n"

#define HELP_MESSAGE "Author: Kirill Putiatin\n\nBuilt-in commands:\nhelp - display this message\nquit - end connection\nhalt - terminate the server\nexit - halt synonym\necho - print arguments or from stdin (-n to prevent newline at the end)\nprompt - print default prompt\n"

#define NO_SCRIPT_MESSAGE "The file specified does not exist or you have no permissions to open it!\n"

#endif /* end of include guard: SETTINGS_H */

// vim: filetype=c
