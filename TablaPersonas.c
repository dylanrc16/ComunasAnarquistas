#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TablaPersonas.h"

// Funcion para dejar la tabla vacia
void iniciarTablaPersonas(struct TablaPersonas *tabla) {
    for (int i = 0; i < TAMANO_TABLA; i++) {
        tabla->casillas[i] = NULL;
    }
    tabla->cantidad = 0;
}

// Funcion hash djb2: empieza en 5381 y por cada letra multiplica por 33 y suma la letra.
// El resultado se ajusta al tamano de la tabla con el residuo (%).
int calcularCasilla(const char *nombre) {
    unsigned long hash = 5381;

    for (int i = 0; nombre[i] != '\0'; i++) {
        hash = hash * 33 + (unsigned char) nombre[i];
    }

    return (int) (hash % TAMANO_TABLA);
}

// Funcion para buscar una persona por nombre exacto: solo revisa la lista de su casilla
struct Persona *buscarPersonaTabla(const struct TablaPersonas *tabla, const char *nombre) {
    struct Persona *actual = tabla->casillas[calcularCasilla(nombre)];

    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

// Funcion para insertar una persona con nombre unico.
// Se inserta al inicio de la lista de su casilla: es O(1) y el orden no importa.
struct Persona *insertarPersonaTabla(struct TablaPersonas *tabla, const char *nombre) {
    if (nombre == NULL || nombre[0] == '\0') {
        return NULL;
    }

    // Si el nombre ya existe, prueba "Nombre 2", "Nombre 3"... hasta encontrar uno libre
    char nombreUnico[50];
    snprintf(nombreUnico, sizeof(nombreUnico), "%s", nombre);

    for (int numero = 2; buscarPersonaTabla(tabla, nombreUnico) != NULL; numero++) {
        snprintf(nombreUnico, sizeof(nombreUnico), "%.36s %d", nombre, numero);
    }

    struct Persona *nueva = crearPersona(nombreUnico);

    if (nueva == NULL) {
        return NULL;
    }

    int casilla = calcularCasilla(nueva->nombre);
    nueva->siguiente = tabla->casillas[casilla];
    tabla->casillas[casilla] = nueva;
    tabla->cantidad++;

    return nueva;
}

// Funcion para cargar las personas de un archivo, una por linea
int cargarTablaPersonas(struct TablaPersonas *tabla, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return 0;
    }

    char nombre[100];

    while (fgets(nombre, sizeof(nombre), archivo) != NULL) {
        // Quita el salto de linea del final
        nombre[strcspn(nombre, "\r\n")] = '\0';

        if (nombre[0] != '\0') {
            insertarPersonaTabla(tabla, nombre);
        }
    }

    fclose(archivo);
    return tabla->cantidad;
}

// Funcion para mostrar casillas usadas, colisiones y la lista mas larga
void imprimirEstadisticasTabla(const struct TablaPersonas *tabla) {
    int usadas = 0;
    int masLarga = 0;

    for (int i = 0; i < TAMANO_TABLA; i++) {
        int largo = 0;
        struct Persona *actual = tabla->casillas[i];

        while (actual != NULL) {
            largo++;
            actual = actual->siguiente;
        }

        if (largo > 0) {
            usadas++;
        }
        if (largo > masLarga) {
            masLarga = largo;
        }
    }

    printf("Tabla hash: %d personas en %d casillas\n", tabla->cantidad, TAMANO_TABLA);
    printf("Casillas usadas: %d  Personas que colisionaron: %d  Lista mas larga: %d\n",
           usadas, tabla->cantidad - usadas, masLarga);
}

// Funcion para liberar todas las personas: libera la lista de cada casilla
void liberarTablaPersonas(struct TablaPersonas *tabla) {
    for (int i = 0; i < TAMANO_TABLA; i++) {
        liberarPersonas(tabla->casillas[i]);
        tabla->casillas[i] = NULL;
    }
    tabla->cantidad = 0;
}
