// Evita que este archivo se incluya dos veces
#ifndef PRODUCCION_H
#define PRODUCCION_H
 
#include "Comunas.h"
 
// Funcion para dar a cada comuna su porcentaje de produccion
void asignarProduccion(struct Comuna *sociedad);
// Funcion para producir los recursos de un dia
void producirRecursos(struct Comuna *sociedad);
 
// Fin de la proteccion contra inclusion doble
#endif
 