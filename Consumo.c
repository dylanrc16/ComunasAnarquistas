#include <stdio.h>
#include "Consumo.h"

/* Le da a cada comuna su propio porcentaje de consumo (entre 2 y 5).
   Es el porcentaje de habitantes que gasta cada bien en un turno.
   Se llama una sola vez, despues de crear la sociedad. */
void asignarConsumo(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;
    int numeroComuna = 0;

    while (comuna != NULL) {
        comuna->porcentajeConsumo = 2 + (numeroComuna * 3) % 4;
        numeroComuna++;
        comuna = comuna->siguiente;
    }
}

/* Cada turno, cada bien de cada comuna baja:
       gasto = personas * porcentajeConsumo / 100   (minimo 1)
   La existencia nunca queda por debajo de 0. */
void consumirBienes(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        int gasto = comuna->cantidadPersonas * comuna->porcentajeConsumo / 100;

        if (gasto < 1) {
            gasto = 1;
        }

        struct Recurso *bien = comuna->bienes;

        while (bien != NULL) {
            bien->existencia -= gasto;

            if (bien->existencia < 0) {
                bien->existencia = 0;
            }

            bien = bien->siguiente;
        }

        comuna = comuna->siguiente;
    }
}