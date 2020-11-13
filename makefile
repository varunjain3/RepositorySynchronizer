folder1 = test1
folder2 = test2
sleeptimer = 1

hello: filesync_local.cpp toolkit.cpp md5.cpp
		@echo "Building filesync..."
		g++ filesync_local.cpp toolkit.cpp md5.cpp -o filesync 
		./filesync $(sleeptimer) $(folder1) $(folder2)
clean: filesync
	rm filesync