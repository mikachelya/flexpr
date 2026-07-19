#ifndef _FEXPR_H
#define _FEXPR_H
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
    INVALID, // results in error
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
    // bool version;         // -v --version
    // bool help;            // -h --help
    bool tokenize;           // --tokenize
    bool postfix;            // --postfix
    // bool integer;         // -I
    double epsilon;          // -E
    long long tolerance;     // -T
    int digits;              // -D
} param_t;


// tokenizer.c
void printtokens(token_t* tokens, int n, int highlight, FILE* fp);
token_t* tokenize(int argc, char** argv, int* ntokens);

// shutingyard.c
token_t* shuntingyard(token_t* input, int ninput, int* n);

// evaluator.x
double evaluate(token_t* input, token_t* original, int n, int noriginal, param_t params);

#endif /* fexpr.h */