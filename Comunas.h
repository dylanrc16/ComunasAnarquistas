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

struct Comuna *crearComuna(char *nombre);

struct Comuna *cargarComunas(void);



#endif