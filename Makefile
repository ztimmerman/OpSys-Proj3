stack.o: stack.c
	gcc -c stack.c

proj3:	fat32.c stack.o
	gcc -o proj3 fat32.c stack.o
clean:
	rm proj3 stack.o
