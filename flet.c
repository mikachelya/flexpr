#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include "flet.h"
#include "help.h"

#define VERSION "1.0.0"
#define USAGE "Usage: %s [OPTIONS]... EXPRESSION\n"
#define ERR_NO_EXPRESSION                                                                  \
    {                                                                                      \
        fprintf(stderr, USAGE"Try '%s --help' for more information.\n", argv[0], argv[0]); \
        exit(1);                                                                           \
    }

int main(int argc, char** argv) {
    if (argc == 1)
        ERR_NO_EXPRESSION;

    param_t params;
    params.epsilon = DBL_EPSILON;
    // params.epsilon = -1.0; 
    params.tolerance = -1; 
    params.digits = 20;
    params.tokenize = false;
    params.postfix = false;

    static struct option long_options[] = {
        {"version",   no_argument,       0, 'V'},
        {"help",      no_argument,       0, 'H'},
        {"tokenize",  no_argument,       0, 't'},
        {"postfix",   no_argument,       0, 'p'},
        {"integer",   no_argument,       0, 'I'},
        {"digits",    required_argument, 0, 'D'},
        {"tolerance", required_argument, 0, 'T'},
        {"epsilon",   required_argument, 0, 'E'},
        {0,           0,                 0,  0 },
    };

    bool done = false;
    while (!done) {
        // "+" at the start of the string ensures argument parsing stops when a non-argument token is reached
        // the first ":" silences arror printing
        char c = getopt_long(argc, argv, "+:VHT:IE:D:", long_options, NULL);
        switch(c) {
        case 'V':
            printf("%s\n", VERSION);
            exit(0);
        
        case 'H':
            fprintf(stdout, USAGE, argv[0]);
            fprintf(stdout, HELP, argv[0], argv[0]);
            exit(0);

        case 'T':
            params.tolerance = atoll(optarg);
            params.epsilon = -1.0;
            // Make sure maxUlps is non-negative and small enough that the
            // default NAN won't compare as equal to anything.
            // TODO: move to arAlmostEqual2sComplementg parser, verify the 1024*1024*4 value
            //       I believe the correct value for doubles should be 2^51
            if (params.tolerance < 0 || params.tolerance >= 1LL << 51) {
                fprintf(stderr, "Invalid tolerance. (0 <= T < 2^51)\n");
                exit(1);
            }
            break;
        
        case 't':
            params.tokenize = true;
            break;
        
        case 'p':
            params.postfix = true;
            break;

        case 'I':
            params.digits = 0;
            break;
        
        case 'E':
            params.epsilon = atof(optarg);
            params.tolerance = -1;
            break;

        case 'D':
            params.digits = atoi(optarg);
            break;

        case '?':
            // printf("stopped on arg %d\n", optind);
            int prevarg = optind - 1;
            if (prevarg == 0) {
                done = true;
                break;
            }
       
            if (strcmp(argv[prevarg], "--") != 0 && argv[prevarg][0] == '-' &&
                (isdigit(argv[prevarg][1]) || isdigit(argv[prevarg][2]) || argv[prevarg][2] == 0)) {
                optind--;
            }

            done = true;
            break;
        
        default:
            done = true;
            break;
        }
    }

    if (optind >= argc)
        ERR_NO_EXPRESSION;

    int ntokens;
    token_t* tokenstream = tokenize(argc - optind, &argv[optind], &ntokens);
    if (params.tokenize)
        printtokens(tokenstream, ntokens, NOHIGHLIGHT, stdout);

    int npostfix;
    token_t* postfix = shuntingyard(tokenstream, ntokens, &npostfix);
    if (params.postfix)
        printtokens(postfix, npostfix, NOHIGHLIGHT, stdout);
    
    double result = evaluate(postfix, tokenstream, npostfix, ntokens, params);

    printf("%.*lf\n", params.digits, result);


    free(tokenstream);
    free(postfix);

    return 0;
}
