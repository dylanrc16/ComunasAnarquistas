// Evita que este archivo se incluya dos veces
#ifndef SOCIEDAD_H
#define SOCIEDAD_H

#include "Comunas.h"
#include "Recurso.h"
#include "TablaPersonas.h"

// Funcion para crear la sociedad con N comunas y 500 personas
struct Comuna *crearSociedad(struct Comuna *nombresComunas, int cantidadComunas);

// Funcion para dar a cada comuna sus bienes y servicios iniciales
void asignarRecursos(struct Comuna *sociedad, struct Recurso *catalogoBienes, int cantidadBienes,struct Recurso *catalogoServicios, int cantidadServicios);

// Funcion para repartir las personas de la tabla entre las comunas,
// guardando en cada persona un puntero a su comuna
void asignarPersonas(struct Comuna *sociedad, struct TablaPersonas *tabla);

// Funcion para darle a cada persona servicios al azar:
// 30% ninguno, 50% uno y 20% dos (de los primeros cantidadServicios del catalogo)
void asignarServiciosPersonas(struct TablaPersonas *tabla, struct Recurso *catalogoServicios, int cantidadServicios);

// Funcion para recalcular, en cada comuna, cuantas personas tiene y cuantas
// saben cada servicio. Se llama al inicio y cada vez que alguien se muda o aprende.
void recalcularServicios(struct Comuna *sociedad, const struct TablaPersonas *tabla);

// Fin de la proteccion contra inclusion doble
#endif