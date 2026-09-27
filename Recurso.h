#ifndef RECURSO_H
#define RECURSO_H

struct Recurso {
    char nombre[50];
    int existencia;
    int maximo;
    struct Recurso *siguiente;
} Recurso;

struct Recurso *buscarRecurso(struct Recurso *inicio, char *nombre);

struct Recurso *crearRecurso(char *nombre, int existencia, int maximo);

struct Recurso *agregarFinal(struct Recurso *inicio, char *nombre, int existencia, int maximo);

void imprimirRecursos(struct Recurso *inicio);

#endif

