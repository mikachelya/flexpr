flags = -Wall -g

flet: flet.c flet.h object/tokenizer.o object/shuntingyard.o object/evaluator.o makefile
	gcc $(flags) -o flet flet.c object/*.o -lm

object/tokenizer.o: tokenizer.c makefile | object
	gcc $(flags) -c -o object/tokenizer.o tokenizer.c

object/shuntingyard.o: shuntingyard.c makefile | object
	gcc $(flags) -c -o object/shuntingyard.o shuntingyard.c

object/evaluator.o: evaluator.c makefile | object
	gcc $(flags) -c -o object/evaluator.o evaluator.c

object:
	mkdir -p object