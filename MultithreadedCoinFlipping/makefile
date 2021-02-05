all: coins 

coins: coins.o
	gcc -pthread -o coins coins.c
	
deleteofiles:
	rm -f *.o
	
deleteexec:
	rm -f coins	

deleteall: deleteexec deleteofiles

createdir:
	mkdir test_directory

deletedir:
	rmdir test_directory