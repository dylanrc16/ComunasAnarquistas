#include <stdio.h>
#include <stdlib.h>
#include "Produccion.h"

// Funcion para sumar la produccion a cada recurso de una lista, sin pasar del maximo
static void producirLista(struct Recurso *lista, float produccionPorDia)
{
    struct Recurso *actual = lista;

    while (actual != NULL)
    {
        // Cada recurso se redondea por separado, asi no todos suben igual
        actual->existencia += redondearAlAzar(produccionPorDia);

        // No deja pasar del maximo
        if (actual->existencia > actual->maximo)
        {
            actual->existencia = actual->maximo;
        }

        actual = actual->siguiente;
    }
}

// Funcion para dar a cada comuna su porcentaje de produccion, al azar entre 2.0 y 5.0.
// Se llama una sola vez, despues de crear la sociedad (y despues de srand).
void asignarProduccion(struct Comuna *sociedad)
{
    struct Comuna *comuna = sociedad;

    while (comuna != NULL)
    {
        // rand() % 301 da 0..300, o sea 2.00 a 5.00 en pasos de 0.01
        comuna->porcentajeProduccion = 2.0f + (rand() % 301) / 100.0f;
        comuna = comuna->siguiente;
    }
}

// Funcion para producir los bienes de un dia: personas * porcentajeProduccion / 100,
// sin pasar del maximo. Los servicios no se producen: salen de lo que saben
// las personas (se recalculan con recalcularServicios).
void producirRecursos(struct Comuna *sociedad)
{
    struct Comuna *comuna = sociedad;

    while (comuna != NULL)
    {
        // Cuanto produce esta comuna de cada bien (con decimales)
        float produccionBien = comuna->cantidadPersonas * comuna->porcentajeProduccion / 100.0f;

        producirLista(comuna->bienes, produccionBien);

        comuna = comuna->siguiente;
    }
}