folder1 = test1/
folder2 = test2/
folder3 = test3/
folder4 = test4/

all: binaries


binaries: final.cpp
	g++ -g -o final final.cpp md5.cpp -lpthread
	rm -r test1/* test2/* test3/* test4/*
	touch test1/file1.txt
	touch test2/file2.txt
	touch test3/file3.txt
	touch test4/file4.txt
	echo "I am text1" > test1/file1.txt
	echo "I am text2" > test2/file2.txt
	echo "I am text3" > test3/file3.txt
	echo "I am text4" > test4/file4.txt
	
p1:
	./final test1/ 10000 10002 10004 10006 0 

p2: 
	./final test2/ 10002 10000 10004 10006 1 

p3:
	./final test3/ 10004 10000 10002 10006 2 

p4:
	./final test4/ 10006 10000 10002 10004 3


clean:
	rm final

#------------------------------------------------
#p1:
	#./final test1/ 10000 10002 10004 0 

#p2: 
	#./final test2/ 10002 10000 10004 1 

#p3:
	#./final test3/ 10004 10000 10002 2 