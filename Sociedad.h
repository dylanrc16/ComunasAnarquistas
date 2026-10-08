// Evita que este archivo se incluya dos veces
#ifndef SOCIEDAD_H
#define SOCIEDAD_H

#include "Comunas.h"
#include "Recurso.h"

// Funcion para crear la sociedad con N comunas y 500 personas
struct Comuna *crearSociedad(struct Comuna *nombresComunas, int cantidadComunas);

// Funcion para dar a cada comuna sus bienes y servicios iniciales
void asignarRecursos(struct Comuna *sociedad, struct Recurso *catalogoBienes, int cantidadBienes,struct Recurso *catalogoServicios, int cantidadServicios);

// Fin de la proteccion contra inclusion doble
#endif