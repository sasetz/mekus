#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "base.h"

// current state of the tokenizer
typedef struct {
    string data;
    size_t length;
    size_t position;
} Tokenizer;

// token is a representation of each individual argument that was passed to the
// shell, without delimiters, spaces, quotes, etc.
typedef struct {
    string contents;
    size_t length;
} Token;

typedef enum {
    REGULAR,
    CONTROL,
    COMMENT,
    ESCAPE,
    PAIR_DELIMITER,
    SINGLE_DELIMITER,
} CharacterType;

Tokenizer _newTokenizer(string data);
void _deleteTokenizer(Tokenizer tokenizer);

// produces next token
Token _nextToken(Tokenizer* tokenizer);

// transforms the input into an array of tokens
// returns NULL on an error
Token* tokenize(string data);

#endif /* end of include guard: TOKENIZER_H */

// vim: filetype=c
