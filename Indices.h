// Evita que este archivo se incluya dos veces
#ifndef INDICES_H
#define INDICES_H

#include "Comunas.h"

// Funcion para calcular la necesidad de una comuna
float calcularNecesidad(struct Comuna *comuna);
// Funcion para calcular la satisfaccion de una comuna
float calcularSatisfaccion(struct Comuna *comuna);
// Funcion para recalcular los indices de toda la sociedad
void actualizarIndices(struct Comuna *sociedad);

// Fin de la proteccion contra inclusion doble
#endif