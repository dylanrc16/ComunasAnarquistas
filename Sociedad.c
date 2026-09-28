#include <stdio.h>
#include "Sociedad.h"

struct Comuna *crearSociedad(struct Comuna *nombresComunas,
                            int cantidadComunas) {
    if (cantidadComunas < 1 || cantidadComunas > 50) {
        printf("La cantidad de comunas debe estar entre 1 y 50\n");
        return NULL;
    }

    int personasPorComuna = 500 / cantidadComunas;
    int personasSobrantes = 500 % cantidadComunas;

    struct Comuna *sociedad = NULL;
    struct Comuna *actual = nombresComunas;

    for (int i = 0; i < cantidadComunas; i++) {
        if (actual == NULL) {
            printf("No hay suficientes nombres en el archivo comunas\n");
            return NULL;
        }

        int habitantes = personasPorComuna;

        if (i < personasSobrantes) {
            habitantes++;
        }

        sociedad = agregarComunaFinal(
            sociedad,
            actual->nombre,
            habitantes
        );

        actual = actual->siguiente;
    }

    return sociedad;
}