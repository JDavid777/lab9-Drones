makall: main.o
	gcc -o main main.o -pthread
main.o: main.c
	gcc -c -o main.o main.c
clean:
	rm -f *.o main