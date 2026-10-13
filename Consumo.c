#include <stdio.h>
#include <stdlib.h>
#include "Consumo.h"

// Funcion para restar el gasto a cada recurso de una lista, sin bajar de 0
static void gastarLista(struct Recurso *lista, float gastoPorDia) {
    struct Recurso *actual = lista;

    while (actual != NULL) {
        // Cada recurso se redondea por separado, asi no todos bajan igual
        actual->existencia -= redondearAlAzar(gastoPorDia);

        // No deja existencias negativas
        if (actual->existencia < 0) {
            actual->existencia = 0;
        }

        actual = actual->siguiente;
    }
}

// Funcion para dar a cada comuna su porcentaje de consumo, al azar entre 2.0 y 5.0.
// Se llama una sola vez, despues de crear la sociedad (y despues de srand).
void asignarConsumo(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        // rand() % 301 da 0..300, o sea 2.00 a 5.00 en pasos de 0.01
        comuna->porcentajeConsumo = 2.0f + (rand() % 301) / 100.0f;
        comuna = comuna->siguiente;
    }
}

// Funcion para gastar los bienes y servicios de un dia.
// Bienes: personas * porcentajeConsumo / 100 por dia.
// Servicios: lo mismo dividido entre 10, igual que su maximo (1 por cada 10 personas).
void consumirRecursos(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        // Cuanto gasta esta comuna de cada bien (con decimales)
        float gastoBien = comuna->cantidadPersonas * comuna->porcentajeConsumo / 100.0f;

        gastarLista(comuna->bienes, gastoBien);
        gastarLista(comuna->servicios, gastoBien / 10.0f);

        comuna = comuna->siguiente;
    }
}