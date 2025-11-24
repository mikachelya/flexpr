#ifndef _EVALUATOR
#define _EVALUATOR

#include "flet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>


#define strswitch(str, n) char* _tocompare = str; int _cmplen = n; if(0)
#define strcase(other) else if (sizeof(other) - 1 == _cmplen && strncmp(_tocompare, other, _cmplen) == 0)
#define ERROR(message)                                                                         \
    {                                                                                          \
        fprintf(stderr, "Error: %s \"%.*s\".\n", message, current->len, current->tokenstring); \
        printtokens(original, noriginal, current->tokenidx, stderr);                           \
        exit(3);                                                                               \
    }
#define ERR_UNARY "unknown unary operator"
#define ERR_BINARY "unknown binary operator"
#define ERR_FUNC "unknown function for the given number of operands"


double evaluate(token_t* input, token_t* original, int n, int noriginal, param_t params) {
    int top = -1;
    double epsilon = params.epsilon;
    token_t* stack = (token_t*)malloc(n * sizeof(token_t));

    for (int i = 0; i < n; i++) {
        token_t* current = &input[i];

        switch (current->type) {
        case PRIMARY:
            stack[++top] = *current;
            break;
        
        case UNARY:
            if (current->len != 1)
                ERROR(ERR_UNARY);
            switch (current->tokenstring[0]) {
            case '+':
                break;

            case '-':
                stack[top].value *= -1;
                break;

            case '!':
                if (fabs(stack[top].value) < epsilon)
                    stack[top].value = 0;
                stack[top].value = !stack[top].value;
                break;

            default:
                ERROR(ERR_UNARY);
            }
            break;
        
        case BINARY:
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
                    stack[top].value = stack[top].value > operand + epsilon;
                    break;
                case '<':
                    stack[top].value = stack[top].value < operand - epsilon;
                    break;
                case '%':
                    double value = fmod(stack[top].value, operand);
                    if (fabs(value - operand) < epsilon)
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
                    stack[top].value = stack[top].value < operand + epsilon 
                                    && stack[top].value > operand - epsilon;
                strcase("!=")
                    stack[top].value = stack[top].value < operand - epsilon 
                                    || stack[top].value > operand + epsilon;
                strcase(">=")
                    stack[top].value = stack[top].value > operand - epsilon;
                strcase("<=")
                    stack[top].value = stack[top].value < operand + epsilon;
                strcase("&&") {
                    if (fabs(stack[top].value) < epsilon)
                        stack[top].value = 0;
                    if (fabs(operand) < epsilon)
                        operand = 0;
                    stack[top].value = stack[top].value && operand;
                }
                strcase("||") {
                    if (fabs(stack[top].value) < epsilon)
                        stack[top].value = 0;
                    if (fabs(operand) < epsilon)
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
                double operand = stack[top--].value;
                strswitch(current->tokenstring, current->len);
                strcase("pow")
                    stack[top].value = pow(stack[top].value, operand);
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
                        if (fabs(stack[top--].value) >= epsilon)
                            res = 1;
                    }
                    stack[++top].value = res;
                }
                strcase("all") {
                    double res = 1;
                    for (int j = 0; j < current->numargs; j++) {
                        if (fabs(stack[top--].value) < epsilon)
                            res = 0;
                    }
                    stack[++top].value = res;
                }
                else ERROR(ERR_FUNC);
            }

            break;

        default:
            printf("Error: invalid token type (type=%d)\n", current->type);
            exit(1);
        }
    }


    printf("evaluation: ");
    if (params.integer == 0)
        printf("%.*lf\n", params.ndigits, stack[top].value);
    else
        printf("%ld\n", (long)stack[top].value);


    return stack[top].value;
}



#endif