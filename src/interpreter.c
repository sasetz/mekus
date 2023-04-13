#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <malloc.h>

#include "interpreter.h"
#include "builtins.h"

void interpretLine(string line, descriptor input, descriptor output) {
    // TODO: implement this function
}

void runPipeline(Pipeline pipeline, descriptor input, descriptor output) {
    if(pipeline.length == 0)
        return;
    if(pipeline.length == 1) {
        // run the single program
        pid pid = runProgram(
            pipeline.options[0].args,
            input,
            output,
            output
        );
        i32 status;
        waitpid(pid, &status, 0); // wait until the pipe stops
        return;
    }
    pid* children = (pid*) malloc(sizeof(pid) * pipeline.length);
    // in this section, we need to create at least one pipe
    descriptor nextInput = input;
    descriptor nextPipe[2];
    for (i32 i = 0; i < pipeline.length - 1; i++) {
        pipe(nextPipe); // create a pipe
        // close the pipe descriptors on exec
        fcntl(nextPipe[0], F_SETFD, FD_CLOEXEC);
        fcntl(nextPipe[1], F_SETFD, FD_CLOEXEC);
        children[i] = redirectAndRun(
            pipeline.options[i],
            nextInput,
            nextPipe[1],
            output
        );

        if(i > 0)
            close(nextInput); // close previous pipe's read end
        close(nextPipe[1]); // close the write end of the pipe

        nextInput = nextPipe[0];
    }
    children[pipeline.length - 1] = redirectAndRun(
        pipeline.options[pipeline.length - 1],
        nextInput,
        output,
        output
    );
    close(nextInput); // close read end of the pipe

    // wait for every child to finish
    for (i32 i = 0; i < pipeline.length; i++) {
        if(children[i] == BUILTIN_PID) // don't wait for builtins
            continue;
        i32 status;
        waitpid(children[i], &status, 0);
    }

    free(children); // release the pids
}

pid redirectAndRun(
    ProgramOptions options,
    descriptor in,
    descriptor out,
    descriptor err
) {
    descriptor actualIn = in;
    descriptor actualOut = out;
    descriptor actualErr = err;
    descriptor files[3] = { -1 };

    if(options.error == 1) {
        actualErr = actualOut;
    } else if(options.error != 2 && options.error != 0) {
        actualErr = options.error;
    }

    if(options.output == 2) {
        actualOut = actualErr;
    } else if(options.output != 1 && options.output != 0) {
        actualOut = options.output;
    }

    if(options.inputFilePath[0] != 0) {
        files[0] = open(options.inputFilePath, O_RDONLY | O_CLOEXEC);
        if(files[0] != -1)
            actualIn = files[0];
    }
    if(options.outputFilePath[0] != 0) {
        files[1] = open(options.inputFilePath,
                        O_WRONLY | O_TRUNC | O_CREAT | O_CLOEXEC);
        if(files[1] != -1)
            actualOut = files[1];
    }
    if(options.errorFilePath[0] != 0) {
        files[2] = open(options.inputFilePath,
                        O_WRONLY | O_TRUNC | O_CREAT | O_CLOEXEC);
        if(files[2] != -1)
            actualErr = files[2];
    }
    pid output = runProgram(options.args, actualIn, actualOut, actualErr);

    // close the files - descriptors are duplicated and we no longer need them
    if(files[0] != -1)
        close(files[0]);
    if(files[1] != -1)
        close(files[1]);
    if(files[2] != -1)
        close(files[2]);

    return output;
}

void runCommand(string* args, descriptor input, descriptor output) {
    // TODO: implement this function
}

pid runProgram(
    string* args,
    descriptor in,
    descriptor out,
    descriptor err
) {
    if(callBuiltin(args, in, out, err)) // try to call a builtin
        return BUILTIN_PID; // return on success

    descriptor originalIn, originalOut, originalErr;
    originalIn = dup(0);
    originalOut = dup(1);
    originalErr = dup(2);

    close(0);
    close(1);
    close(2);

    if(dup2(in, 0) == -1) 
        return -1;
    if(dup2(out, 1) == -1) 
        return -1;
    if(dup2(err, 2) == -1) 
        return -1;

    pid child;
    if((child = fork()) == 0) {
        // the child
        close(originalIn); // close original io, it may not be CLOEXEC
        close(originalOut);
        close(originalErr);

        execvp(args[0], args); // exec with args, respects PATH
        pthread_exit((void*)2); // kill this process
        return -1; // just to be sure
    }
    // the parent
    
    close(0);
    close(1);
    close(2);

    dup2(originalIn, 0);
    dup2(originalOut, 1);
    dup2(originalErr, 2);

    close(originalIn);
    close(originalOut);
    close(originalErr);

    return child;
}

