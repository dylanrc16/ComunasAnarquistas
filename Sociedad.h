#ifndef SOCIEDAD_H
#define SOCIEDAD_H

#include "Comunas.h"
#include "Recurso.h"

struct Comuna *crearSociedad(struct Comuna *nombresComunas, int cantidadComunas);

void asignarRecursos(struct Comuna *sociedad, struct Recurso *catalogoBienes, int cantidadBienes,struct Recurso *catalogoServicios, int cantidadServicios);

#endif