#ifndef _FLET_H
#define _FLET_H
#include <stdio.h>

#define NOHIGHLIGHT -1

typedef enum Token_type {
    // INT,
    // FLOAT,
    PRIMARY,
    UNARY,
    BINARY,
    FUNC,
    LBRACKET,
    RBRACKET,
    COMMA,
    NONE,
} Token_type;

typedef struct token_t {
    Token_type type;
    char* tokenstring;
    int len;
    int numargs;
    double value;
} token_t;


// tokenizer.c
void printtokens(token_t* tokens, int n, int highlight, FILE* fp);
token_t* tokenize(int argc, char** argv, int* ntokens);
void printtokens(token_t* tokens, int n, int highlight, FILE* fp);

// shutingyard.c
token_t* shuntingyard(token_t* input, int ninput, int* n);

// evaluator.x
void evaluate(token_t* postfix, int npostfix);

#endif /* flet.h */