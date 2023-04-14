#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

#include "interpreter.h"
#include "builtins.h"

void interpret(string data, descriptor in, descriptor out, descriptor err) {
    Tokenizer tokenizer = _newTokenizer(data);
    TokenList* tokens = _newTokenList();
    while(_hasNextToken(tokenizer)) {
        Token* token = _produceNextToken(&tokenizer);
        if(
            token->isControl &&
            (token->data[0] == ';' || token->data[0] == '\n')
        ) {
            // push current args list to program
            runCommand(tokens, in, out, err);
            _destroyTokenList(tokens);
            tokens = _newTokenList();
        } else {
            _insertToken(tokens, *token);
        }
        _destroyToken(token);
    }

    if(tokens->length > 0) {
        runCommand(tokens, in, out, err);
    }

    _destroyTokenList(tokens);
}

static ProgramOptions getProgramOptions(TokenList* list) {
    TokenList* args = _newTokenList();
    Token* currentToken = _peekCurrentToken(list);
    Token* nextToken = _peekNextToken(list);

    ProgramOptions output = {
        NULL, NULL, NULL, NULL, 1, 2
    };

    while(currentToken != NULL) {
        if(currentToken->isControl) {
            if(currentToken->data[0] == '<') {
                if(nextToken == NULL)
                    break;

                output.inputFilePath = strdup(nextToken->data);
            } else if(currentToken->data[0] == '>') {
                if(nextToken == NULL)
                    break;
                Token* previousToken = _peekPreviousToken(list);
                bool isOut = TRUE; // true = stdout, false = stderr
                descriptor fd = -1;
                if(strcmp(previousToken->data, "2") == 0) {
                    isOut = FALSE;
                }
                if(strcmp(nextToken->data, "&") == 0) {
                    _stepForward(list);
                    Token* tempToken = _peekNextToken(list);
                    if(tempToken == NULL)
                        break;
                    fd = atoi(tempToken->data);
                }
                if(isOut) {
                    if(fd == -1) {
                        output.outputFilePath = strdup(nextToken->data);
                    } else {
                        output.output = fd;
                    }
                } else {
                    if(fd == -1) {
                        output.errorFilePath = strdup(nextToken->data);
                    } else {
                        output.error = fd;
                    }
                }
            }
        } else {
            _insertToken(args, *currentToken);
        }

        _stepForward(list);
        currentToken = _peekCurrentToken(list);
        nextToken = _peekNextToken(list);
    }

    output.args = _toStringArray(list);
    _destroyTokenList(list);
    return output;
}

static void _destroyProgramOption(ProgramOptions object) {
    if(object.args != NULL) {
        string current = object.args[0];
        i32 index = 0;
        while(current != NULL) {
            free(current);
            current = object.args[index];
            index++;
        }
        free(object.args);
    }
    free(object.inputFilePath);
    free(object.outputFilePath);
    free(object.errorFilePath);
}

void runCommand(
    TokenList* args,
    descriptor in,
    descriptor out,
    descriptor err
) {
    _resetCursor(args);
    TokenList* list = _newTokenList();
    Token* currentToken = _peekCurrentToken(args);
    ProgramOptions* array = NULL;
    i32 length = 0;
    while(currentToken != NULL) {
        if(currentToken->isControl && currentToken->data[0] == '|') {
            if(list->length == 0)
                continue;
            ProgramOptions tempOptions = getProgramOptions(list);
            ProgramOptions* oldArray = array;
            array = (ProgramOptions*)
                malloc(sizeof(ProgramOptions) * (length + 1));
            for(i32 i = 0; i < length; i++)
                array[i] = oldArray[i];
            array[length] = tempOptions;
            length++;
            free(oldArray);
        } else {
            _insertToken(list, *currentToken);
        }
        _stepForward(args);
        currentToken = _peekNextToken(args);
    }
    Pipeline pipeline = {
        array,
        length
    };
    runPipeline(pipeline, in, out, err);

    // clear up
    for(i32 i = 0; i < length; i++) {
        _destroyProgramOption(array[i]);
    }
    free(array);
    _destroyTokenList(list);
}

void runPipeline(
    Pipeline pipeline,
    descriptor input,
    descriptor output,
    descriptor error
) {
    if(pipeline.length == 0)
        return;
    if(pipeline.length == 1) {
        // run the single program
        pid pid = redirectAndRun(
            pipeline.options[0],
            input,
            output,
            error
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
            error
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
        error
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

    if(options.inputFilePath != NULL) {
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
        pthread_exit((void*)EXIT_FAILURE); // kill this process
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

