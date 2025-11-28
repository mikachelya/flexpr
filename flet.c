#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <getopt.h>
#include "flet.h"


int main(int argc, char** argv) {
    // for (int i = 0; i < argc; i++) {
    //     printf("argv[%d] = '%s'\n", i, argv[i]);
    // }   

    if (argc == 1) {
        fprintf(stderr, "Usage: %s [OPTIONS] EXPRESSION.\nFor more information, use %s --help\n", argv[0], argv[0]);
        exit(argc == 1 ? 1 : 0);
    }

    param_t params;
    params.epsilon = DBL_EPSILON;
    params.digits = 20;
    params.integer = false;


    static struct option long_options[] = {
        {"version",  no_argument, 0, 'V'},
        {"help",     no_argument, 0, 'H'},
        {"tokenize", no_argument, 0, 'T'},
        {"postfix",  no_argument, 0, 'P'},
        {0,          0,           0,  0 },
    };

    bool done = false;
    while (!done) {
        char c = getopt_long(argc, argv, "+:VHTPIE:D:", long_options, NULL);
        switch(c) {
        case 'V':
            printf("0.0.0\n");
            exit(0);
        
        case 'H':
            fprintf(stderr, "Usage: %s [OPTIONS] EXPRESSION.\n", argv[0]);
            exit(0);
        
        case 'T':
            params.tokenize = true;
            break;
        
        case 'P':
            params.postfix = true;
            break;
        
        case 'I':
            params.integer = true;
            break;
        
        case 'E':
            params.epsilon = atof(optarg);
            break;

        case 'D':
            params.digits = atoi(optarg);
            break;

        case '?':
            done = true;
            optind--;
            break;
        
        default:
            done = true;
            break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [OPTIONS] EXPRESSION.\nFor more information, use %s --help\n", argv[0], argv[0]);
        exit(1);
    }

    int ntokens;
    token_t* tokenstream = tokenize(argc - optind, &argv[optind], &ntokens);
    if (params.tokenize)
        printtokens(tokenstream, ntokens, NOHIGHLIGHT, stdout);

    int npostfix;
    token_t* postfix = shuntingyard(tokenstream, ntokens, &npostfix);
    if (params.postfix)
        printtokens(postfix, npostfix, NOHIGHLIGHT, stdout);
    
    double result = evaluate(postfix, tokenstream, npostfix, ntokens, params);

    if (params.integer == 0)
        printf("%.*lf\n", params.digits, result);
    else
        printf("%ld\n", (long)result);


    free(tokenstream);
    free(postfix);

    return 0;
}
