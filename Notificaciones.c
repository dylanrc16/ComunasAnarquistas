#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Notificaciones.h"
#include "Comunas.h"

// Dia actual de la simulacion, para ponerle fecha a cada notificacion
static int diaActual = 1;

// Funcion para dejar la bandeja vacia
void iniciarBandeja(struct Bandeja *bandeja) {
    bandeja->tope = NULL;
    bandeja->cantidad = 0;
    bandeja->noLeidas = 0;
}

// Funcion para apilar una notificacion en el tope en O(1)
void apilarNotificacion(struct Bandeja *bandeja, int dia, const char *texto) {
    struct Notificacion *nueva = calloc(1, sizeof(struct Notificacion));

    if (nueva == NULL) {
        return;
    }

    nueva->dia = dia;
    snprintf(nueva->texto, sizeof(nueva->texto), "%s", texto);

    // La nueva apunta a la que estaba arriba y pasa a ser el tope
    nueva->siguiente = bandeja->tope;
    bandeja->tope = nueva;
    bandeja->cantidad++;
    bandeja->noLeidas++;

    // Si hay demasiadas, se descarta la del fondo (la mas vieja)
    if (bandeja->cantidad > NOTIFICACIONES_MAXIMAS) {
        struct Notificacion *penultima = bandeja->tope;

        while (penultima->siguiente->siguiente != NULL) {
            penultima = penultima->siguiente;
        }

        free(penultima->siguiente);
        penultima->siguiente = NULL;
        bandeja->cantidad--;

        if (bandeja->noLeidas > bandeja->cantidad) {
            bandeja->noLeidas = bandeja->cantidad;
        }
    }
}

// Funcion para sacar la notificacion del tope en O(1)
struct Notificacion *desapilarNotificacion(struct Bandeja *bandeja) {
    if (bandeja->tope == NULL) {
        return NULL;
    }

    struct Notificacion *tope = bandeja->tope;
    bandeja->tope = tope->siguiente;
    bandeja->cantidad--;

    if (bandeja->noLeidas > bandeja->cantidad) {
        bandeja->noLeidas = bandeja->cantidad;
    }

    tope->siguiente = NULL;
    return tope;
}

// Funcion para liberar todas las notificaciones
void liberarBandeja(struct Bandeja *bandeja) {
    while (bandeja->tope != NULL) {
        free(desapilarNotificacion(bandeja));
    }
}

// Funcion para fijar el dia actual
void fijarDiaNotificaciones(int dia) {
    diaActual = dia;
}

// Funcion para avisarle algo a una comuna. Usa los "..." como printf:
// vsnprintf arma el texto con el formato y los valores que se le pasen.
void notificar(struct Comuna *comuna, const char *formato, ...) {
    if (comuna == NULL) {
        return;
    }

    char texto[LARGO_NOTIFICACION];
    va_list valores;
    va_start(valores, formato);
    vsnprintf(texto, sizeof(texto), formato, valores);
    va_end(valores);

    apilarNotificacion(&comuna->bandeja, diaActual, texto);
}
