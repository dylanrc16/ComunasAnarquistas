// Evita que este archivo se incluya dos veces
#ifndef CONSUMO_H
#define CONSUMO_H

#include "Comunas.h"

// Funcion para dar a cada comuna su porcentaje de consumo
void asignarConsumo(struct Comuna *sociedad);
// Funcion para gastar los bienes de un dia
void consumirBienes(struct Comuna *sociedad);

// Fin de la proteccion contra inclusion doble
#endif