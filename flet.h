#ifndef _FLET_H
#define _FLET_H
#include <stdio.h>

#define NOHIGHLIGHT -1

typedef enum Token_type Token_type;
typedef struct token_t token_t;

void printtokens(token_t* tokens, int n, int highlight, FILE* fp);
token_t* tokenize(int argc, char** argv, int* ntokens);
void printtokens(token_t* tokens, int n, int highlight, FILE* fp);
void destroytoken(token_t* token);
void destroytokenstream(token_t* tokenstream, int n);

#endif /* flet.h */