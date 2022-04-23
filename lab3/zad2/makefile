CC = gcc
FLAGS = -Wall

.PHONY: main

main:
	$(CC) $(FLAGS) main.c -o main

tests: main
	rm -f report2.txt
	./main 0.00000001 5
	./main 0.00000001 10
	./main 0.00000001 20
	./main 0.000000001 5
	./main 0.000000001 10
	./main 0.000000001 60