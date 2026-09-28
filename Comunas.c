#include <stdio.h>
#include <string.h>

struct Comuna *crearComuna(char *nombre, int cantidadPersonas){
    if(nombre == NULL || cantidadPersonas < 0){
        return -1;
    }
    struct Comuna *nueva = calloc(1, sizeof(Comuna));

    nueva->nombre = nombre;
    nueva->cantidadPersonas = cantidadPersonas;

    return nueva;
}

struct Comuna *agregarFinal(struct Comuna *inicio, char *nombre, int cantidadPersonas){

    struct Comuna *nueva = crearComuna(nombre, cantidadPersonas);

    if (nueva == NULL){
        return NULL;
    }

    if (inicio == NULL){
        return nueva;
    }

    struct Comuna *act = inicio;

    while(act->siguiente != NULL){
        act = act->siguiente;
    }
    act->siguiente = nueva;

    return inicio;
}

void imprimir(struct Comuna *inicio) {

    struct Comuna *act = inicio;

    while (act != NULL) {

        printf("%d\n", act->nombre, act->cantidadPersonas);
        act = act->siguiente;
    }
}

struct Comuna *buscar(struct Comuna *inicio, char *nombre) {

    struct Comuna *act = inicio;
    while (act != NULL) {

        if (act->nombre == nombre) {
            return act;
        }
        act = act->siguiente;
    }
    return NULL;
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
            inicio = agregarFinal(inicio, nombre, 0);
        }
    }

    fclose(archivo);
    return inicio;
}




