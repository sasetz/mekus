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
        case '`':
        case '|': {
            output = CONTROL;
            break;
        }
        case '"': {
            output = PAIR_DELIMITER;
            break;
        }
        case '\n':
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
Token* _newToken(string tokenStart, string tokenEnd) {
    size_t length = tokenEnd - tokenStart;
    string data = (string) malloc(sizeof(char) * (length + 1));
    zero(data, length + 1);

    strncpy(data, tokenStart, length);

    Token* output = (Token*) malloc(sizeof(Token));
    output->data = data;
    output->length = length;

    return output;
}

void _deleteToken(Token* token) {
    free(token->data);
    free(token);
}

// startPos = position of the first quote
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
        &tokenizer->data[endQuotePos]
    );
}

Token* _nextToken(Tokenizer* tokenizer) {
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
                    return _newToken(startAddress, startAddress + 1);
                } else {
                    // a token has been started, finish it
                    return _newToken(startAddress, currentAddress);
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
                    return _newToken(startAddress, currentAddress + 1);
            }
            case SINGLE_DELIMITER: {
                if(tokenStart < tokenizer->position) {
                    tokenizer->position++;
                    // no +1 at the end address, since we don't need to
                    // capture the delimiter in the token
                    return _newToken(startAddress, currentAddress);
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
            &tokenizer->data[tokenizer->position]
        );
    }
    return NULL; // no tokens left to process
}

bool _hasNextToken(Tokenizer tokenizer) {
    return tokenizer.position < tokenizer.length;
}

Token* tokenizer(string data) {
    // TODO: implement this function
    return 0;
}

