CC = gcc
FLAGS = -Wall
NAME = library1

.PHONY: static shared

all:
	make static
	make shared

static:
	$(CC) $(FLAGS) -c $(NAME).c -o $(NAME).o
	ar rcs $(NAME).a $(NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c library1.c -o $(NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(NAME).so $(NAME).o
	make clean

clean:
	rm -f *.o