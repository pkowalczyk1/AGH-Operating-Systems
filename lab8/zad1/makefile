CC = gcc
FLAGS = -Wall

.PHONY: main

main:
	$(CC) $(FLAGS) main.c -o main -lpthread -lm

tests: main
	rm -f Times.txt
	echo "Image 600x600\nNumbers method" >> "Times.txt"
	echo "1 thread:" >> "Times.txt"
	./main 1 numbers ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "2 threads:" >> "Times.txt"
	./main 2 numbers ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "4 threads:" >> "Times.txt"
	./main 4 numbers ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "8 threads:" >> "Times.txt"
	./main 8 numbers ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "16 threads:" >> "Times.txt"
	./main 16 numbers ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "Image 600x600\nBlock method" >> "Times.txt"
	echo "1 thread:" >> "Times.txt"
	./main 1 block ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "2 threads:" >> "Times.txt"
	./main 2 block ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "4 threads:" >> "Times.txt"
	./main 4 block ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "8 threads:" >> "Times.txt"
	./main 8 block ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "16 threads:" >> "Times.txt"
	./main 16 block ../images/apollonian_gasket.ascii.pgm apollonian_gasket_out.ascii.pgm
	echo "------------------------------------" >> "Times.txt"
	echo "Image 640x480\nNumbers method" >> "Times.txt"
	echo "1 threads:" >> "Times.txt"
	./main 1 numbers ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "2 threads:" >> "Times.txt"
	./main 2 numbers ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "4 threads:" >> "Times.txt"
	./main 4 numbers ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "8 threads:" >> "Times.txt"
	./main 8 numbers ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "16 threads:" >> "Times.txt"
	./main 16 numbers ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "Image 640x480\nBlock method" >> "Times.txt"
	echo "1 thread:" >> "Times.txt"
	./main 1 block ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "2 threads:" >> "Times.txt"
	./main 2 block ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "4 threads:" >> "Times.txt"
	./main 4 block ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "8 threads:" >> "Times.txt"
	./main 8 block ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "16 threads:" >> "Times.txt"
	./main 16 block ../images/columns.ascii.pgm columns_out.ascii.pgm
	echo "------------------------------------" >> "Times.txt"
	echo "Image 320x428\nNumbers method" >> "Times.txt"
	echo "1 thread:" >> "Times.txt"
	./main 1 numbers ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "2 threads:" >> "Times.txt"
	./main 2 numbers ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "4 threads:" >> "Times.txt"
	./main 4 numbers ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "8 threads:" >> "Times.txt"
	./main 8 numbers ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "16 threads:" >> "Times.txt"
	./main 16 numbers ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "Image 320x428\nBlock method" >> "Times.txt"
	echo "1 threads:" >> "Times.txt"
	./main 1 block ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "2 threads:" >> "Times.txt"
	./main 2 block ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "4 threads:" >> "Times.txt"
	./main 4 block ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "8 threads:" >> "Times.txt"
	./main 8 block ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm
	echo "16 threads:" >> "Times.txt"
	./main 16 block ../images/glassware_noisy.ascii.pgm glassware_out.ascii.pgm