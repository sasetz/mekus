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
    bool isControl; // is the token controlling one or not
} Token;

// node of token linked list
typedef struct _tokennode {
    struct _tokennode* previous;
    struct _tokennode* next;
    Token* token;
} TokenNode;

// linked list of tokens
typedef struct {
    TokenNode* start;
    TokenNode* end;
    TokenNode* cursor;
    i32 length;
} TokenList;

typedef enum {
    REGULAR,
    CONTROL,
    COMMENT,
    ESCAPE,
    PAIR_DELIMITER,
    SINGLE_DELIMITER,
} CharacterType;

// return input character's type
CharacterType characterData(char character);

// ------------ token ------------
//
Token* _newToken(string tokenStart, string tokenEnd, bool isControl);

void _destroyToken(Token* token);

// ------------ tokenizer ------------
//
Tokenizer _newTokenizer(string data);
void _deleteTokenizer(Tokenizer tokenizer);

Token* _produceNextToken(Tokenizer* tokenizer);

// true if the tokenizer has more stuff to process
bool _hasNextToken(Tokenizer tokenizer);

// ------------ token node ------------
//
TokenNode* _newTokenNode(Token token);
void _destroyTokenNode(TokenNode* tokenNode);

// ------------ token linked list ------------
//
TokenList* _newTokenList(); // empty token list
void _destroyTokenList(TokenList* tokenList);

// frees the token later, no need to do it yourself
void _insertToken(TokenList* tokenList, Token token);

// get next token under the cursor, NULL when no next tokens left
Token* _peekPreviousToken(TokenList* tokenList);
Token* _peekCurrentToken(TokenList* tokenList);
Token* _peekNextToken(TokenList* tokenList);
void _stepForward(TokenList* tokenList);
void _resetCursor(TokenList* tokenList);
string* _toStringArray(TokenList* tokenList);

#endif /* end of include guard: TOKENIZER_H */

// vim: filetype=c
