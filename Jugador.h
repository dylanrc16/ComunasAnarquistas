// Evita que este archivo se incluya dos veces
#ifndef JUGADOR_H
#define JUGADOR_H

#include "Comunas.h"

// Cuantas comunas administra el jugador
#define COMUNAS_ADMINISTRADAS 3
// Cuantas comunas propias se pueden perder antes de perder la partida
#define VIDAS 3

// Nodo de la lista de comunas que administra el jugador.
// Solo apunta a la comuna: la comuna vive en la lista doble de la sociedad.
struct ComunaAdministrada {
    struct Comuna *comuna;  // Comuna que administra (no es una copia)
    struct ComunaAdministrada *siguiente;  // Siguiente comuna administrada
};

// Lo que maneja el jugador
struct Jugador {
    struct ComunaAdministrada *comunas;  // Sus comunas (como maximo 3)
    int vidasPerdidas;  // Cuantas de sus comunas se han disuelto
};

// Funcion para dejar al jugador sin comunas y sin vidas perdidas
void iniciarJugador(struct Jugador *jugador);
// Funcion para saber si una comuna es del jugador (1 si, 0 no)
int estaAdministrada(const struct Jugador *jugador, const struct Comuna *comuna);
// Funcion para contar cuantas comunas administra
int contarAdministradas(const struct Jugador *jugador);
// Funcion para agregar una comuna a la lista del jugador.
// Devuelve 1 si la agrego y 0 si ya estaba o si ya tiene 3.
int agregarAdministrada(struct Jugador *jugador, struct Comuna *comuna);
// Funcion para sacar una comuna de la lista del jugador (no libera la comuna).
// Devuelve 1 si estaba y la saco, 0 si no estaba.
int quitarAdministrada(struct Jugador *jugador, struct Comuna *comuna);
// Funcion para liberar la lista del jugador (solo los nodos, no las comunas)
void liberarJugador(struct Jugador *jugador);

// Fin de la proteccion contra inclusion doble
#endif
