all: binaries

binaries: file_transfer.cpp
	g++ -o file_transfer file_transfer.cpp -l pthread

p1: 
	./file_transfer 12345 10000 10002 50.txt received1/

p2: 
	./file_transfer 10000 12345 10002 129.txt received2/

p3: 
	./file_transfer 10002 10000 12345 3651.txt received3/

clean:
	rm file_transfer

cleanfolders:
	rm received1/books/* received2/books/* received3/books/*
