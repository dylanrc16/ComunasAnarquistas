#include <stdio.h>
#include "Economia.h"

// La comuna que regala nunca baja de este porcentaje de su maximo:
// se regala el excedente, no lo que hace falta para vivir
#define RESERVA_PORCENTAJE 40

// Solidaridad que gana la comuna que regala
#define BONO_SOLIDARIDAD 5.0f

// Tope de la solidaridad: es reconocimiento social, no algo que se acumule sin fin
#define SOLIDARIDAD_MAXIMA 15.0f

// Apoyo que siente la comuna que recibe un regalo, y su tope
#define BONO_APOYO 5.0f
#define APOYO_MAXIMO 15.0f

// Funcion para anotar un regalo en las dos comunas: cuenta el regalo dado y el
// recibido (para la reciprocidad), da solidaridad a quien regala y apoyo a quien recibe
static void registrarRegalo(struct Comuna *da, struct Comuna *recibe) {
    da->regalosDados++;
    recibe->regalosRecibidos++;

    // Quien regala gana reconocimiento de su comunidad, no una moneda
    da->solidaridad += BONO_SOLIDARIDAD;
    if (da->solidaridad > SOLIDARIDAD_MAXIMA) {
        da->solidaridad = SOLIDARIDAD_MAXIMA;
    }

    // Quien recibe siente el alivio de la ayuda
    recibe->apoyoRecibido += BONO_APOYO;
    if (recibe->apoyoRecibido > APOYO_MAXIMO) {
        recibe->apoyoRecibido = APOYO_MAXIMO;
    }
}

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

    // Anota el regalo: contadores, solidaridad y apoyo
    registrarRegalo(da, recibe);

    return regalo;
}

// Funcion para "regalar" un servicio mudando a una persona que lo sabe.
// Es como la reserva de los bienes: la comuna que da no se queda sin el servicio.
struct Persona *regalarServicio(struct TablaPersonas *tabla, struct Comuna *da, struct Comuna *recibe, const char *servicio) {
    if (da == NULL || recibe == NULL || da == recibe || servicio == NULL) {
        return NULL;
    }

    // Busca en la tabla a las personas de la comuna que da que saben el servicio
    int cuantasSaben = 0;
    struct Persona *candidata = NULL;

    for (int i = 0; i < TAMANO_TABLA; i++) {
        for (struct Persona *persona = tabla->casillas[i]; persona != NULL; persona = persona->siguiente) {
            if (persona->comuna == da && sabeServicio(persona, servicio)) {
                cuantasSaben++;

                if (candidata == NULL) {
                    candidata = persona;
                }
            }
        }
    }

    // Tiene que quedar al menos una persona que lo sepa en la comuna que da
    if (cuantasSaben < 2) {
        return NULL;
    }

    // La persona se muda: basta con cambiar su puntero a la comuna
    candidata->comuna = recibe;

    // Anota el regalo: contadores, solidaridad y apoyo
    registrarRegalo(da, recibe);

    return candidata;
}
