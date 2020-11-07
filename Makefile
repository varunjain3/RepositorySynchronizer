all: binaries

binaries: file_transfer.cpp
	g++ -o file_transfer file_transfer.cpp -l pthread

p1: 
	./file_transfer 12345 10000 10002 50.txt r1.txt

p2: 
	./file_transfer 10000 12345 10002 51.txt r2.txt

p3: 
	./file_transfer 10002 10000 12345 53.txt r3.txt

clean:
	rm file_transfer
