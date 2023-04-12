#include <malloc.h>
#include <string.h>

#include "tokenizer.h"

Tokenizer _newTokenizer(string data) {
    size_t length = strlen(data); // this length does not count \0
    string outputData = (string) malloc(sizeof(char) * (length + 1) );
    return (Tokenizer){
        outputData,
        length,
        0
    };
}

void _deleteTokenizer(Tokenizer tokenizer) {
    free(tokenizer.data);
}

Token _nextToken(Tokenizer* tokenizer) {
    // TODO: implement this function
    return (Token){ 0 };
}

Token* tokenizer(string data) {
    // TODO: implement this function
    return 0;
}

