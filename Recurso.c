#include <stdio.h>
#include <string.h>

struct Recurso *crearRecurso(char nombre, int existencia, int maximo){

    struct Recurso *nuevo = calloc(1, sizeof(Recurso));

    if (nuevo == NULL){
        return NULL;
    }

    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", nombre);
    nuevo->existencia = existencia;
    nuevo->maximo = maximo;

    return nuevo;  
}

struct Recurso *agregarFinal(struct Recurso *inicio, char *nombre, int existencia, int maximo){

    struct Recurso *nueva = crearRecurso(nombre, existencia, maximo);

    if (nueva == NULL){
        return NULL;
    }

    if (inicio == NULL){
        return nueva;
    }

    struct Recurso *act = inicio;

    while(act->siguiente != NULL){
        act = act->siguiente;
    }
    act->siguiente = nueva;

    return inicio;
}

struct Recurso *buscarRecurso(struct Recurso *inicio, char *nombre) {
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
        printf("%s: %d/%d\n",
               actual->nombre,
               actual->existencia,
               actual->maximo);

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

struct Recurso *cargarRecursos(char *nombreArchivo) {
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
