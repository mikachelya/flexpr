flags = -Wall -g

.PHONY: 
target: tokenizer.o
	gcc $(flags) -o flet flet.c tokenizer.o


tokenizer.o: tokenizer.c
	gcc $(flags) -c -o tokenizer.o tokenizer.c