#include <stdio.h>
#include "Economia.h"

/* La comuna que regala nunca baja de este porcentaje de su maximo:
   se regala el excedente, no lo que hace falta para vivir */
#define RESERVA_PORCENTAJE 40

#define BONO_SOLIDARIDAD 5.0f

/* Regala "cantidad" del recurso "nombre" de la comuna "da" a "recibe".
   Nadie recibe nada a cambio y no se lleva ninguna cuenta de deudas.
   Se regala menos de lo pedido si:
     - a quien regala le quedaria menos de su reserva
     - a quien recibe no le cabe (maximo) */
int regalarRecurso(struct Comuna *da, struct Comuna *recibe, int esBien, const char *nombre, int cantidad)
{
    if (da == NULL || recibe == NULL || da == recibe || cantidad <= 0)
    {
        return 0;
    }

    struct Recurso *listaDa;
    
    if (esBien) {
        listaDa = da->bienes;
    } else {
        listaDa = da->servicios;
    }


    struct Recurso *listaRecibe;

    if (esBien) {
        listaRecibe = recibe->bienes;
    } else {
        listaRecibe = recibe->servicios;
    }

    struct Recurso *origen = buscarRecurso(listaDa, nombre);
    struct Recurso *destino = buscarRecurso(listaRecibe, nombre);

    if (origen == NULL || destino == NULL)
    {
        return 0;
    }

    int excedente = origen->existencia - origen->maximo * RESERVA_PORCENTAJE / 100;
    int espacio = destino->maximo - destino->existencia;

    int regalo = cantidad;
    if (regalo > excedente)
    {
        regalo = excedente;
    }
    if (regalo > espacio)
    {
        regalo = espacio;
    }
    if (regalo <= 0)
    {
        return 0;
    }

    origen->existencia -= regalo;
    destino->existencia += regalo;

    /* quien regala gana reconocimiento de su comunidad, no una moneda */
    da->solidaridad += BONO_SOLIDARIDAD;

    return regalo;
}