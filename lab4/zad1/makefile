CC = gcc
FLAGS = -Wall

.PHONY: main

main:
	$(CC) $(FLAGS) main.c -o main

handler: main
	./main handler

mask: main
	./main mask

ignore: main
	./main ignore

pending: main
	./main pending