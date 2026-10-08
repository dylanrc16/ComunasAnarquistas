#include <stdio.h>
#include "Economia.h"

// La comuna que regala nunca baja de este porcentaje de su maximo:
// se regala el excedente, no lo que hace falta para vivir
#define RESERVA_PORCENTAJE 40

// Solidaridad que gana la comuna que regala
#define BONO_SOLIDARIDAD 5.0f

// Funcion para regalar "cantidad" del recurso "nombre" de la comuna "da" a "recibe".
// Nadie recibe nada a cambio y no se lleva cuenta de deudas.
// Se regala menos de lo pedido si a quien regala le quedaria menos
// de su reserva o si a quien recibe no le cabe (maximo).
int regalarRecurso(struct Comuna *da, struct Comuna *recibe, int esBien, const char *nombre, int cantidad)
{
    // Valida que haya dos comunas distintas y una cantidad positiva
    if (da == NULL || recibe == NULL || da == recibe || cantidad <= 0)
    {
        return 0;
    }

    // Elige la lista de bienes o de servicios de quien regala
    struct Recurso *listaDa;
    
    if (esBien) {
        listaDa = da->bienes;
    } else {
        listaDa = da->servicios;
    }


    // Elige la misma lista en la comuna que recibe
    struct Recurso *listaRecibe;

    if (esBien) {
        listaRecibe = recibe->bienes;
    } else {
        listaRecibe = recibe->servicios;
    }

    // Busca el recurso en las dos comunas
    struct Recurso *origen = buscarRecurso(listaDa, nombre);
    struct Recurso *destino = buscarRecurso(listaRecibe, nombre);

    if (origen == NULL || destino == NULL)
    {
        return 0;
    }

    // Lo que sobra por encima de la reserva
    int excedente = origen->existencia - origen->maximo * RESERVA_PORCENTAJE / 100;
    // Lo que le cabe a la comuna que recibe
    int espacio = destino->maximo - destino->existencia;

    // El regalo es lo pedido, recortado por excedente y espacio
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

    // Mueve el recurso de una comuna a la otra
    origen->existencia -= regalo;
    destino->existencia += regalo;

    // Quien regala gana reconocimiento de su comunidad, no una moneda
    da->solidaridad += BONO_SOLIDARIDAD;

    return regalo;
}