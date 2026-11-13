// Evita que este archivo se incluya dos veces
#ifndef ECONOMIA_H
#define ECONOMIA_H

#include "Comunas.h"
#include "TablaPersonas.h"

// Funcion para regalar un recurso de una comuna a otra (economia del regalo).
// Devuelve la cantidad que realmente se regalo (0 si no se pudo).
int regalarRecurso(struct Comuna *da, struct Comuna *recibe, int esBien, const char *nombre, int cantidad);

// Funcion para "regalar" un servicio: una persona de la comuna que da, que sabe
// ese servicio, se muda a la comuna que recibe (con todos sus servicios).
// Solo se puede si en la comuna que da quedan al menos otra persona que lo sepa.
// Devuelve la persona que se mudo, o NULL si no se pudo.
struct Persona *regalarServicio(struct TablaPersonas *tabla, struct Comuna *da, struct Comuna *recibe, const char *servicio);

// Fin de la proteccion contra inclusion doble
#endif