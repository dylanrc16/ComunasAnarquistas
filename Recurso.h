// Evita que este archivo se incluya dos veces
#ifndef RECURSO_H
#define RECURSO_H

// Nodo de la lista simple de recursos (bienes o servicios)
struct Recurso {
    char nombre[50];  // Nombre del recurso
    int existencia;  // Cantidad que hay ahora
    int maximo;  // Cantidad maxima que se puede tener
    struct Recurso *siguiente;  // Apunta al siguiente recurso
};

// Funcion para crear un recurso nuevo
struct Recurso *crearRecurso(const char *nombre, int existencia, int maximo);
// Funcion para agregar un recurso al final
struct Recurso *agregarRecursoFinal(struct Recurso *inicio, const char *nombre, int existencia, int maximo);
// Funcion para buscar un recurso por nombre
struct Recurso *buscarRecurso(struct Recurso *inicio, const char *nombre);
// Funcion para imprimir todos los recursos
void imprimirRecursos(struct Recurso *inicio);
// Funcion para liberar la lista de recursos
void liberarRecursos(struct Recurso *inicio);
// Funcion para cargar recursos desde un archivo
struct Recurso *cargarRecursos(const char *nombreArchivo);

// Fin de la proteccion contra inclusion doble
#endif