#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "flet.h"


int main(int argc, char** argv) {
    // for (int i = 0; i < argc; i++) {
    //     printf("arg[%d] = '%s'\n", i, argv[i]);
    // }   

    if (argc == 1) {
        fprintf(stderr, "Usage: %s [arguments].\nFor more information, use %s --help\n", argv[0], argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [arguments].\n", argv[0]);
        exit(0);
    }
    
    param_t params;
    params.epsilon = DBL_EPSILON;
    // params.epsilon = 0.001;
    params.ndigits = 10;
    params.integer = 0;

    int ntokens;
    token_t* tokenstream = tokenize(argc - 1, &argv[1], &ntokens);
    int npostfix;
    token_t* postfix = shuntingyard(tokenstream, ntokens, &npostfix);
    evaluate(postfix, tokenstream, npostfix, ntokens, params);

    free(tokenstream);
    free(postfix);
}