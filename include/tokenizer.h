#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "base.h"

// current state of the tokenizer
typedef struct {
    string data; // always contains a copy, not a reference
    size_t length;
    size_t position;
} Tokenizer;

// token is a representation of each individual argument that was passed to the
// shell, without delimiters, spaces, quotes, etc.
typedef struct {
    string data; // always contains a copy, not a reference
    size_t length;
    bool isControl; // is the token controlling one or not
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
Token* _nextToken(Tokenizer* tokenizer);

// true if the tokenizer has more stuff to process
bool _hasNextToken(Tokenizer tokenizer);

Token* _newToken(string tokenStart, string tokenEnd, bool isControl);
void _deleteToken(Token* token);

// return input character's type
CharacterType characterData(char character);

#endif /* end of include guard: TOKENIZER_H */

// vim: filetype=c
