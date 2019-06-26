#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "aleatorio.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define X 100
#define Y 100

typedef enum
{
    pendiente,
    asignado,
    entregado
} estado;
typedef struct
{
    int x;
    int y;
} coordenada;
typedef struct
{
    int id;
    coordenada ubicacion;
} dron;
typedef struct
{
    char nombre[80];
    long telefono;
    char direccion[30];
    coordenada coordenada;
    estado stdo;
} paquete;
typedef sem_t semaphore;
semaphore mutex;
int numPack = 0;
int fin = 0;
int numPendientes=0;
paquete *bufPaquetes;
void down(sem_t *);
void up(sem_t *);
int masCercano(dron *);
void generarPaq();
coordenada generarCoordenada();
int existeCo(coordenada);
paquete *generar_Buffer(size_t);
void asignarDron(dron *);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    pthread_t dron1;
    pthread_t dron2;
    pthread_t dron3;
    numPack = atoi(argv[1]);

    if (X * Y < numPack)
    {
        fprintf(stderr, "El numero de paquetes es muy grande.");
        exit(EXIT_FAILURE);
    }
    sem_init(&mutex, 0, 1);

    bufPaquetes = generar_Buffer(numPack);
    generarPaq();
    numPendientes=numPack;
    dron d;
    dron d2;
    dron d3;
    d.id = 1;
    d.ubicacion.x = 0;
    d.ubicacion.y = 0;
    d2.id = 2;
    d2.ubicacion.x = 0;
    d2.ubicacion.y = 0;
    d3.id = 3;
    d3.ubicacion.x = 0;
    d3.ubicacion.y = 0;
     for (int i = 0; i < numPack; i++)
    {
        printf("%s coordenada: %d-%d\n",bufPaquetes[i].nombre,bufPaquetes[i].coordenada.x,bufPaquetes[i].coordenada.y);
         //printf("mas cercano: %s\n",masCercano(&d).nombre);
  
    }
   
    pthread_create(&dron1, NULL, (void *)asignarDron, (void *)&d);
    pthread_create(&dron2, NULL, (void *)asignarDron, (void *)&d2);
    pthread_create(&dron3, NULL, (void *)asignarDron, (void *)&d3);
    //sleep(5);

    pthread_join(dron1, NULL);
    pthread_join(dron2, NULL);
    pthread_join(dron3, NULL);
    printf("paso2\n");
    exit(EXIT_SUCCESS);
}

void asignarDron(dron *d)
{
    int idx;
    coordenada co;
    while (!fin)
    {
        down(&mutex);
        if (numPack != 0)
        {
            co.x = d->ubicacion.x;
            co.y = d->ubicacion.y;
            idx=masCercano(d);
            d->ubicacion = bufPaquetes[idx].coordenada;
            if (idx == -1)
            {
                fin = 1;
                up(&mutex);
                continue;
            }
            bufPaquetes[idx].stdo = asignado;
            numPendientes--;
            printf("Paquete de %s asignado al dron %d que esta en %d-%d \n", bufPaquetes[idx].nombre, d->id, co.x, co.y);
            up(&mutex);
            // sleep(2); //TODO
            bufPaquetes[idx].stdo = entregado;
            
           printf("Paquete de %s entregado por el dron %d que esta en %d-%d \n", bufPaquetes[idx].nombre, d->id, d->ubicacion.x, d->ubicacion.y);
        }
        else
        {
            fin = 1;
            up(&mutex);
        }
   }
}

int masCercano(dron *d)
{
    double distancia = 0.0;
    int idx_paq=-1;
    double menor = X * Y;
    double suma;
    for (int i = 0; i < numPack; i++)
    {
            if (strstr(bufPaquetes[i].nombre, "Cliente") != NULL)
            {
                if (bufPaquetes[i].stdo == pendiente)
                {
                    suma = pow(d->ubicacion.x - bufPaquetes[i].coordenada.x, 2) + pow(d->ubicacion.y - bufPaquetes[i].coordenada.y, 2);
                    distancia = sqrt(suma);
                    if (distancia < menor)
                    {
                        menor = distancia;
                        idx_paq=i;   
                    }
                }
        }
    }
    return idx_paq;
}
coordenada generarCoordenada()
{
    coordenada co;
    co.x = aleatorio(X);
    co.y = aleatorio(X);
    if (co.x == 0 && co.y == 0)
    {
        generarCoordenada();
    }
    /* if (existeCo(co)==1)
    {
        generarCoordenada;
    }*/
    return co;
}
int existeCo(coordenada co){
    for (int i = 0; i < numPack; i++)
    {
            if (co.x==bufPaquetes[i].coordenada.x&&co.y==bufPaquetes[i].coordenada.y)
            {
                return 1;
            }
    }
    return 0;
    
}
void generarPaq()
{
    if (bufPaquetes != NULL)
    {
        char cliente[30];
        char direccion[40];
        paquete paq;
        for (int i = 0; i < numPack; i++)
        {
            sprintf(cliente, "Cliente: %i", i+1);
            strcpy(paq.nombre, cliente);
            paq.coordenada.x = generarCoordenada().x;
            paq.coordenada.y = generarCoordenada().y;
            sprintf(direccion,"Calle %d con carrera %d No-%d",paq.coordenada.x,paq.coordenada.y,aleatorio(X));
            strcpy(paq.direccion,direccion);
            paq.telefono = rand();
            paq.stdo = pendiente;
            bufPaquetes[i] = paq;
        }
    }
}
/*Recerva bufPaquetes en memoria para el buffer de paquetes */
paquete *generar_Buffer(size_t n)
{
    paquete *aux;
    //Reservar memoria para n apuntadores (arreglos) a double
    aux = (paquete*)malloc(n * sizeof(paquete));
    if (aux == NULL)
    {
        free(aux);
        return NULL;
    }
    return aux;
}
void down(sem_t *s)
{
    sem_wait(s);
}
void up(sem_t *s)
{
    sem_post(s);
}