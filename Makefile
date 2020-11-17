folder1 = test1/
folder2 = test2/
folder3 = test3/
folder4 = test4/

all: binaries


binaries: final.cpp
	g++ -o final final.cpp md5.cpp toolkit.cpp -lpthread
	rm -r test1/ test2/ test3/
	mkdir test1/ test2/ test3/
	touch test1/file1.txt
	touch test2/file2.txt
	touch test3/file3.txt
	echo "I am text1" > test1/file1.txt
	echo "I am text2" > test2/file2.txt
	echo "I am text3" > test3/file3.txt

	# ./final 10001 10000 $(folder1) &
	# ./final 10000 10001 $(folder2) &
	# ./final 10002 10003 $(folder2) &
	# ./final 10003 10002 $(folder3) &
	# ./final 10004 10005 $(folder3) &
	# ./final 10005 10004 $(folder4) &
	# ./final 10006 10007 $(folder4) &
	# ./final 10007 10006 $(folder1) &
	# ./final 10008 10009 $(folder4) &
	# ./final 10009 10008 $(folder2) &
	# ./final 10010 10011 $(folder3) &
	# ./final 10011 10010 $(folder1) &
