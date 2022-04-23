CC = gcc
FLAGS = -Wall

.PHONY: normal nftw

normal:
	$(CC) $(FLAGS) find.c -o find

nftw:
	$(CC) $(FLAGS) find_nftw.c -o find_nftw

test: normal nftw
	./find ./test
	echo "*********\n"
	./find_nftw ./test
