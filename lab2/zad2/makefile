CC = gcc
FLAGS = -Wall

.PHONY: system library

system:
	$(CC) $(FLAGS) main_sys.c -o main_sys

library:
	$(CC) $(FLAGS) main_lib.c -o main_lib

all: system library
	rm pomiar_zad_2.txt
	./main_sys a test
	./main_lib a test