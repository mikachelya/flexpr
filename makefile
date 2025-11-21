flags = -Wall -g

flet: tokenizer.o flet.c
	gcc $(flags) -o flet flet.c tokenizer.o

tokenizer.o: tokenizer.c
	gcc $(flags) -c -o tokenizer.o tokenizer.c