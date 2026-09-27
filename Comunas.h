#ifndef COMUNA_H
#define COMUNA_H

#include "Recurso.h"

struct Comuna {
    char nombre[50];

    Recurso *bienes;
    Recurso *servicios;

    struct Comuna *anterior;
    struct Comuna *siguiente;
} Comuna;

Comuna *crearComuna(char *nombre);



#endif