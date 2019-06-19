#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int x;
    int y;
}coordenada;
typedef struct {
    char* nombre;
    long telefono;
    char* direccion;
    coordenada coordenada;
   
}destinatario;
typedef sem_t semaphore;
semaphore mutex;
semaphore pendientes;
int fin=0;
void down(semaphore *s){
    sem_wait(s);
}
void up(semaphore *s){
    sem_post(s);
}
void generarPaq();
coordenada generarCoordenada();
void dron(coordenada);
destinatario masCercano(coordenada);


int main (int argc, char* argv[]){

    int N=0;//TODO

    sem_init(&mutex,0,1);
    sem_init(&pendientes,0,N);

    fin=1;//TODO
    exit(EXIT_SUCCESS);
}
void dron(coordenada coor){
    while(!fin){
        down(&mutex);
        
    }

}

