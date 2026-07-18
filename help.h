#define HELP "Evaluate the floating point EXPRESSION.\n\
Examples: %s -D 2 1.1 + 1.2\n\
          %s -I \"sqrt(2) + prod(1,2,3,4)\"\n\
\n\
Options:\n\
  Precision:\n\
    These options determine which values are considered equal\n\
    -E, --epsilon=EPS    set epsilon value (default DBL_EPSILON, ~2.22e-16);\n\
                         mutually exclusive with -T\n\
    -T, --tolerance=TOL  set tolerance in terms of the number of double-\n\
                         precision values between the compared numbers;\n\
                         mutually exclusive with -E\n\
\n\
  Output:\n\
    -D, --digits=NUM     round the output to the specified number of digits\n\
                         after the decimal point (rounds to even)\n\
                         mutually exclusive with -I\n\
    -I, --integer        rounds the output to an integer;\n\
                         equivalent to -D 0;\n\
                         mutually exclusive with -D\n\
\n\
  Debug:\n\
    --tokenize           display the output of the tokenization stage\n\
    --postfix            display the EXPRESSION in postfix notation\n\
\n\
  Miscellaneous:\n\
    -H, --help           display this help text and exit\n\
    -V, --version        display version information and exit\n\
\n\
Operators (Standard C-precedence):\n\
  + - * / %% > < == != >= <= && || !\n\
\n\
Constants:\n\
  pi e\n\
\n\
Functions:\n\
  Standard:\n\
    abs()   sqrt()  exp()   floor() ceil()  round() pow(base, exponent)\n\
  Trigonometric (radians):\n\
    sin()   cos()   tan()   asin()  acos()  atan()\n\
  Logarithms:\n\
    ln()    natural logarithm (base e)\n\
    log()   common logarithm (base 10)\n\
  Aggregates (Accepts 1 or more arguments, e.g., fn(a, b, c)):\n\
    min(...)  max(...)  sum(...)  prod(...)\n\
  Logical Aggregates (Evaluates truthiness based on EPS/TOL):\n\
    any(...)  all(...)\n"
