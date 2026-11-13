#include <stdio.h>
#include <stdlib.h>
#include "Jugador.h"

// Funcion para dejar al jugador sin comunas y sin vidas perdidas
void iniciarJugador(struct Jugador *jugador) {
    jugador->comunas = NULL;
    jugador->vidasPerdidas = 0;
}

// Funcion para saber si una comuna es del jugador: compara punteros,
// porque la lista guarda la direccion de la comuna, no una copia
int estaAdministrada(const struct Jugador *jugador, const struct Comuna *comuna) {
    struct ComunaAdministrada *actual = jugador->comunas;

    while (actual != NULL) {
        if (actual->comuna == comuna) {
            return 1;
        }
        actual = actual->siguiente;
    }

    return 0;
}

// Funcion para contar cuantas comunas administra
int contarAdministradas(const struct Jugador *jugador) {
    int cantidad = 0;
    struct ComunaAdministrada *actual = jugador->comunas;

    while (actual != NULL) {
        cantidad++;
        actual = actual->siguiente;
    }

    return cantidad;
}

// Funcion para agregar una comuna al final de la lista del jugador
int agregarAdministrada(struct Jugador *jugador, struct Comuna *comuna) {
    if (comuna == NULL || estaAdministrada(jugador, comuna) ||
        contarAdministradas(jugador) >= COMUNAS_ADMINISTRADAS) {
        return 0;
    }

    struct ComunaAdministrada *nueva = calloc(1, sizeof(struct ComunaAdministrada));

    if (nueva == NULL) {
        return 0;
    }

    nueva->comuna = comuna;

    if (jugador->comunas == NULL) {
        jugador->comunas = nueva;
        return 1;
    }

    struct ComunaAdministrada *actual = jugador->comunas;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nueva;
    return 1;
}

// Funcion para sacar una comuna de la lista. Se libera el nodo de la lista,
// pero NO la comuna: la comuna se libera aparte, en la lista doble de la sociedad.
int quitarAdministrada(struct Jugador *jugador, struct Comuna *comuna) {
    struct ComunaAdministrada *anterior = NULL;
    struct ComunaAdministrada *actual = jugador->comunas;

    while (actual != NULL) {
        if (actual->comuna == comuna) {
            // Reengancha la lista saltandose el nodo
            if (anterior == NULL) {
                jugador->comunas = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }

            free(actual);
            return 1;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    return 0;
}

// Funcion para liberar la lista del jugador (solo los nodos)
void liberarJugador(struct Jugador *jugador) {
    struct ComunaAdministrada *actual = jugador->comunas;

    while (actual != NULL) {
        struct ComunaAdministrada *siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }

    jugador->comunas = NULL;
}
