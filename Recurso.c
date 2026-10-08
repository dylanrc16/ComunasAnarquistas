#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Recurso.h"

// Funcion para crear un recurso nuevo con su existencia y maximo
struct Recurso *crearRecurso(const char *nombre, int existencia, int maximo) {
    // Rechaza datos invalidos
    if (nombre == NULL || existencia < 0 || maximo < 0) {
        return NULL;
    }

    // Pide memoria en ceros para el nodo
    struct Recurso *nuevo = calloc(1, sizeof(struct Recurso));

    if (nuevo == NULL) {
        return NULL;
    }

    // Copia el nombre y guarda las cantidades
    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", nombre);
    nuevo->existencia = existencia;
    nuevo->maximo = maximo;

    return nuevo;
}

// Funcion para agregar un recurso al final de la lista
struct Recurso *agregarRecursoFinal(struct Recurso *inicio, const char *nombre, int existencia, int maximo) {
    struct Recurso *nuevo = crearRecurso(nombre, existencia, maximo);

    // Si no se pudo crear, la lista queda igual
    if (nuevo == NULL) {
        return inicio;
    }

    // Si la lista esta vacia, el nuevo es el inicio
    if (inicio == NULL) {
        return nuevo;
    }

    struct Recurso *actual = inicio;

    // Avanza hasta el ultimo nodo
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    // Engancha el nuevo recurso al final
    actual->siguiente = nuevo;
    return inicio;
}

// Funcion para buscar un recurso por nombre (NULL si no existe)
struct Recurso *buscarRecurso(struct Recurso *inicio, const char *nombre) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        // Compara nombres; si son iguales lo encontro
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

// Funcion para imprimir cada recurso como nombre: existencia/maximo
void imprimirRecursos(struct Recurso *inicio) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        printf("%s: %d/%d\n", actual->nombre, actual->existencia, actual->maximo);
        actual = actual->siguiente;
    }
}

// Funcion para liberar la memoria de toda la lista de recursos
void liberarRecursos(struct Recurso *inicio) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        // Guarda el siguiente antes de liberar el actual
        struct Recurso *siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
}

// Funcion para cargar los nombres de recursos desde un archivo (bienes o servicios)
struct Recurso *cargarRecursos(const char *nombreArchivo) {
    // Abre el archivo en modo lectura
    FILE *archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return NULL;
    }

    struct Recurso *inicio = NULL;
    char nombre[100];

    // Lee el archivo linea por linea
    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        // Quita el salto de linea del final
        nombre[strcspn(nombre, "\r\n")] = '\0';

        if (nombre[0] != '\0') {
            // En el catalogo solo importa el nombre; las cantidades se asignan despues
            inicio = agregarRecursoFinal(inicio, nombre, 0, 0);
        }
    }

    // Cierra el archivo
    fclose(archivo);
    return inicio;
}