#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Persona.h"

struct Persona *crearPersona(const char *nombre) {
    struct Persona *nueva = calloc(1, sizeof(struct Persona));

    if (nueva == NULL) {
        return NULL;
    }

    snprintf(nueva->nombre, sizeof(nueva->nombre), "%s", nombre);
    return nueva;
}

struct Persona *agregarPersonaFinal(struct Persona *inicio, const char *nombre) {
    struct Persona *nueva = crearPersona(nombre);

    if (nueva == NULL) {
        return inicio;
    }

    if (inicio == NULL) {
        return nueva;
    }

    struct Persona *actual = inicio;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nueva;
    return inicio;
}

struct Persona *cargarPersonas(void) {
    FILE *archivo = fopen("personas", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo personas\n");
        return NULL;
    }

    struct Persona *inicio = NULL;
    char nombre[100];

    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        nombre[strcspn(nombre, "\r\n")] = '\0';

        if (nombre[0] != '\0') {
            inicio = agregarPersonaFinal(inicio, nombre);
        }
    }

    fclose(archivo);
    return inicio;
}