makall: main.o aleatorio.o
	gcc -o main main.o aleatorio.o  -pthread -lm
main.o: main.c
	gcc -c -o main.o main.c
aleatorio.o:aleatorio.c
	gcc -c -o aleatorio.o aleatorio.c
clean:
	rm -f *.o main