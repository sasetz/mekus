#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "script.h"
#include "settings.h"
#include "interpreter.h"

bool script(string scriptPath, descriptor in, descriptor out, descriptor err) {
    FILE* file = fopen(
        scriptPath,
        "r"
    );
    if(file == NULL) {
        return FALSE;
    }
    char buffer[512];
    while(fgets(buffer, 512, (FILE*)file) != NULL) {
        interpret(buffer, in, out, err);
    }
    fclose(file);
    return TRUE;
}

