#include <stdio.h>
#include "Consumo.h"

// Funcion para dar a cada comuna su porcentaje de consumo (entre 2 y 5).
// Es el porcentaje de habitantes que gasta cada bien en un turno.
// Se llama una sola vez, despues de crear la sociedad.
void asignarConsumo(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;
    int numeroComuna = 0;

    while (comuna != NULL) {
        // Da 2, 5, 4, 3, 2, 5... segun la posicion de la comuna
        comuna->porcentajeConsumo = 2 + (numeroComuna * 3) % 4;
        numeroComuna++;
        comuna = comuna->siguiente;
    }
}

// Funcion para gastar los bienes de un dia.
// gasto = personas * porcentajeConsumo / 100 (minimo 1). Nunca baja de 0.
void consumirBienes(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        // Calcula cuanto gasta esta comuna de cada bien
        int gasto = comuna->cantidadPersonas * comuna->porcentajeConsumo / 100;

        // Toda comuna gasta al menos 1
        if (gasto < 1) {
            gasto = 1;
        }

        struct Recurso *bien = comuna->bienes;

        // Resta el gasto a cada bien
        while (bien != NULL) {
            bien->existencia -= gasto;

            // No deja existencias negativas
            if (bien->existencia < 0) {
                bien->existencia = 0;
            }

            bien = bien->siguiente;
        }

        comuna = comuna->siguiente;
    }
}