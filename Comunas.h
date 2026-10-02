#ifndef COMUNAS_H
#define COMUNAS_H

#include "Recurso.h"

struct Comuna
{
    char nombre[50];
    int cantidadPersonas;
    int porcentajeConsumo;
    int porcentajeProduccion;

    float necesidad;
    float satisfaccion;
    float penalizacion;
    float solidaridad;

    struct Recurso *bienes;
    struct Recurso *servicios;

    struct Comuna *anterior;
    struct Comuna *siguiente;
};

struct Comuna *crearComuna(const char *nombre, int cantidadPersonas);
struct Comuna *agregarComunaFinal(struct Comuna *inicio, const char *nombre, int cantidadPersonas);
struct Comuna *buscarComuna(struct Comuna *inicio, const char *nombre);
void imprimirComunas(struct Comuna *inicio);
void liberarComunas(struct Comuna *inicio);
struct Comuna *cargarComunas(void);

#endif