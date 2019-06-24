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
    char nombre[30];
    long telefono;
    char direccion[30];
    coordenada coordenada;
    estado stdo;
} destinatario;
int numPack = 0;
destinatario **espacio;

coordenada masCercano(dron);

void generarPaq();
coordenada generarCoordenada();

destinatario **generar_matriz(size_t, size_t);
void free_matriz(destinatario **, size_t);

void asignarDron(dron *);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    numPack = atoi(argv[1]);

    if (X * Y < numPack)
    {
        fprintf(stderr, "El numero de paquetes es muy grande.");
        exit(EXIT_FAILURE);
    }
    espacio = generar_matriz(X, Y);
    generarPaq();
    dron d;
    dron d2;
    d.id=1;
    d.ubicacion.x=0;
    d.ubicacion.y=0;
    d2.id=2;
    d2.ubicacion.x=2;
    d2.ubicacion.y=2;
    asignarDron(&d);
   asignarDron(&d2);
    exit(EXIT_SUCCESS);
}
void asignarDron(dron *argdron)
{
    dron d;
    d.id = argdron->id;
    d.ubicacion = argdron->ubicacion;

    d.ubicacion = masCercano(d);
    printf("mas cer: %d-%d\n",d.ubicacion.x,d.ubicacion.y);
    espacio[d.ubicacion.x][d.ubicacion.y].stdo = asignado;
    printf("%d-",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.x);
    printf("%d\n",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.y);
    sleep(2); //TODO
    espacio[d.ubicacion.x][d.ubicacion.y].stdo = entregado;
    printf("%d-",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.x);
    printf("%d\n",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.y);
}

coordenada masCercano(dron d)
{
    double distancia = 0.0;
    coordenada co;
    double menor = X * Y;
    double suma;
    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Y; j++)
        {
            if (strstr(espacio[i][j].nombre,"Cliente")!=NULL)
            {
                if (espacio[i][j].stdo == 0)
                {
                    suma = pow(d.ubicacion.x - i, 2) + pow(d.ubicacion.y - j, 2);
                    distancia = sqrt(suma);
                    if (distancia <= menor)
                    {
                        menor = distancia;
                        co.x = i;
                        co.y = j;
                        // printf("%lf\n",distancia);
                        printf("dentrocercano: %d-%d\n",co.x,co.y);
                       
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
    if (co.x==0&&co.y==0)
    {
        return generarCoordenada();
    }
    
    return co;
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
            //sprintf(cliente,"Cliente: %i",i);
            // puts(cliente);
            strcpy(paq.nombre, "Cliente");
            paq.coordenada.x = generarCoordenada().x;
         paq.coordenada.y = generarCoordenada().y;
         
            paq.telefono = rand();
            paq.stdo = pendiente;
           // printf("coor: %d -%d\n ",espacio[paq.coordenada.x][paq.coordenada.y].coordenada.x,espacio[paq.coordenada.x][paq.coordenada.y].coordenada.y);
            //strcat(paq.nombre,itoa(i));
            espacio[paq.coordenada.x][paq.coordenada.y] = paq;
            printf("coor: %d -%d\n ",espacio[paq.coordenada.x][paq.coordenada.y].coordenada.x,espacio[paq.coordenada.x][paq.coordenada.y].coordenada.y);
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