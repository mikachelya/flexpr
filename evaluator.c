#ifndef _EVALUATOR
#define _EVALUATOR

#include "flet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <assert.h>


#define strswitch(str, n) char* strswitch_cmp = str; int strswitch_cmplen = n; if(0)
#define strcase(other) else if (sizeof(other) - 1 == strswitch_cmplen && strncmp(strswitch_cmp, other, strswitch_cmplen) == 0)
#define ERROR(message)                                                                         \
    {                                                                                          \
        fprintf(stderr, "Error: %s \"%.*s\".\n", message, current->len, current->tokenstring); \
        printtokens(original, noriginal, current->tokenidx, stderr);                           \
        exit(3);                                                                               \
    }
#define ERR_UNARY "unknown unary operator"
#define ERR_UNARY_NOTHING "no operand given for unary operator"
#define ERR_BINARY "unknown binary operator"
#define ERR_BINARY_NOT_ENOUGH "not enough operands given for binary operator"
#define ERR_FUNC "unknown function for the given number of operands"
#define MZERO 0x8000000000000000 // bits corresponding to a double value of -0

typedef union bitcast {
    long long l;
    double d;
} bitcast;


// function adapted from https://staff.polito.it/claudio.fornaro/CorsoINF/Other%20interesting%20lectures/Comparing%20Floating%20Point%20Numbers.pdf
bool almostEqual2sComplement(double A, double B, param_t* param) {
    // make sure that the bitcast will work as intended
    static_assert(sizeof(long long) == sizeof(double));
    long long maxUlps = param->tolerance;
    
    bitcast a, b;
    a.d = A;
    b.d = B;

    // printf("%llx %llx\n", a.l, b.l);

    // Make a lexicographically ordered as a twos-complement int
    if (a.l < 0)
        a.l = MZERO - a.l;
    // Make b lexicographically ordered as a twos-complement int
    if (b.l < 0)
        b.l = MZERO - b.l;
    long long longdiff = llabs(a.l - b.l);
    if (longdiff <= maxUlps)
        return true;
    return false;
}


bool almostEqual(double A, double B, param_t* param) {
    if (A == B) return true;
    return fabs(A - B) <= fmax(fabs(A), fabs(B)) * param->epsilon;
}


double evaluate(token_t* input, token_t* original, int n, int noriginal, param_t params) {
    int top = -1;
    token_t* stack = (token_t*)malloc(n * sizeof(token_t));

    bool (*equal)(double, double, param_t*);
    if (params.tolerance >= 0) {
        equal = almostEqual2sComplement;
        // printf("Comparing using tolerance of %lld\n", params.tolerance);
    } else {
        equal = almostEqual;
        // printf("Comparing using epsilon of %lf\n", params.epsilon);
    }

    for (int i = 0; i < n; i++) {
        token_t* current = &input[i];

        switch (current->type) {
        case PRIMARY:
            stack[++top] = *current;
            break;
        
        case UNARY:
            if (current->len != 1)
                ERROR(ERR_UNARY);
            if (top < 0)
                ERROR(ERR_UNARY_NOTHING); // should be unreachable
            switch (current->tokenstring[0]) {
            case '+':
                break;

            case '-':
                stack[top].value *= -1;
                break;

            case '!':
                if (equal(stack[top].value, 0, &params))
                    stack[top].value = 0;
                stack[top].value = !stack[top].value;
                break;

            default:
                ERROR(ERR_UNARY);
            }
            break;
        
        case BINARY:
            if (top == 0)
                ERROR(ERR_BINARY_NOT_ENOUGH); // should be unreachable
            double operand = stack[top--].value;

            if (current->len == 1) {
                switch (current->tokenstring[0]) {
                case '+':
                    stack[top].value += operand;
                    break;
                case '-':
                    stack[top].value -= operand;
                    break;
                case '*':
                    stack[top].value *= operand;
                    break;
                case '/':
                    stack[top].value /= operand;
                    break;
                case '>':
                    stack[top].value = stack[top].value > operand && !equal(stack[top].value, operand, &params);
                    break;
                case '<':
                    stack[top].value = stack[top].value < operand && !equal(stack[top].value, operand, &params);
                    break;
                case '%':
                    double value = fmod(stack[top].value, operand);
                    double quotient = stack[top].value / operand;
                    if (equal(round(quotient), quotient, &params))
                        value = 0;
                    stack[top].value = value;
                    break;

                default:
                    ERROR(ERR_BINARY);
                }
            }
            else if (current->len == 2) {
                strswitch(current->tokenstring, 2);
                strcase("==")
                    stack[top].value = (double)equal(stack[top].value, operand, &params);
                strcase("!=")
                    stack[top].value = (double)(!equal(stack[top].value, operand, &params));
                strcase(">=")
                    stack[top].value = stack[top].value > operand || equal(stack[top].value, operand, &params);
                strcase("<=")
                    stack[top].value = stack[top].value < operand || equal(stack[top].value, operand, &params);
                strcase("&&") {
                    if (equal(stack[top].value, 0, &params))
                        stack[top].value = 0;
                    if (equal(operand, 0, &params))
                        operand = 0;
                    stack[top].value = stack[top].value && operand;
                }
                strcase("||") {
                    if (equal(stack[top].value, 0, &params))
                        stack[top].value = 0;
                    if (equal(operand, 0, &params))
                        operand = 0;
                    stack[top].value = stack[top].value || operand;
                }
                else ERROR(ERR_BINARY);
            }
            else ERROR(ERR_BINARY);
            break;

        case FUNC:
            bool evalled = false;
            if (current->numargs == 1) {
                evalled = true;
                double operand = stack[top].value;
                strswitch(current->tokenstring, current->len);
                strcase("abs")
                    stack[top].value = fabs(operand);
                strcase("sin")
                    stack[top].value = sin(operand);
                strcase("cos")
                    stack[top].value = cos(operand);
                strcase("tan")
                    stack[top].value = tan(operand);
                strcase("asin")
                    stack[top].value = asin(operand);
                strcase("acos")
                    stack[top].value = acos(operand);
                strcase("atan")
                    stack[top].value = atan(operand);
                strcase("sqrt")
                    stack[top].value = sqrt(operand);
                strcase("exp")
                    stack[top].value = exp(operand);
                strcase("ln")
                    stack[top].value = log(operand);
                strcase("log")
                    stack[top].value = log10(operand);
                strcase("floor")
                    stack[top].value = floor(operand);
                strcase("ceil")
                    stack[top].value = ceil(operand);
                strcase("round")
                    stack[top].value = round(operand);
                else evalled = false;
            }

            if (current->numargs == 2) {
                evalled = true;
                double operand = stack[top].value;
                strswitch(current->tokenstring, current->len);
                strcase("pow") {
                    double result = pow(stack[--top].value, operand);
                    stack[top].value = result;
                }
                else evalled = false;
            }

            if (!evalled) {
                strswitch(current->tokenstring, current->len);
                strcase("min") {
                    double min = DBL_MAX;
                    for (int j = 0; j < current->numargs; j++) {
                        if (stack[top].value < min)
                            min = stack[top].value;
                        top--;
                    }
                    stack[++top].value = min;
                }
                strcase("max") {
                    double max = -DBL_MAX;
                    for (int j = 0; j < current->numargs; j++) {
                        if (stack[top].value > max)
                            max = stack[top].value;
                        top--;
                    }
                    stack[++top].value = max;
                }
                strcase("sum") {
                    double sum = 0;
                    for (int j = 0; j < current->numargs; j++)
                        sum += stack[top--].value;
                    stack[++top].value = sum;
                }
                strcase("prod") {
                    double prod = 1;
                    for (int j = 0; j < current->numargs; j++)
                        prod *= stack[top--].value;
                    stack[++top].value = prod;
                }
                strcase("any") {
                    double res = 0;
                    for (int j = 0; j < current->numargs; j++) {
                        if (!equal(stack[top--].value, 0, &params))
                            res = 1;
                    }
                    stack[++top].value = res;
                }
                strcase("all") {
                    double res = 1;
                    for (int j = 0; j < current->numargs; j++) {
                        if (equal(stack[top--].value, 0, &params))
                            res = 0;
                    }
                    stack[++top].value = res;
                }
                else ERROR(ERR_FUNC);
            }

            break;

        default:
            printf("Internal error: invalid token type (type=%d)\n", current->type);
            exit(4);
        }
    }

    return stack[top].value;
}



#endif