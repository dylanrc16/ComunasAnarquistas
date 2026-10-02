#ifndef ECONOMIA_H
#define ECONOMIA_H

#include "Comunas.h"

/* Economia del regalo + ayuda mutua.
   Devuelve la cantidad que realmente se regalo (0 si no se pudo). */
int regalarRecurso(struct Comuna *da, struct Comuna *recibe, int esBien, const char *nombre, int cantidad);

#endif