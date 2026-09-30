#ifndef RECURSO_H
#define RECURSO_H

struct Recurso {
    char nombre[50];
    int existencia;
    int maximo;
    struct Recurso *siguiente;
};

struct Recurso *crearRecurso(const char *nombre, int existencia, int maximo);
struct Recurso *agregarRecursoFinal(struct Recurso *inicio, const char *nombre, int existencia, int maximo);
struct Recurso *buscarRecurso(struct Recurso *inicio, const char *nombre);
void imprimirRecursos(struct Recurso *inicio);
void liberarRecursos(struct Recurso *inicio);
struct Recurso *cargarRecursos(const char *nombreArchivo);

#endif