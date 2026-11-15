// Evita que este archivo se incluya dos veces
#ifndef ECONOMIA_H
#define ECONOMIA_H

#include "Comunas.h"
#include "TablaPersonas.h"
#include "Jugador.h"
#include "Cola.h"

// Una comuna autonoma regala un bien cuando tiene mas de este porcentaje de su maximo
#define AUTONOMA_EXCEDENTE 70
// Solo le regala a una comuna que tenga menos de este porcentaje de ese bien
#define AUTONOMA_NECESITA 40
// Regala como maximo este porcentaje de su maximo por bien por dia
#define AUTONOMA_REGALO 10

// Funcion para regalar un recurso de una comuna a otra (economia del regalo).
// Devuelve la cantidad que realmente se regalo (0 si no se pudo).
int regalarRecurso(struct Comuna *da, struct Comuna *recibe, int esBien, const char *nombre, int cantidad);

// Funcion para "regalar" un servicio: una persona de la comuna que da, que sabe
// ese servicio, se muda a la comuna que recibe (con todos sus servicios).
// Solo se puede si en la comuna que da quedan al menos otra persona que lo sepa.
// Devuelve la persona que se mudo, o NULL si no se pudo.
struct Persona *regalarServicio(struct TablaPersonas *tabla, struct Comuna *da, struct Comuna *recibe, const char *servicio);

// Funcion para la ayuda mutua automatica de las comunas que NO administra el jugador.
// Se llama una vez por dia. Cada comuna autonoma:
//   - regala de cada bien que tenga sobre el 70% a la comuna que mas lo necesite (bajo 40%)
//   - si tiene mas personas de las que necesita en un servicio, muda una a la comuna
//     que no tenga a nadie con ese servicio (como maximo una mudanza por dia)
// Imprime los regalos que reciben las comunas del jugador y un resumen.
// Devuelve cuantos regalos se hicieron.
int ayudaAutonoma(struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Jugador *jugador);

// Funcion para atender la cola de pedidos de la red de ayuda mutua.
// Se llama una vez por dia, antes de ayudaAutonoma. Cada pedido se atiende en orden
// de llegada: la comuna autonoma con mas excedente de ese bien (sobre el 70%) le
// regala el 10% de su maximo. Si nadie puede, el pedido vuelve al final y espera.
// Devuelve cuantos pedidos se atendieron.
int atenderPedidos(struct Comuna *sociedad, struct Cola *cola, const struct Jugador *jugador);

// Fin de la proteccion contra inclusion doble
#endif