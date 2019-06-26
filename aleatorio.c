#include "aleatorio.h"


int raleatorio(size_t rango){
    int result;
   
    result=(int)rand()%rango;
    
    return result;
}
int aleatorio(){
    int result;
   
    result=(int)rand()%RANGO;
    
    return result;
}