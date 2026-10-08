#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Comunas.h"

// Funcion para crear una comuna nueva con su nombre y habitantes
struct Comuna *crearComuna(const char *nombre, int cantidadPersonas) {
    // Rechaza datos invalidos
    if (nombre == NULL || cantidadPersonas < 0) {
        return NULL;
    }

    // Pide memoria en ceros: indices en 0 y punteros en NULL
    struct Comuna *nueva = calloc(1, sizeof(struct Comuna));

    if (nueva == NULL) {
        return NULL;
    }

    // Copia el nombre sin pasarse del tamano del arreglo
    snprintf(nueva->nombre, sizeof(nueva->nombre), "%s", nombre);
    nueva->cantidadPersonas = cantidadPersonas;

    return nueva;
}

// Funcion para agregar al final
struct Comuna *agregarComunaFinal(struct Comuna *inicio, const char *nombre, int cantidadPersonas) {
    struct Comuna *nueva = crearComuna(nombre, cantidadPersonas);

    // Si no se pudo crear, la lista queda igual
    if (nueva == NULL) {
        return inicio;
    }

    // Si la lista esta vacia, la nueva es el inicio
    if (inicio == NULL) {
        return nueva;
    }

    struct Comuna *actual = inicio;

    // Avanza hasta la ultima comuna
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    // Enlaza en los dos sentidos: ultima -> nueva y nueva -> ultima
    actual->siguiente = nueva;
    nueva->anterior = actual;

    return inicio;
}

// Funcion para buscar una comuna por nombre (NULL si no existe)
struct Comuna *buscarComuna(struct Comuna *inicio, const char *nombre) {
    struct Comuna *actual = inicio;

    while (actual != NULL) {
        // Compara nombres; si son iguales la encontro
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

// Funcion para imprimir cada comuna con sus habitantes
void imprimirComunas(struct Comuna *inicio) {
    struct Comuna *actual = inicio;

    while (actual != NULL) {
        printf("%s: %d personas\n", actual->nombre, actual->cantidadPersonas);
        actual = actual->siguiente;
    }
}

// Funcion para liberar todas las comunas junto con sus bienes y servicios
void liberarComunas(struct Comuna *inicio) {
    struct Comuna *actual = inicio;

    while (actual != NULL) {
        // Guarda la siguiente antes de liberar la actual
        struct Comuna *siguiente = actual->siguiente;
        // Primero libera las listas internas y luego el nodo
        liberarRecursos(actual->bienes);
        liberarRecursos(actual->servicios);
        free(actual);
        actual = siguiente;
    }
}

// Funcion para cargar los nombres de comunas desde el archivo "comunas"
struct Comuna *cargarComunas(void) {
    // Abre el archivo en modo lectura
    FILE *archivo = fopen("comunas", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo comunas\n");
        return NULL;
    }

    struct Comuna *inicio = NULL;
    char nombre[100];

    // Lee el archivo linea por linea
    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        // Quita el salto de linea del final
        nombre[strcspn(nombre, "\r\n")] = '\0';

        if (nombre[0] != '\0') {
            // Solo se guarda el nombre; los habitantes se asignan en crearSociedad
            inicio = agregarComunaFinal(inicio, nombre, 0);
        }
    }

    // Cierra el archivo
    fclose(archivo);
    return inicio;
}