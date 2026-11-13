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
        // Primero los servicios y los libros leidos, y despues el nodo
        liberarServicios(actual->servicios);
        liberarLibrosEncontrados(actual->librosLeidos);
        free(actual);
        actual = siguiente;
    }
}

// ---------- Servicios de una persona ----------

// Funcion para saber si una persona sabe un servicio
int sabeServicio(const struct Persona *persona, const char *servicio) {
    struct Servicio *actual = persona->servicios;

    while (actual != NULL) {
        if (strcmp(actual->nombre, servicio) == 0) {
            return 1;
        }
        actual = actual->siguiente;
    }

    return 0;
}

// Funcion para que una persona aprenda un servicio: lo agrega al final
// de su lista, sin repetir los que ya sabe
int aprenderServicio(struct Persona *persona, const char *servicio) {
    if (persona == NULL || servicio == NULL || sabeServicio(persona, servicio)) {
        return 0;
    }

    struct Servicio *nuevo = calloc(1, sizeof(struct Servicio));

    if (nuevo == NULL) {
        return 0;
    }

    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", servicio);

    if (persona->servicios == NULL) {
        persona->servicios = nuevo;
        return 1;
    }

    struct Servicio *actual = persona->servicios;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nuevo;
    return 1;
}

// Funcion para contar cuantos servicios sabe una persona
int contarServicios(const struct Persona *persona) {
    int cantidad = 0;
    struct Servicio *actual = persona->servicios;

    while (actual != NULL) {
        cantidad++;
        actual = actual->siguiente;
    }

    return cantidad;
}

// Funcion para liberar una lista de servicios
void liberarServicios(struct Servicio *inicio) {
    while (inicio != NULL) {
        struct Servicio *siguiente = inicio->siguiente;
        free(inicio);
        inicio = siguiente;
    }
}

// ---------- Lectura ----------

// Funcion para saber si una persona ya leyo un libro
int yaLeyo(const struct Persona *persona, const char *libro) {
    struct LibroEncontrado *actual = persona->librosLeidos;

    while (actual != NULL) {
        if (strcmp(actual->nombre, libro) == 0) {
            return 1;
        }
        actual = actual->siguiente;
    }

    return 0;
}

// Funcion para leer un libro: lo anota y, si completa 3, aprende el servicio
int leerLibro(struct Persona *persona, const char *libro) {
    if (persona == NULL || libro == NULL || yaLeyo(persona, libro)) {
        return 0;
    }

    persona->librosLeidos = agregarLibroEncontrado(persona->librosLeidos, libro);

    // Si no esta estudiando nada, el libro queda leido pero no cuenta para un servicio
    if (persona->servicioEnEstudio[0] == '\0') {
        return 0;
    }

    persona->librosParaServicio++;

    if (persona->librosParaServicio < LIBROS_PARA_APRENDER) {
        return 0;
    }

    // Completo los libros: aprende el servicio y queda libre para estudiar otro
    aprenderServicio(persona, persona->servicioEnEstudio);
    persona->servicioEnEstudio[0] = '\0';
    persona->librosParaServicio = 0;
    return 1;
}
