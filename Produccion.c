#include <stdio.h>
#include "Produccion.h"

/* Le da a cada comuna su propio porcentaje de produccion (entre 2 y 5).
   Es el porcentaje de habitantes que aporta a producir cada bien en un turno.
   Se llama una sola vez, despues de crear la sociedad. */
void asignarProduccion(struct Comuna *sociedad)
{
    struct Comuna *comuna = sociedad;
    int numeroComuna = 0;

    while (comuna != NULL)
    {
        comuna->porcentajeProduccion = 2 + (numeroComuna * 5) % 4;
        numeroComuna++;
        comuna = comuna->siguiente;
    }
}

/* Cada turno:
   - cada bien sube personas * porcentajeProduccion / 100 (minimo 1)
   - cada servicio sube 1 (las personas se recuperan y vuelven a atender)
   Ninguno pasa de su maximo. */
void producirRecursos(struct Comuna *sociedad)
{
    struct Comuna *comuna = sociedad;

    while (comuna != NULL)
    {
        int produccion = comuna->cantidadPersonas * comuna->porcentajeProduccion / 100;

        if (produccion < 1)
        {
            produccion = 1;
        }

        struct Recurso *bien = comuna->bienes;

        while (bien != NULL)
        {
            bien->existencia += produccion;

            if (bien->existencia > bien->maximo)
            {
                bien->existencia = bien->maximo;
            }

            bien = bien->siguiente;
        }

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