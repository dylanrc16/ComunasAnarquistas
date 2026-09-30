#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Comunas.h"

struct Comuna *crearComuna(const char *nombre, int cantidadPersonas) {
    if (nombre == NULL || cantidadPersonas < 0) {
        return NULL;
    }

    struct Comuna *nueva = calloc(1, sizeof(struct Comuna));

    if (nueva == NULL) {
        return NULL;
    }

    snprintf(nueva->nombre, sizeof(nueva->nombre), "%s", nombre);
    nueva->cantidadPersonas = cantidadPersonas;

    return nueva;
}

struct Comuna *agregarComunaFinal(struct Comuna *inicio, const char *nombre, int cantidadPersonas) {
    struct Comuna *nueva = crearComuna(nombre, cantidadPersonas);

    if (nueva == NULL) {
        return inicio;
    }

    if (inicio == NULL) {
        return nueva;
    }

    struct Comuna *actual = inicio;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nueva;
    nueva->anterior = actual;

    return inicio;
}

struct Comuna *buscarComuna(struct Comuna *inicio, const char *nombre) {
    struct Comuna *actual = inicio;

    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

void imprimirComunas(struct Comuna *inicio) {
    struct Comuna *actual = inicio;

    while (actual != NULL) {
        printf("%s: %d personas\n", actual->nombre, actual->cantidadPersonas);
        actual = actual->siguiente;
    }
}

void liberarComunas(struct Comuna *inicio) {
    struct Comuna *actual = inicio;

    while (actual != NULL) {
        struct Comuna *siguiente = actual->siguiente;
        liberarRecursos(actual->bienes);
        liberarRecursos(actual->servicios);
        free(actual);
        actual = siguiente;
    }
}

struct Comuna *cargarComunas(void) {
    FILE *archivo = fopen("comunas", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo comunas\n");
        return NULL;
    }

    struct Comuna *inicio = NULL;
    char nombre[100];

    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        nombre[strcspn(nombre, "\r\n")] = '\0';

        if (nombre[0] != '\0') {
            inicio = agregarComunaFinal(inicio, nombre, 0);
        }
    }

    fclose(archivo);
    return inicio;
}