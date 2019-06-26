#include "aleatorio.h"


int aleatorio(size_t rango){
    int result;
   
    result=(int)rand()%rango;
    
    return result;
}