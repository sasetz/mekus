#ifndef SETTINGS_H
#define SETTINGS_H

#define DEFAULT_CONFIG "~/.config/mekus/config.mek"
#define DEFAULT_PROMPT "~/.config/mekus/prompt.mek"

#define CONNECTION_QUEUE_LENGTH 5
#define CONNECTION_TIMEOUT_S 120

char DISCONNECT_MESSAGE[] = "Thank you for using Mekus!\n";
char TERMINATE_MESSAGE[] =
    "Thank you for using Mekus! Terminating others...\n";

char HELP_MESSAGE[] =
    "Author: Kirill Putiatin\n\nBuilt-in commands:\nhelp - display this message\nquit - end connection\nhalt - terminate the server\nexit - halt synonym\n";

char NO_SCRIPT_MESSAGE[] = "The file specified does not exist or you have no permissions to open it!\n";

#endif /* end of include guard: SETTINGS_H */

// vim: filetype=c
