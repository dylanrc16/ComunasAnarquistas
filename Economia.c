#include <stdio.h>
#include "Economia.h"
#include "Sociedad.h"

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

// ---------- Ayuda mutua automatica ----------

// Una comuna autonoma regala un bien cuando tiene mas de este porcentaje de su maximo
#define AUTONOMA_EXCEDENTE 70
// Solo le regala a una comuna que tenga menos de este porcentaje de ese bien
#define AUTONOMA_NECESITA 40
// Regala como maximo este porcentaje de su maximo por bien por dia
#define AUTONOMA_REGALO 10

// Funcion para buscar la comuna que mas necesita un bien: la de menor porcentaje,
// siempre que este bajo AUTONOMA_NECESITA. Devuelve NULL si nadie lo necesita.
static struct Comuna *comunaQueMasNecesitaBien(struct Comuna *sociedad, struct Comuna *da, const char *bien) {
    struct Comuna *elegida = NULL;
    float menorPorcentaje = AUTONOMA_NECESITA;

    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        struct Recurso *recurso = buscarRecurso(comuna->bienes, bien);

        if (comuna == da || recurso == NULL || recurso->maximo == 0) {
            continue;
        }

        float porcentaje = 100.0f * recurso->existencia / recurso->maximo;

        if (porcentaje < menorPorcentaje) {
            menorPorcentaje = porcentaje;
            elegida = comuna;
        }
    }

    return elegida;
}

// Funcion para contar cuantas personas de una comuna saben un servicio
// (sin el tope del maximo que usa recalcularServicios)
static int personasQueSaben(const struct TablaPersonas *tabla, const struct Comuna *comuna, const char *servicio) {
    int cantidad = 0;

    for (int i = 0; i < TAMANO_TABLA; i++) {
        for (struct Persona *persona = tabla->casillas[i]; persona != NULL; persona = persona->siguiente) {
            if (persona->comuna == comuna && sabeServicio(persona, servicio)) {
                cantidad++;
            }
        }
    }

    return cantidad;
}

// Funcion para buscar una comuna donde nadie sepa un servicio
static struct Comuna *comunaSinServicio(struct Comuna *sociedad, struct Comuna *da, const char *servicio) {
    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        struct Recurso *recurso = buscarRecurso(comuna->servicios, servicio);

        if (comuna != da && recurso != NULL && recurso->existencia == 0) {
            return comuna;
        }
    }

    return NULL;
}

// Funcion para la ayuda mutua automatica de las comunas autonomas
int ayudaAutonoma(struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Jugador *jugador) {
    int regalos = 0;

    for (struct Comuna *da = sociedad; da != NULL; da = da->siguiente) {
        // Las comunas del jugador no regalan solas: eso lo decide el jugador
        if (estaAdministrada(jugador, da)) {
            continue;
        }

        // 1. Bienes: regala lo que le sobra a quien mas lo necesita
        for (struct Recurso *bien = da->bienes; bien != NULL; bien = bien->siguiente) {
            if (bien->maximo == 0 || 100 * bien->existencia < AUTONOMA_EXCEDENTE * bien->maximo) {
                continue;
            }

            struct Comuna *recibe = comunaQueMasNecesitaBien(sociedad, da, bien->nombre);

            if (recibe == NULL) {
                continue;
            }

            int cantidad = bien->maximo * AUTONOMA_REGALO / 100;
            int regalado = regalarRecurso(da, recibe, 1, bien->nombre, cantidad);

            if (regalado > 0) {
                regalos++;

                // Solo se anuncian los regalos que reciben las comunas del jugador
                if (estaAdministrada(jugador, recibe)) {
                    printf("Ayuda mutua: %s le regalo %d de %s a su comuna %s\n",
                           da->nombre, regalado, bien->nombre, recibe->nombre);
                }
            }
        }

        // 2. Servicios: si le sobran personas en un servicio, una se muda
        //    a una comuna donde nadie lo sabe (maximo una mudanza por dia)
        for (struct Recurso *servicio = da->servicios; servicio != NULL; servicio = servicio->siguiente) {
            if (personasQueSaben(tabla, da, servicio->nombre) <= servicio->maximo) {
                continue;
            }

            struct Comuna *recibe = comunaSinServicio(sociedad, da, servicio->nombre);

            if (recibe == NULL) {
                continue;
            }

            struct Persona *persona = regalarServicio(tabla, da, recibe, servicio->nombre);

            if (persona != NULL) {
                regalos++;

                if (estaAdministrada(jugador, recibe)) {
                    printf("Ayuda mutua: %s se mudo de %s a su comuna %s (%s)\n",
                           persona->nombre, da->nombre, recibe->nombre, servicio->nombre);
                }

                // Cambiaron las personas: se recalculan antes de seguir
                recalcularServicios(sociedad, tabla);
                break;
            }
        }
    }

    if (regalos > 0) {
        printf("Ayuda mutua entre comunas: %d regalos hoy\n", regalos);
    }

    return regalos;
}
