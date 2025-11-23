flags = -Wall -g

flet: tokenizer.o flet.c flet.h shuntingyard.o
	gcc $(flags) -o flet flet.c tokenizer.o shuntingyard.o

tokenizer.o: tokenizer.c
	gcc $(flags) -c -o tokenizer.o tokenizer.c

shuntingyard.o: shuntingyard.c
	gcc $(flags) -c -o shuntingyard.o shuntingyard.c