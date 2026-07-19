#include "fexpr.h"
#include <stdio.h>
#include <stdlib.h>

#define ERROR(message)                                \
    {                                                 \
        fprintf(stderr, "Error: %s.\n", message);     \
        printtokens(input, ninput, i, stderr);        \
        exit(2);                                      \
    }
#define ERR_INVALID "invalid numeric literal"
#define ERR_OPERATOR "expected operator"
#define ERR_OPERAND "expected operand"
#define ERR_BRACE "expected opening parentheses"
#define ERR_ARG "empty argument"
#define ERR_EMPTY "empty parentheses"
#define ERR_UNBALANCED "unbalanced parentheses"
#define ERR_COMMA "comma outside of function call"

token_t* shuntingyard(token_t* input, int ninput, int* n) {
    int noutput = 0, top = -1;
    token_t* output = (token_t*)malloc(ninput * sizeof(token_t));
    token_t* stack = (token_t*)malloc(ninput * sizeof(token_t));

    char precedence[128];
    precedence['|'] = 1;
    precedence['&'] = 2;
    precedence['='] = 3;
    precedence['!'] = 3;
    precedence['<'] = 4;
    precedence['>'] = 4;
    precedence['-'] = 5;
    precedence['+'] = 5;
    precedence['/'] = 10;
    precedence['*'] = 10;

    token_t* lastfunc = NULL;
    Token_type prevtype = NONE;

    int i;
    for (i = 0; i < ninput; i++) {
        token_t* current = &input[i];

        if (current->type == PRIMARY || current->type == FUNC || current->type == LBRACKET)
            if (prevtype == PRIMARY || prevtype == RBRACKET)
                ERROR(ERR_OPERATOR);
        if (current->type == PRIMARY || current->type == FUNC || current->type == UNARY)
            if (prevtype == FUNC)
                ERROR(ERR_BRACE);
        if (current->type == RBRACKET && prevtype == LBRACKET)
            ERROR(ERR_EMPTY);
        if (prevtype == UNARY && current->type != LBRACKET && current->type != FUNC && current->type != PRIMARY)
            ERROR(ERR_OPERAND);
        if (current->type == INVALID)
            ERROR(ERR_INVALID);

        switch (current->type) {
        case PRIMARY:
            output[noutput++] = *current;
            break;

        case FUNC:
            stack[++top] = *current;
            lastfunc = &stack[top];
            lastfunc->numargs = 1;
            break;

        case UNARY:
            if (prevtype == UNARY)
                ERROR(ERR_OPERAND);
            current->numargs = 1;
            stack[++top] = *current;
            break;
        
        case BINARY:
            if (prevtype == LBRACKET || prevtype == COMMA || i == ninput - 1)
                ERROR(ERR_OPERAND);
            while (top >= 0 && stack[top].type != LBRACKET 
                && precedence[(int)(stack[top].tokenstring[0])] >= precedence[(int)(current->tokenstring[0])])
                output[noutput++] = stack[top--];
            current->numargs = 2;
            stack[++top] = *current;
            break;
        
        case COMMA:
            if (prevtype == COMMA)
                ERROR(ERR_ARG);
            if (lastfunc == NULL) // TODO: fexpr "sum(sin(1), 2)" forgets about the sum
                ERROR(ERR_COMMA);
            while (top >= 0 && stack[top].type != LBRACKET)
                output[noutput++] = stack[top--];
            lastfunc->numargs++;
            break;

        case LBRACKET:
            stack[++top] = *current;
            break;
        
        case RBRACKET:
            if (prevtype == BINARY || prevtype == UNARY || prevtype == COMMA)
                ERROR(ERR_OPERAND);
            while (top >= 0 && stack[top].type != LBRACKET)
                output[noutput++] = stack[top--];
            // manage mismatched parens
            if (stack[top].type != LBRACKET)
                ERROR(ERR_UNBALANCED);
            // trash the opening paren
            top--;
            if (stack[top].type == FUNC) {
                output[noutput++] = stack[top--];
                // find the top function
                lastfunc = NULL;
                for (int i = top; i >= 0; i--) {
                    if (stack[top].type == FUNC) {
                        lastfunc = &stack[top];
                        break;
                    }
                }
            }
            break;
        
        default:
            printf("Internal error: invalid token type (type=%d)\n", current->type);
            exit(4);
        }

        prevtype = current->type;
    }

    if (prevtype == UNARY)
        ERROR(ERR_OPERAND);

    while (top >= 0) {
        //mismatched parens
        if (stack[top].type == LBRACKET) {
            i--;
            ERROR(ERR_UNBALANCED);
        }
        output[noutput++] = stack[top--];
    }

    *n = noutput;
    free(stack);
    return output;
}
