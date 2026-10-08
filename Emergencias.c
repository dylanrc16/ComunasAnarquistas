#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Emergencias.h"

// Cada cuantos dias ocurre una emergencia (se elige al azar en este rango)
#define MIN_DIAS_EMERGENCIA 3
#define MAX_DIAS_EMERGENCIA 7

// Porcentaje de la existencia que pierde un recurso afectado
#define MIN_PERDIDA 10
#define MAX_PERDIDA 50

// Cuanto sube la penalizacion de las comunas cercanas
#define PENALIZACION_VECINAS 10.0f

// Dias que faltan para la proxima emergencia
static int diasRestantes = 0;

// Funcion para sortear en cuantos dias sera la proxima emergencia (3 a 7)
static int diasHastaProximaEmergencia(void) {
    return MIN_DIAS_EMERGENCIA + rand() % (MAX_DIAS_EMERGENCIA - MIN_DIAS_EMERGENCIA + 1);
}

// Funcion para iniciar las emergencias. Se llama una sola vez al empezar el juego
void iniciarEmergencias(void) {
    // Usa la hora como semilla para que cada partida sea distinta
    srand((unsigned int) time(NULL));
    diasRestantes = diasHastaProximaEmergencia();
}

// Funcion para danar una lista de recursos: cada recurso tiene 50% de
// probabilidad de ser afectado y, si lo es, pierde entre 10% y 50%
static void golpearLista(struct Recurso *lista) {
    struct Recurso *actual = lista;

    while (actual != NULL) {
        // Moneda al aire: 50% de probabilidad
        if (rand() % 2 == 0) {
            // Porcentaje de perdida al azar entre 10 y 50
            int porcentaje = MIN_PERDIDA + rand() % (MAX_PERDIDA - MIN_PERDIDA + 1);
            int perdida = actual->existencia * porcentaje / 100;

            actual->existencia -= perdida;
            printf("   %s: -%d\n", actual->nombre, perdida);
        }

        actual = actual->siguiente;
    }
}

// Funcion para subir la penalizacion de las 2 comunas anteriores y las 2 siguientes
static void penalizarVecinas(struct Comuna *comuna) {
    // Hacia atras: hasta 2 comunas
    struct Comuna *vecina = comuna->anterior;

    for (int i = 0; i < 2 && vecina != NULL; i++) {
        vecina->penalizacion += PENALIZACION_VECINAS;
        vecina = vecina->anterior;
    }

    // Hacia adelante: hasta 2 comunas
    vecina = comuna->siguiente;

    for (int i = 0; i < 2 && vecina != NULL; i++) {
        vecina->penalizacion += PENALIZACION_VECINAS;
        vecina = vecina->siguiente;
    }
}

// Funcion para provocar una emergencia en una comuna al azar
static void provocarEmergencia(struct Comuna *sociedad) {
    int cantidad = 0;
    struct Comuna *comuna = sociedad;

    // Cuenta cuantas comunas hay
    while (comuna != NULL) {
        cantidad++;
        comuna = comuna->siguiente;
    }

    // Se elige una comuna al azar
    comuna = sociedad;
    for (int i = rand() % cantidad; i > 0; i--) {
        comuna = comuna->siguiente;
    }

    printf("\n!!! EMERGENCIA en %s !!!\n", comuna->nombre);

    // Dana sus bienes y servicios y castiga a sus vecinas
    golpearLista(comuna->bienes);
    golpearLista(comuna->servicios);
    penalizarVecinas(comuna);
}

// Funcion para avanzar las emergencias un dia. Se llama una vez por dia.
// Cuando se acaban los dias, ocurre la emergencia y se sortea la siguiente.
void avanzarEmergencias(struct Comuna *sociedad) {
    diasRestantes--;

    // Si ya toca, provoca la emergencia y reinicia la cuenta
    if (diasRestantes <= 0) {
        provocarEmergencia(sociedad);
        diasRestantes = diasHastaProximaEmergencia();
    }
}