flags = -Wall -g

.PHONY: 
target:
	gcc $(flags) -o flet flet.c