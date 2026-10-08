#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Persona.h"

// Funcion para crear una persona nueva con su nombre
struct Persona *crearPersona(const char *nombre) {
    // Pide memoria en ceros para el nodo
    struct Persona *nueva = calloc(1, sizeof(struct Persona));

    // Si no hay memoria, no se crea
    if (nueva == NULL) {
        return NULL;
    }

    // Copia el nombre sin pasarse del tamano del arreglo
    snprintf(nueva->nombre, sizeof(nueva->nombre), "%s", nombre);
    return nueva;
}

// Funcion para agregar una persona al final de la lista
struct Persona *agregarPersonaFinal(struct Persona *inicio, const char *nombre) {
    struct Persona *nueva = crearPersona(nombre);

    // Si no se pudo crear, la lista queda igual
    if (nueva == NULL) {
        return inicio;
    }

    // Si la lista esta vacia, la nueva es el inicio
    if (inicio == NULL) {
        return nueva;
    }

    struct Persona *actual = inicio;

    // Avanza hasta el ultimo nodo
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    // Engancha la nueva persona al final
    actual->siguiente = nueva;
    return inicio;
}

// Funcion para cargar las personas desde el archivo "personas"
struct Persona *cargarPersonas(void) {
    // Abre el archivo en modo lectura
    FILE *archivo = fopen("personas", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo personas\n");
        return NULL;
    }

    struct Persona *inicio = NULL;
    char nombre[100];

    // Lee el archivo linea por linea
    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        // Quita el salto de linea del final
        nombre[strcspn(nombre, "\r\n")] = '\0';

        // Ignora las lineas vacias
        if (nombre[0] != '\0') {
            inicio = agregarPersonaFinal(inicio, nombre);
        }
    }

    // Cierra el archivo
    fclose(archivo);
    return inicio;
}

// Funcion para liberar la memoria de toda la lista de personas
void liberarPersonas(struct Persona *inicio) {
    struct Persona *actual = inicio;

    while (actual != NULL) {
        // Guarda el siguiente antes de liberar el actual
        struct Persona *siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
}