// Evita que este archivo se incluya dos veces
#ifndef INDICES_H
#define INDICES_H

#include "Comunas.h"

// Funcion para calcular la necesidad de una comuna
float calcularNecesidad(struct Comuna *comuna);
// Funcion para calcular el indice de reciprocidad (0 a 100): parte de 50 (neutral)
// y sube hacia 100 si da y recibe parejo, o baja hacia 0 si solo da o solo recibe,
// de a poco segun cuantos regalos ha intercambiado
float calcularReciprocidad(struct Comuna *comuna);
// Funcion para calcular la satisfaccion de una comuna
float calcularSatisfaccion(struct Comuna *comuna);
// Funcion para bajar penalizacion, solidaridad y apoyo recibido (una vez por dia)
void decaerEfectos(struct Comuna *sociedad);
// Funcion para recalcular los indices de toda la sociedad
void actualizarIndices(struct Comuna *sociedad);

// Fin de la proteccion contra inclusion doble
#endif