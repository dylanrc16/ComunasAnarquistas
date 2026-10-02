#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Emergencias.h"

/* Cada cuantos dias ocurre una emergencia (se elige al azar en este rango) */
#define MIN_DIAS_EMERGENCIA 3
#define MAX_DIAS_EMERGENCIA 7

/* Porcentaje de la existencia que pierde un recurso afectado */
#define MIN_PERDIDA 10
#define MAX_PERDIDA 50

/* Cuanto sube la penalizacion de las comunas cercanas */
#define PENALIZACION_VECINAS 10.0f

static int diasRestantes = 0;

static int diasHastaProximaEmergencia(void) {
    return MIN_DIAS_EMERGENCIA + rand() % (MAX_DIAS_EMERGENCIA - MIN_DIAS_EMERGENCIA + 1);
}

/* Se llama una sola vez al empezar el juego */
void iniciarEmergencias(void) {
    srand((unsigned int) time(NULL));
    diasRestantes = diasHastaProximaEmergencia();
}

/* Cada recurso tiene 50% de probabilidad de ser afectado, y si lo es
   pierde entre 10% y 50% de su existencia */
static void golpearLista(struct Recurso *lista) {
    struct Recurso *actual = lista;

    while (actual != NULL) {
        if (rand() % 2 == 0) {
            int porcentaje = MIN_PERDIDA + rand() % (MAX_PERDIDA - MIN_PERDIDA + 1);
            int perdida = actual->existencia * porcentaje / 100;

            actual->existencia -= perdida;
            printf("   %s: -%d\n", actual->nombre, perdida);
        }

        actual = actual->siguiente;
    }
}

/* Sube la penalizacion de las 2 comunas anteriores y las 2 siguientes */
static void penalizarVecinas(struct Comuna *comuna) {
    struct Comuna *vecina = comuna->anterior;

    for (int i = 0; i < 2 && vecina != NULL; i++) {
        vecina->penalizacion += PENALIZACION_VECINAS;
        vecina = vecina->anterior;
    }

    vecina = comuna->siguiente;

    for (int i = 0; i < 2 && vecina != NULL; i++) {
        vecina->penalizacion += PENALIZACION_VECINAS;
        vecina = vecina->siguiente;
    }
}

static void provocarEmergencia(struct Comuna *sociedad) {
    int cantidad = 0;
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        cantidad++;
        comuna = comuna->siguiente;
    }

    /* se elige una comuna al azar */
    comuna = sociedad;
    for (int i = rand() % cantidad; i > 0; i--) {
        comuna = comuna->siguiente;
    }

    printf("\n!!! EMERGENCIA en %s !!!\n", comuna->nombre);

    golpearLista(comuna->bienes);
    golpearLista(comuna->servicios);
    penalizarVecinas(comuna);
}

/* Se llama una vez por dia. Cuando se acaban los dias, ocurre la emergencia
   y se sortea cuando sera la siguiente. */
void avanzarEmergencias(struct Comuna *sociedad) {
    diasRestantes--;

    if (diasRestantes <= 0) {
        provocarEmergencia(sociedad);
        diasRestantes = diasHastaProximaEmergencia();
    }
}