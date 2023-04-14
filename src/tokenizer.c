#include <malloc.h>
#include <string.h>

#include "tokenizer.h"

Tokenizer _newTokenizer(string data) {
    size_t length = strlen(data); // this length does not count \0
    string outputData = strndup(data, length);
    return (Tokenizer){
        outputData,
        length,
        0
    };
}

CharacterType characterData(char character) {
    CharacterType output;
    switch (character) {
        case '#': {
            output = COMMENT;
            break;
        }
        case '\\': {
            output = ESCAPE;
            break;
        }
        case '<':
        case '>':
        case '&':
        case ';':
        case '\n':
        case '|': {
            output = CONTROL;
            break;
        }
        case '"': {
            output = PAIR_DELIMITER;
            break;
        }
        case ' ': {
            output = SINGLE_DELIMITER;
            break;
        }
        default: {
            output = REGULAR;
        }
    }
    return output;
}

void _deleteTokenizer(Tokenizer tokenizer) {
    free(tokenizer.data);
}

// tokenStart = start address of the string
//
// tokenEnd = end address of the string, character under this
//      address will NOT be included to the string
//
// isControl = is the token control, or just text one
Token* _newToken(string tokenStart, string tokenEnd, bool isControl) {
    size_t length = tokenEnd - tokenStart;
    string data = (string) malloc(sizeof(char) * (length + 1));
    zero(data, length + 1);

    strncpy(data, tokenStart, length);

    Token* output = (Token*) malloc(sizeof(Token));
    output->data = data;
    output->isControl = isControl;

    return output;
}

void _destroyToken(Token* token) {
    free(token->data);
    free(token);
}

// startPos = position of the first quote
// TODO: add escaping of the quote
Token* getTokenBetweenQuotes(Tokenizer* tokenizer) {
    size_t startQuotePos = tokenizer->position;

    while(
        tokenizer->position < tokenizer->length &&
        tokenizer->data[tokenizer->position] != '"'
    ) tokenizer->position++;

    size_t endQuotePos = tokenizer->position;
    tokenizer->position++; // step forward not to check the quote next again

    return _newToken(
        &tokenizer->data[startQuotePos + 1],
        &tokenizer->data[endQuotePos],
        FALSE
    );
}

Token* _produceNextToken(Tokenizer* tokenizer) {
    size_t tokenStart = tokenizer->position;

    while(tokenizer->position < tokenizer->length) {
        char character = tokenizer->data[tokenizer->position];
        string startAddress = tokenizer->data + tokenStart;
        string currentAddress = tokenizer->data + tokenizer->position;
        switch (characterData(character)) {
            case REGULAR: {
                tokenizer->position++;
                continue;
            }
            case CONTROL: {
                if(tokenStart == tokenizer->position) {
                    tokenizer->position++;
                    // get the address of the character
                    return _newToken(startAddress, startAddress + 1, TRUE);
                } else {
                    // a token has been started, finish it
                    return _newToken(startAddress, currentAddress, FALSE);
                }
                break;
            }
            case COMMENT: {
                // the whole line is read until the end
                tokenizer->position = tokenizer->length;
                return NULL; // NULL since no more tokens for this line
                break;
            }
            case ESCAPE: {
                tokenizer->position += 2; // skip the next character
                continue;
            }
            case PAIR_DELIMITER: {
                // if we encountered the quote right away, process it
                if(tokenStart == tokenizer->position)
                    return getTokenBetweenQuotes(tokenizer);
                // if we had some token before, separate it and leave
                // the quote for later
                else
                    return _newToken(startAddress, currentAddress + 1, FALSE);
            }
            case SINGLE_DELIMITER: {
                if(tokenStart < tokenizer->position) {
                    tokenizer->position++;
                    // no +1 at the end address, since we don't need to
                    // capture the delimiter in the token
                    return _newToken(startAddress, currentAddress, FALSE);
                }
                tokenizer->position++;
                tokenStart = tokenizer->position;
                break;
            }
        }
    }
    // in case that we run out of symbols to process
    if(tokenStart < tokenizer->position) {
        // unfinished token
        return _newToken(
            &tokenizer->data[tokenStart],
            &tokenizer->data[tokenizer->position],
            FALSE
        );
    }
    return NULL; // no tokens left to process
}

bool _hasNextToken(Tokenizer tokenizer) {
    return tokenizer.position < tokenizer.length;
}

TokenNode* _newTokenNode(Token token) {
    TokenNode* output = (TokenNode*) malloc(sizeof(TokenNode));

    output->previous = NULL;
    output->next = NULL;

    Token* tokenCopy = (Token*) malloc(sizeof(Token));
    size_t length = strlen(token.data);

    tokenCopy->data = (string) malloc(sizeof(char) * (length + 1));
    tokenCopy->data[length] = 0;
    strncpy(tokenCopy->data, token.data, length);
    tokenCopy->isControl = token.isControl;

    output->token = tokenCopy;
    return output;
}

void _destroyTokenNode(TokenNode* tokenNode) {
    _destroyToken(tokenNode->token);
    free(tokenNode);
}

TokenList* _newTokenList() {
    TokenList* output = (TokenList*) malloc(sizeof(TokenList));

    output->length = 0;
    output->start = NULL;
    output->end = NULL;
    output->cursor = NULL;

    return output;
}

void _destroyTokenList(TokenList* tokenList) {
    // traverse
    TokenNode* current = tokenList->start;
    TokenNode* nextOne = current->next;
    while(current != NULL) {
        _destroyTokenNode(current);
        current = nextOne;

        if(current == NULL)
            break;
        nextOne = current->next;
    }

    free(tokenList);
}

void _insertToken(TokenList* tokenList, Token token) {
    TokenNode* newNode = _newTokenNode(token);

    if(tokenList->length == 0) {
        tokenList->length = 1;
        tokenList->start = newNode;
        tokenList->end = newNode;
        tokenList->cursor = newNode;
        return;
    }

    newNode->previous = tokenList->end;
    tokenList->end = newNode;
    tokenList->length++;
}

Token* _peekCurrentToken(TokenList* tokenList) {
    if(tokenList->cursor == NULL)
        return NULL;

    return tokenList->cursor->token;
}

Token* _peekPreviousToken(TokenList* tokenList) {
    if(tokenList->cursor == NULL)
        return NULL;
    if(tokenList->cursor->previous == NULL)
        return NULL;

    return tokenList->cursor->previous->token;
}

Token* _peekNextToken(TokenList* tokenList) {
    if(tokenList->cursor == NULL)
        return NULL;
    if(tokenList->cursor->next == NULL)
        return NULL;

    return tokenList->cursor->next->token;
}

void _stepForward(TokenList* tokenList) {
    tokenList->cursor = tokenList->cursor->next;
}

void _resetCursor(TokenList* tokenList) {
    tokenList->cursor = tokenList->start;
}

string* _toStringArray(TokenList* tokenList) {
    string* args = (string*) malloc(sizeof(tokenList->length + 1));
    args[tokenList->length] = 0; // null-terminate the array

    // traverse
    TokenNode* current = tokenList->start;
    TokenNode* nextOne = current->next;
    i32 i = 0;
    while(current != NULL) {
        i32 length = strlen(current->token->data);
        args[i] = (string) malloc(sizeof(char) * (length + 1));
        args[i][length] = '\0'; // null-terminate the copied string
        strncpy(args[i], current->token->data, length);

        current = nextOne;

        if(current == NULL)
            break;
        nextOne = current->next;
        i++;
    }

    return args;
}

