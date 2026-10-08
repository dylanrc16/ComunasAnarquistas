// Evita que este archivo se incluya dos veces
#ifndef COMUNAS_H
#define COMUNAS_H

#include "Recurso.h"

// Nodo de la lista doblemente enlazada de comunas
struct Comuna
{
    char nombre[50];  // Nombre de la comuna
    int cantidadPersonas;  // Habitantes de la comuna
    int porcentajeConsumo;  // Porcentaje de habitantes que gasta cada bien por dia
    int porcentajeProduccion;  // Porcentaje de habitantes que produce cada bien por dia

    float necesidad;  // Que tanto le falta (5 a 95)
    float satisfaccion;  // Que tan bien esta (0 a 100)
    float penalizacion;  // Castigo por emergencias cercanas
    float solidaridad;  // Reconocimiento por haber regalado

    struct Recurso *bienes;  // Lista de bienes de la comuna
    struct Recurso *servicios;  // Lista de servicios de la comuna

    struct Comuna *anterior;  // Comuna anterior en la lista
    struct Comuna *siguiente;  // Comuna siguiente en la lista
};

// Funcion para crear una comuna nueva
struct Comuna *crearComuna(const char *nombre, int cantidadPersonas);
// Funcion para agregar al final
struct Comuna *agregarComunaFinal(struct Comuna *inicio, const char *nombre, int cantidadPersonas);
// Funcion para buscar una comuna por nombre
struct Comuna *buscarComuna(struct Comuna *inicio, const char *nombre);
// Funcion para imprimir todas las comunas
void imprimirComunas(struct Comuna *inicio);
// Funcion para liberar la lista de comunas
void liberarComunas(struct Comuna *inicio);
// Funcion para cargar los nombres de comunas del archivo
struct Comuna *cargarComunas(void);

// Fin de la proteccion contra inclusion doble
#endif