CC = gcc
FLAGS = -Wall
NAME = library1

.PHONY: main

all: static
	make main

static:
	$(CC) $(FLAGS) -c $(NAME).c -o $(NAME).o
	ar rcs $(NAME).a $(NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c library1.c -o $(NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(NAME).so $(NAME).o
	make clean

main:
	$(CC) $(FLAGS) main.c -o main -I./ $(NAME).a
	make clean

clean:
	rm -f *.o

tests: all
	./main create_table 15 measure_time wc_files "../test_files/small1.txt" end_measure "one small" \
    measure_time wc_files "../test_files/medium1.txt" end_measure "one medium" \
    measure_time wc_files "../test_files/large1.txt" end_measure "one large" \
    measure_time wc_files "../test_files/small1.txt ../test_files/small2.txt ../test_files/small3.txt ../test_files/small4.txt" end_measure "many small" \
    measure_time wc_files "../test_files/medium1.txt ../test_files/medium2.txt ../test_files/medium3.txt ../test_files/medium4.txt" end_measure "many medium" \
 	measure_time wc_files "../test_files/large1.txt ../test_files/large2.txt ../test_files/large3.txt ../test_files/large4.txt" end_measure "many large" \
 	measure_time wc_files "../test_files/large1.txt ../test_files/large2.txt ../test_files/large3.txt ../test_files/large4.txt" end_measure "a few mixed size" \
 	measure_time wc_files "../test_files/large1.txt ../test_files/large2.txt ../test_files/large3.txt ../test_files/large4.txt ../test_files/medium1.txt ../test_files/medium2.txt ../test_files/medium3.txt ../test_files/medium4.txt ../test_files/small1.txt ../test_files/small2.txt ../test_files/small3.txt ../test_files/small4.txt" end_measure "many mixed size" \
 	measure_time save_tmp end_measure "save large" \
 	measure_time remove_block 0 end_measure "delete large" \
 	wc_files "../test_files/small1.txt ../test_files/medium2.txt" \
 	measure_time save_tmp end_measure "save small" \
 	wc_files "../test_files/small4.txt ../test_files/large2.txt ../test_files/large3.txt ../test_files/medium4.txt ../test_files/small3.txt" \
 	measure_time save_tmp end_measure "save medium" \
 	measure_time remove_block 0 end_measure "delete small" \
 	measure_time remove_block 1 end_measure "delete medium" \
 	measure_time wc_files "../test_files/small1.txt ../test_files/small2.txt ../test_files/medium2.txt" \
 	save_tmp remove_block 0 wc_files "../test_files/medium1.txt ../test_files/small2.txt ../test_files/small3.txt ../test_files/large1.txt" \
 	wc_files "../test_files/small1.txt ../test_files/medium2.txt" \
 	wc_files "../test_files/large1.txt ../test_files/small2.txt ../test_files/medium1.txt" \
 	wc_files "../test_files/medium2.txt ../test_files/large2.txt ../test_files/small2.txt ../test_files/medium1.txt ../test_files/small2.txt" \
 	wc_files "../test_files/medium4.txt ../test_files/large1.txt ../test_files/large2.txt ../test_files/small2.txt" \
 	wc_files "../test_files/small1.txt ../test_files/medium2.txt" save_tmp \
 	remove_block 1 remove_block 0 wc_files "../test_files/small1.txt ../test_files/medium4.txt" \
 	wc_files "../test_files/small2.txt ../test_files/medium3.txt ../test_files/large1.txt ../test_files/small2.txt" \
 	wc_files "../test_files/small3.txt ../test_files/small4.txt ../test_files/medium1.txt ../test_files/medium4.txt" save_tmp \
 	remove_block 0 wc_files "../test_files/large1.txt ../test_files/large2.txt ../test_files/small2.txt ../test_files/medium2.txt" \
 	wc_files "../test_files/large2.txt ../test_files/small2.txt ../test_files/medium3.txt" \
 	wc_files "../test_files/small3.txt ../test_files/small4.txt ../test_files/small2.txt" \
 	wc_files "../test_files/small2.txt ../test_files/small1.txt ../test_files/medium2.txt ../test_files/medium3.txt" \
 	wc_files "../test_files/small3.txt ../test_files/small4.txt ../test_files/medium1.txt ../test_files/small1.txt ../test_files/medium3.txt" \
 	wc_files "../test_files/small2.txt ../test_files/small2.txt" \
 	wc_files "../test_files/small2.txt ../test_files/small2.txt" \
 	wc_files "../test_files/small2.txt ../test_files/small2.txt" \
 	save_tmp remove_block 0 \
 	end_measure "mixed operations"