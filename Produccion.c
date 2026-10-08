#include <stdio.h>
#include "Produccion.h"

// Funcion para dar a cada comuna su porcentaje de produccion (entre 2 y 5).
// Es el porcentaje de habitantes que aporta a producir cada bien en un turno.
// Se llama una sola vez, despues de crear la sociedad.
void asignarProduccion(struct Comuna *sociedad)
{
    struct Comuna *comuna = sociedad;
    int numeroComuna = 0;

    while (comuna != NULL)
    {
        // Da 2, 3, 4, 5, 2, 3... segun la posicion de la comuna
        comuna->porcentajeProduccion = 2 + (numeroComuna * 5) % 4;
        numeroComuna++;
        comuna = comuna->siguiente;
    }
}

// Funcion para producir los recursos de un dia.
// Cada bien sube personas * porcentajeProduccion / 100 (minimo 1).
// Cada servicio sube 1. Ninguno pasa de su maximo.
void producirRecursos(struct Comuna *sociedad)
{
    struct Comuna *comuna = sociedad;

    while (comuna != NULL)
    {
        // Calcula cuanto produce esta comuna de cada bien
        int produccion = comuna->cantidadPersonas * comuna->porcentajeProduccion / 100;

        // Toda comuna produce al menos 1
        if (produccion < 1)
        {
            produccion = 1;
        }

        // Suma la produccion a cada bien
        struct Recurso *bien = comuna->bienes;

        while (bien != NULL)
        {
            bien->existencia += produccion;

            // No deja pasar del maximo
            if (bien->existencia > bien->maximo)
            {
                bien->existencia = bien->maximo;
            }

            bien = bien->siguiente;
        }

        // Los servicios se recuperan de 1 en 1
        struct Recurso *servicio = comuna->servicios;

        while (servicio != NULL)
        {
            if (servicio->existencia < servicio->maximo)
            {
                servicio->existencia++;
            }

            servicio = servicio->siguiente;
        }

        comuna = comuna->siguiente;
    }
}