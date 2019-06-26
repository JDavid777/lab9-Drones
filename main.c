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
} destinatario;
typedef sem_t semaphore;
semaphore mutex;
semaphore entregados;
int numPack = 0;
int fin = 0;
int idx = 1;

destinatario **espacio;
void down(sem_t *);
void up(sem_t *);
coordenada masCercano(dron *);

void generarPaq();
coordenada generarCoordenada();
int existeCo(coordenada);
destinatario **generar_matriz(size_t, size_t);
void free_matriz(destinatario **, size_t);

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
    sem_init(&entregados, 0, numPack);

    espacio = generar_matriz(X, Y);
    generarPaq();
    printf("paso\n");
    dron d;
    dron d2;
    dron d3;
    d.id = 1;
    d.ubicacion.x = 0;
    d.ubicacion.y = 0;
    d2.id = 2;
    d2.ubicacion.x = 56;
    d2.ubicacion.y = 43;
    d3.id = 3;
    d3.ubicacion.x = 12;
    d3.ubicacion.y = 31;
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
    coordenada co;
    while (!fin)
    {
        down(&mutex);
        if (numPack != 0)
        {
            co.x = d->ubicacion.x;
            co.y = d->ubicacion.y;
            d->ubicacion = masCercano(d);
            if (d->ubicacion.x == -1)
            {
                fin = 1;
                up(&mutex);
                continue;
            }
            espacio[d->ubicacion.x][d->ubicacion.y].stdo = asignado;
            numPack--;
            printf("Paquete de %s asignado al dron %d que esta en %d-%d \n", espacio[d->ubicacion.x][d->ubicacion.y].nombre, d->id, co.x, co.y);
            up(&mutex);
            // sleep(2); //TODO

            espacio[d->ubicacion.x][d->ubicacion.y].stdo = entregado;
            printf("Paquete de %s entregado por el dron %d que esta en %d-%d \n", espacio[d->ubicacion.x][d->ubicacion.y].nombre, d->id, d->ubicacion.x, d->ubicacion.y);
        }
        else
        {
            fin = 1;
            up(&mutex);
        }
    }
}

coordenada masCercano(dron *d)
{
    double distancia = 0.0;
    coordenada co;
    co.x = -1;
    co.y = -1;
    double menor = X * Y;
    double suma;
    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Y; j++)
        {
            if (strstr(espacio[i][j].nombre, "Cliente") != NULL)
            {
                if (espacio[i][j].stdo == pendiente)
                {
                    suma = pow(d->ubicacion.x - i, 2) + pow(d->ubicacion.y - j, 2);
                    distancia = sqrt(suma);
                    if (distancia <= menor)
                    {

                        menor = distancia;
                        co.x = i;
                        co.y = j;
                    }
                }
            }
        }
    }
    return co;
}
coordenada generarCoordenada()
{
    coordenada co;
    co.x = aleatorio();
    co.y = aleatorio();
    if (co.x == 0 && co.y == 0)
    {
        co=generarCoordenada();
        if (existeCo(co)==1)
        {
            generarCoordenada;
        }
    }
    return co;
}
int existeCo(coordenada co){
    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Y; j++)
        {
            if (co.x==espacio[i][j].coordenada.x&&co.y==espacio[i][j].coordenada.y)
            {
                return 1;
            }
            
        }
        
    }
    return 0;
    
}
void generarPaq()
{
    if (espacio != NULL)
    {
        //TODO
        char cliente[30];
        destinatario paq;
        for (int i = 0; i < numPack; i++)
        {
            sprintf(cliente, "Cliente: %i", i);
            strcpy(paq.nombre, cliente);
            paq.coordenada.x = generarCoordenada().x;
            paq.coordenada.y = generarCoordenada().y;

            paq.telefono = rand();
            paq.stdo = pendiente;
            //strcat(paq.nombre,itoa(i));
            espacio[paq.coordenada.x][paq.coordenada.y] = paq;
        }
    }
}
destinatario **generar_matriz(size_t n, size_t m)
{

    destinatario **aux;

    //Reservar memoria para n apuntadores (arreglos) a double
    aux = (destinatario **)malloc(n * sizeof(destinatario *));
    if (aux == NULL)
    {
        free(aux);
        return NULL;
    }
    for (int i = 0; i < n; i++)
    {
        aux[i] = malloc(m * sizeof(destinatario));
        if (aux[i] == NULL)
        {
            free_matriz(aux, i);
            return NULL;
        }
    }
    return aux;
}
/**
* Libera una matriz de n x m aleatorios 
* @param int n filas
* @param int m columnas
*/
void free_matriz(destinatario **m, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        free(m[i]);
    }
    free(m);
}
void down(sem_t *s)
{
    sem_wait(s);
}
void up(sem_t *s)
{
    sem_post(s);
}