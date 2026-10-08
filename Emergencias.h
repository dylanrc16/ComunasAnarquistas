// Evita que este archivo se incluya dos veces
#ifndef EMERGENCIAS_H
#define EMERGENCIAS_H

#include "Comunas.h"

// Funcion para preparar el sistema de emergencias
void iniciarEmergencias(void);
// Funcion para avanzar un dia y provocar una emergencia si toca
void avanzarEmergencias(struct Comuna *sociedad);

// Fin de la proteccion contra inclusion doble
#endif