#ifndef _FLET_H
#define _FLET_H
#include <stdio.h>
#include <stdbool.h>

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
    int tokenidx;
    double value;
} token_t;

typedef struct param_t {
    // bool version;   // -v --version
    // bool help;      // -h --help
    bool tokenize;  // -T --tokenize
    bool postfix;   // -P --postfix
    bool integer;   // -I
    double epsilon; // -E // int tolerance;
    int digits;     // -D
} param_t;


// tokenizer.c
void printtokens(token_t* tokens, int n, int highlight, FILE* fp);
token_t* tokenize(int argc, char** argv, int* ntokens);
void printtokens(token_t* tokens, int n, int highlight, FILE* fp);

// shutingyard.c
token_t* shuntingyard(token_t* input, int ninput, int* n);

// evaluator.x
double evaluate(token_t* input, token_t* original, int n, int noriginal, param_t params);

#endif /* flet.h */