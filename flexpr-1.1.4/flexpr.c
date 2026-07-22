#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include "flexpr.h"
#include "help.h"

#define VERSION "1.1.4"
#define USAGE "Usage: %s [OPTIONS]... EXPRESSION\n"
#define ERR_NO_EXPRESSION                                                                  \
    {                                                                                      \
        fprintf(stderr, USAGE"Try '%s --help' for more information.\n", argv[0], argv[0]); \
        exit(4);                                                                           \
    }

bool is_negative_operand(const char *str) {
    if (str[0] != '-' || str[1] == '\0') return false;
    return !isupper(str[1]);
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
    params.logical = false;

    static struct option long_options[] = {
        {"version",   no_argument,       0, 'V'},
        {"help",      no_argument,       0, 'H'},
        {"tokenize",  no_argument,       0, 't'},
        {"logical",   no_argument,       0, 'L'},
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
        char c = getopt_long(argc, argv, "+:VHT:LIE:D:", long_options, NULL);
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
        
        case 'L':
            params.logical = true;
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

        // encountered unknown argument
        // if the argument was a single character (such as -e), it is consumed.
        // in that case we need to decrement optind.
        case '?':
            // printf("stopped on arg %d, optopt %c\n", optind, optopt);
            // 1. Hard error on invalid uppercase flags (-F, -IF, etc.)
            if (optopt != 0 && isupper((unsigned char)optopt)) {
                fprintf(stderr, "Error: Unknown option '-%c'\n", optopt);
                exit(4);
            }

            // 2. Check if the error happened inside an attached cluster (e.g. -I1 or -Ae)
            // If argv[optind - 1] starts with '-' and has more than 2 chars, 
            // it means valid flags ran before hitting this character in the same string!
            char *prev_arg = argv[optind - 1];
            if (prev_arg[0] == '-' && strlen(prev_arg) > 2) {
                if (optopt != 0)
                    fprintf(stderr, "Error: Unknown option '-%c' in cluster '%s'\n", optopt, prev_arg);
                else
                    fprintf(stderr, "Error: Unknown long option '%s'\n", prev_arg);
                exit(4);
            }

            // 3. It's a standalone negative operand (-1, -1+1, -e)
            // Rewind optind ONLY if getopt consumed the standalone token (e.g. "-1")
            if (prev_arg[0] == '-' && prev_arg[1] == optopt && prev_arg[2] == '\0') {
                optind--;
            }
            done = true;
            break;
        
        // argument parsing finished normally
        default:
            // printf("default on arg %d\n", optind);

            done = true;
            break;
        }
    }

    if (optind >= argc)
        ERR_NO_EXPRESSION;

    int ntokens;
    token_t* tokenstream = tokenize(argc - optind, &argv[optind], &ntokens);
    if (ntokens == 0)
        ERR_NO_EXPRESSION;
    if (params.tokenize)
        printtokens(tokenstream, ntokens, NOHIGHLIGHT, stdout);

    int npostfix;
    token_t* postfix = shuntingyard(tokenstream, ntokens, &npostfix);
    if (params.postfix)
        printtokens(postfix, npostfix, NOHIGHLIGHT, stdout);
    
    double result = evaluate(postfix, tokenstream, npostfix, ntokens, params);

    if (!params.logical)
        printf("%.*lf\n", params.digits, result);

    free(tokenstream);
    free(postfix);

    if (params.logical)
        return !result;

    return 0;
}
