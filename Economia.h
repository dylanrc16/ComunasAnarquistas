// Evita que este archivo se incluya dos veces
#ifndef ECONOMIA_H
#define ECONOMIA_H

#include "Comunas.h"

// Funcion para regalar un recurso de una comuna a otra (economia del regalo).
// Devuelve la cantidad que realmente se regalo (0 si no se pudo).
int regalarRecurso(struct Comuna *da, struct Comuna *recibe, int esBien, const char *nombre, int cantidad);

// Fin de la proteccion contra inclusion doble
#endif