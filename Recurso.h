#ifndef RECURSO_H
#define RECURSO_H

struct Recurso {
    char nombre[50];
    int existencia;
    int maximo;
    struct Recurso *siguiente;
} Recurso;

#endif

