CC = gcc
FLAGS = -Wall

.PHONY: consumer producer first_test

consumer:
	$(CC) $(FLAGS) consumer.c -o consumer

producer:
	$(CC) $(FLAGS) producer.c -o producer

first_test:
	$(CC) $(FLAGS) run_test.c -o run_test

run_first_test: producer consumer first_test
	rm -f test_fifo
	rm -f output_test.txt
	./run_test

1_producer_4_consumers: producer consumer
	rm -f test_fifo
	mkfifo test_fifo
	rm -f output.txt
	echo "Test for 1 producer, 4 consumers, 100 character read"
	@./producer test_fifo 5 test_files/test1.txt 100 &
	@./consumer test_fifo output.txt 100 &
	@./consumer test_fifo output.txt 100 &
	@./consumer test_fifo output.txt 100 &
	@./consumer test_fifo output.txt 100
	@./compare_results.py test_files/test1.txt output.txt
	rm -f output.txt
	echo "Test for 1 producer, 4 consumers, 1000 character read"
	@./producer test_fifo 5 test_files/test1.txt 1000 &
	@./consumer test_fifo output.txt 1000 &
	@./consumer test_fifo output.txt 1000 &
	@./consumer test_fifo output.txt 1000 &
	@./consumer test_fifo output.txt 1000
	@./compare_results.py test_files/test1.txt output.txt
	rm -f output.txt
	echo "Test for 1 producer, 4 consumers, 5000 character read"
	@./producer test_fifo 5 test_files/test1.txt 5000 &
	@./consumer test_fifo output.txt 5000 &
	@./consumer test_fifo output.txt 5000 &
	@./consumer test_fifo output.txt 5000 &
	@./consumer test_fifo output.txt 5000
	@./compare_results.py test_files/test1.txt output.txt

4_producers_1_consumer: producer consumer
	rm -f test_fifo
	mkfifo test_fifo
	rm -f output.txt
	echo "Test for 4 producers, 1 consumer, 100 character read"
	@./producer test_fifo 5 test_files/test1.txt 100 &
	@./producer test_fifo 10 test_files/test2.txt 100 &
	@./producer test_fifo 6 test_files/test3.txt 100 &
	@./producer test_fifo 2 test_files/test4.txt 100 &
	@./consumer test_fifo output.txt 100
	@./compare_results.py test_files/test1.txt output.txt
	@./compare_results.py test_files/test2.txt output.txt
	@./compare_results.py test_files/test3.txt output.txt
	@./compare_results.py test_files/test4.txt output.txt
	rm -f output.txt
	echo "Test for 4 producers, 1 consumer, 1000 character read"
	@./producer test_fifo 5 test_files/test1.txt 1000 &
	@./producer test_fifo 10 test_files/test2.txt 1000 &
	@./producer test_fifo 6 test_files/test3.txt 1000 &
	@./producer test_fifo 2 test_files/test4.txt 1000 &
	@./consumer test_fifo output.txt 1000
	@./compare_results.py test_files/test1.txt output.txt
	@./compare_results.py test_files/test2.txt output.txt
	@./compare_results.py test_files/test3.txt output.txt
	@./compare_results.py test_files/test4.txt output.txt
	rm -f output.txt
	echo "Test for 4 producers, 1 consumer, 5000 character read"
	@./producer test_fifo 5 test_files/test1.txt 5000 &
	@./producer test_fifo 10 test_files/test2.txt 5000 &
	@./producer test_fifo 6 test_files/test3.txt 5000 &
	@./producer test_fifo 2 test_files/test4.txt 5000 &
	@./consumer test_fifo output.txt 5000
	@./compare_results.py test_files/test1.txt output.txt
	@./compare_results.py test_files/test2.txt output.txt
	@./compare_results.py test_files/test3.txt output.txt
	@./compare_results.py test_files/test4.txt output.txt

4_producers_4_consumers: producer consumer
	rm -f test_fifo
	mkfifo test_fifo
	rm -f output.txt
	echo "Test for 4 producers, 4 consumers, 100 character read"
	@./consumer test_fifo output.txt 100 &
	@./consumer test_fifo output.txt 100 &
	@./consumer test_fifo output.txt 100 &
	@./consumer test_fifo output.txt 100 &
	@./producer test_fifo 5 test_files/test1.txt 100 &
	@./producer test_fifo 6 test_files/test2.txt 100 &
	@./producer test_fifo 10 test_files/test3.txt 100 &
	@./producer test_fifo 2 test_files/test4.txt 100
	@./compare_results.py test_files/test1.txt output.txt
	@./compare_results.py test_files/test2.txt output.txt
	@./compare_results.py test_files/test3.txt output.txt
	@./compare_results.py test_files/test4.txt output.txt
	rm -f output.txt
	echo "Test for 4 producers, 4 consumers, 1000 character read"
	@./producer test_fifo 5 test_files/test1.txt 1000 &
	@./producer test_fifo 6 test_files/test2.txt 1000 &
	@./producer test_fifo 10 test_files/test3.txt 1000 &
	@./producer test_fifo 2 test_files/test4.txt 1000 &
	@./consumer test_fifo output.txt 1000
	@./consumer test_fifo output.txt 1000
	@./consumer test_fifo output.txt 1000
	@./consumer test_fifo output.txt 1000
	@./compare_results.py test_files/test1.txt output.txt
	@./compare_results.py test_files/test2.txt output.txt
	@./compare_results.py test_files/test3.txt output.txt
	@./compare_results.py test_files/test4.txt output.txt
	rm -f output.txt
	echo "Test for 4 producers, 4 consumers, 5000 character read"
	@./producer test_fifo 5 test_files/test1.txt 5000 &
	@./producer test_fifo 6 test_files/test2.txt 5000 &
	@./producer test_fifo 10 test_files/test3.txt 5000 &
	@./producer test_fifo 2 test_files/test4.txt 5000 &
	@./consumer test_fifo output.txt 5000
	@./consumer test_fifo output.txt 5000
	@./consumer test_fifo output.txt 5000
	@./consumer test_fifo output.txt 5000
	@./compare_results.py test_files/test1.txt output.txt
	@./compare_results.py test_files/test2.txt output.txt
	@./compare_results.py test_files/test3.txt output.txt
	@./compare_results.py test_files/test4.txt output.txt

test: 1_producer_4_consumers 4_producers_1_consumer 4_producers_4_consumers
