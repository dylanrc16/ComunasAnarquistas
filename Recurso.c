#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Recurso.h"

struct Recurso *crearRecurso(const char *nombre, int existencia, int maximo) {
    if (nombre == NULL || existencia < 0 || maximo < 0) {
        return NULL;
    }

    struct Recurso *nuevo = calloc(1, sizeof(struct Recurso));

    if (nuevo == NULL) {
        return NULL;
    }

    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", nombre);
    nuevo->existencia = existencia;
    nuevo->maximo = maximo;

    return nuevo;
}

struct Recurso *agregarRecursoFinal(struct Recurso *inicio, const char *nombre, int existencia, int maximo) {
    struct Recurso *nuevo = crearRecurso(nombre, existencia, maximo);

    if (nuevo == NULL) {
        return inicio;
    }

    if (inicio == NULL) {
        return nuevo;
    }

    struct Recurso *actual = inicio;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nuevo;
    return inicio;
}

struct Recurso *buscarRecurso(struct Recurso *inicio, const char *nombre) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

void imprimirRecursos(struct Recurso *inicio) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        printf("%s: %d/%d\n", actual->nombre, actual->existencia, actual->maximo);
        actual = actual->siguiente;
    }
}

void liberarRecursos(struct Recurso *inicio) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        struct Recurso *siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
}

struct Recurso *cargarRecursos(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return NULL;
    }

    struct Recurso *inicio = NULL;
    char nombre[100];

    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        nombre[strcspn(nombre, "\r\n")] = '\0';

        if (nombre[0] != '\0') {
            inicio = agregarRecursoFinal(inicio, nombre, 0, 0);
        }
    }

    fclose(archivo);
    return inicio;
}