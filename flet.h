#ifndef _FLET_H
#define _FLET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define __USE_MISC 1
#include <math.h>


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

typedef struct {
    Token_type type;
    char* tokenstring;
    int len;
    int numargs;
    double value;
} token_t;

void printtokens(token_t* tokens, int n, int highlight, FILE* fp);


token_t* tokenize(int argc, char** argv, int* ntokens) {
    int currentarg = 0, n = 0;
    char* current = argv[0];
    int capacity = 16;
    Token_type prevtype = NONE;


    token_t* tokenstream = (token_t*)malloc(capacity * sizeof(token_t));
    while (1) {
        bool done = false;
        while (1) {
            if (*current == 0) {
                currentarg++;
                if (currentarg >= argc) {
                    done = true;
                    break;
                }
                current = argv[currentarg];
            }
            else {
                while (isblank(*current) || *current == '\n' || *current == '\\' || *current == '\"')
                current++;
                if (*current == 0)
                    continue;
                else
                    break;
            }
        }
        if (done) break;


        if (n >= capacity) {
            capacity *= 2;
            tokenstream = (token_t*)realloc(tokenstream, capacity * sizeof(token_t));
        }
        token_t* currenttoken = &tokenstream[n++];
        
        int len = 0;
        
        if ((*current >= '0' && *current <= '9') || *current == '.') {
            // read decimal number
            // printf("read decimal ");
            char *end;
            float value = strtod(current, &end);
            len = end - current;
            // printf("len=%d ", len);
            if (len == 0) {
                len = strlen(current);
                currenttoken->tokenstring = (char*)malloc((len + 1) * sizeof(char));
                strcpy(currenttoken->tokenstring, current);
                printtokens(tokenstream, n - 1, n - 1, stderr);
            }
            currenttoken->value = value;
            currenttoken->type = PRIMARY;
        }

        else if (current[0] == 'p' && current[1] == 'i' && !isalpha(current[2])) {
            // read pi
            // printf("read pi ");
            len = 2;
            currenttoken->value = M_PI;
            currenttoken->type = PRIMARY;
        }

        else if (current[0] == 'e' && !isalpha(current[1])) {
            // read e
            // printf("read e ");
            len = 1;
            currenttoken->value = M_E;
            currenttoken->type = PRIMARY;
        }

        else if (isalpha(*current)) {
            // read func
            // printf("read func ");
            len = 0;
            while (isalpha(current[len]))
                len++;
            currenttoken->type = FUNC;
        }

        else {
            // read operator
            // printf("read operator ");
            len = 1;

            if (*current == '(')
                currenttoken->type = LBRACKET;
            else if (*current == ')')
                currenttoken->type = RBRACKET;
            else if (*current == ',')
                currenttoken->type = COMMA;
            else if (prevtype == PRIMARY || prevtype == RBRACKET) {
                currenttoken->type = BINARY;
                currenttoken->numargs = 2;
                if (current[1] == '=')
                    len++;
            }
            else {
                currenttoken->type = UNARY;
                currenttoken->numargs = 1;
            }
            
        }

        char* tokenstring = (char*)malloc((len + 1) * sizeof(char));
        memcpy(tokenstring, current, len * sizeof(char));
        tokenstring[len] = 0;
        currenttoken->len = len;
        currenttoken->tokenstring = tokenstring;
        prevtype = currenttoken->type;
        

        // printf("current=%s\n", current);
        current += len;
        // if (*current == 0) {
        //     currentarg++;
        //     if (currentarg >= argc)
        //         break;
        //     current = argv[currentarg];
        // }
    }

    printtokens(tokenstream, n, NOHIGHLIGHT, stdout);

    *ntokens = n;
    return tokenstream;
}


void printtokens(token_t* tokens, int n, int highlight, FILE* fp) {
    int printed = 0, highlightlen = 0;
    for (int i = 0; i < n; i++) {
        token_t current = tokens[i];
        int len = fprintf(fp, "%s", current.tokenstring);
        bool space = true;
        
        if (current.type == LBRACKET || current.type == FUNC
            || (i < n - 1 && (tokens[i + 1].type == COMMA || tokens[i + 1].type == RBRACKET))) {
            space = false;
        }
        if (space)
            putc(' ', fp);

        if (highlight != NOHIGHLIGHT && i < highlight)
            printed += len + space;
        if (i == highlight)
            highlightlen = len;
    }
    
    putc('\n', fp);

    if (highlight != NOHIGHLIGHT) {
        for (int i = 0; i < printed; i++)
            putc(' ', fp);
        for (int i = 0; i < highlightlen; i++)
            putc('^', fp);
        putc('\n', fp);
    }
}


void destroytoken(token_t* token) {
    free(token->tokenstring);
}


void destroytokenstream(token_t* tokenstream, int n) {
    for (int i = 0; i < n; i++) {
        destroytoken(&tokenstream[i]);
    }
    free(tokenstream);
}


#endif /* flet.h */