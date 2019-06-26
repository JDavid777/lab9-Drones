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
//Variables y arrays-----------
int numPack = 0;
int numDrones = 0;
int fin = 0;
int numPendientes = 0;
paquete *bufPaquetes;
dron *drones;

//--------------------

/**
* Recerva memoria a un array de paquetes.
*@param n numero de paquetes que podra almacenar el array
 */
paquete *generar_Buffer(size_t n);
/**
 * Gerena una coordenada aleatoria dentro del rando definido (X,Y)
 */
coordenada generarCoordenada();
/**
 * Verifica que la coordenada generada no exista.
 * */
int existeCo(coordenada);
void generarPaq();
/**
 * Recerva memoria a una lista de drones de tamaño numDrones.
 */
dron *inicializarDrones();
/**
 * Libera n drones para ser asignados a m paquetes.
 */
void liberarDrones();
void asignarDron(dron *);
int masCercano(dron *);
void down(sem_t *);
void up(sem_t *);
double calcularDistancia(coordenada,coordenada);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    if (argc!=3)
    {
        fprintf(stderr,"Debe especificar el numero de paquetes y drones para la simulación.\n");
         exit(EXIT_FAILURE);
    }
    numPack = atoi(argv[1]);
    numDrones = atoi(argv[2]);
    numPendientes = numPack;
    if (X * Y < numPack)
    {
        fprintf(stderr, "El numero de paquetes es muy grande.");
        exit(EXIT_FAILURE);
    }
    sem_init(&mutex, 0, 1);
    bufPaquetes = generar_Buffer(numPack);
    generarPaq();
    drones = inicializarDrones();
    liberarDrones();

    free(drones);
    free(bufPaquetes);
    exit(EXIT_SUCCESS);
}
dron *inicializarDrones()
{
    dron *listaDrones;
    listaDrones = (dron *)malloc(numDrones * sizeof(dron));
    if (listaDrones == NULL)
    {
        free(listaDrones);
        return NULL;
    }
    return listaDrones;
}
void liberarDrones()
{
    dron d;
    pthread_t *hilos = (pthread_t *)malloc(numDrones * sizeof(pthread_t));
    if(hilos==NULL){
        fprintf(stderr,"Error al recervar memoria a los hilos.");
        free(hilos);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < numDrones; i++)
    {
        d.id = i + 1;
        d.ubicacion = generarCoordenada();
        drones[i] = d;
        pthread_create(&hilos[i], NULL, (void *)asignarDron, (void *)&drones[i]); // crear hilo
    }
    sleep(1);
    fin = 1;
    for (int i = 0; i < numDrones; i++)
    {
        pthread_join(hilos[i], NULL); // esperar a que 'terminen' los n hilos
    }
}
void asignarDron(dron *d) //Metodo que sera ejecutado por los drones.
{
    int idx;
    coordenada co;
    double distancia;
    while (!fin)
    {
        down(&mutex);
        if (numPack != 0)
        {
            co.x = d->ubicacion.x;
            co.y = d->ubicacion.y;
            idx = masCercano(d);
            d->ubicacion = bufPaquetes[idx].coordenada;
            if (idx == -1)
            {
                fin = 1;
                up(&mutex);
                continue;
            }
            bufPaquetes[idx].stdo = asignado;
            numPendientes--;
            printf("Paquete de %s asignado al dron %d que esta en %d-%d a %.2f unidades de distancia. \n", bufPaquetes[idx].nombre, d->id, co.x, co.y,distancia);
            up(&mutex);
            distancia=calcularDistancia(co,bufPaquetes[idx].coordenada);
            //sleep(distancia); //TODO
            bufPaquetes[idx].stdo = entregado;

            printf("Paquete de %s entregado por el dron %d que esta en %d-%d \n\n", bufPaquetes[idx].nombre, d->id, d->ubicacion.x, d->ubicacion.y);
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
    int idx_paq = -1;
    double menor = X * Y;
    for (int i = 0; i < numPack; i++)
    {
        if (strstr(bufPaquetes[i].nombre, "Cliente") != NULL)
        {
            if (bufPaquetes[i].stdo == pendiente)
            {
                distancia=calcularDistancia(d->ubicacion,bufPaquetes[i].coordenada);
                if (distancia < menor)
                {
                    menor = distancia;
                    idx_paq = i;
                }
            }
        }
    }
    return idx_paq;
}
coordenada generarCoordenada()
{
    coordenada co;
    co.x = raleatorio(X);
    co.y = raleatorio(Y);
    if (co.x == 0 && co.y == 0)
    {
        generarCoordenada();
    }
     if (existeCo(co)==1)
    {
        generarCoordenada;
    }
    return co;
}
int existeCo(coordenada co)
{
    for (int i = 0; i < numPack; i++)
    {
        if (co.x == bufPaquetes[i].coordenada.x && co.y == bufPaquetes[i].coordenada.y)
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
            sprintf(cliente, "Cliente: %i", i + 1);
            strcpy(paq.nombre, cliente);
            paq.coordenada.x = generarCoordenada().x;
            paq.coordenada.y = generarCoordenada().y;
            sprintf(direccion, "Calle %d con carrera %d No-%d", paq.coordenada.x, paq.coordenada.y, aleatorio());
            strcpy(paq.direccion, direccion);
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
    aux = (paquete *)malloc(n * sizeof(paquete));
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
double calcularDistancia(coordenada co1, coordenada co2){
    double suma;
    suma = pow(co1.x - co2.x, 2) + pow(co1.y - co2.y, 2);
    return sqrt(suma);
}