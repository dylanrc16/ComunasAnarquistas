#include <stdio.h>
#include "Sociedad.h"

struct Comuna *crearSociedad(struct Comuna *nombresComunas, int cantidadComunas) {
    if (cantidadComunas < 1 || cantidadComunas > 50) {
        printf("La cantidad de comunas debe estar entre 1 y 50\n");
        return NULL;
    }

    int personasPorComuna = 500 / cantidadComunas;
    int personasSobrantes = 500 % cantidadComunas;

    struct Comuna *sociedad = NULL;
    struct Comuna *actual = nombresComunas;

    for (int i = 0; i < cantidadComunas; i++) {
        if (actual == NULL) {
            printf("No hay suficientes nombres en el archivo comunas\n");
            liberarComunas(sociedad);
            return NULL;
        }

        int habitantes = personasPorComuna;

        if (i < personasSobrantes) {
            habitantes++;
        }

        sociedad = agregarComunaFinal(sociedad, actual->nombre, habitantes);
        actual = actual->siguiente;
    }

    return sociedad;
}

void asignarRecursos(struct Comuna *sociedad,struct Recurso *catalogoBienes, int cantidadBienes,struct Recurso *catalogoServicios, int cantidadServicios) {
    struct Comuna *comuna = sociedad;
    int numeroComuna = 0;

    while (comuna != NULL) {
        int maximoBien = comuna->cantidadPersonas * 2;
        struct Recurso *bien = catalogoBienes;

        for (int i = 0; i < cantidadBienes && bien != NULL; i++) {
            int porcentaje = 40 + 10 * ((numeroComuna + i) % 5);
            int existencia = maximoBien * porcentaje / 100;

            comuna->bienes = agregarRecursoFinal(comuna->bienes, bien->nombre, existencia, maximoBien);
            bien = bien->siguiente;
        }

        int maximoServicio = (comuna->cantidadPersonas + 9) / 10;
        struct Recurso *servicio = catalogoServicios;

        for (int i = 0; i < cantidadServicios && servicio != NULL; i++) {
            int porcentaje = 40 + 10 * ((numeroComuna + i) % 5);
            int existencia = maximoServicio * porcentaje / 100;

            comuna->servicios = agregarRecursoFinal(comuna->servicios, servicio->nombre, existencia, maximoServicio);
            servicio = servicio->siguiente;
        }

        numeroComuna++;
        comuna = comuna->siguiente;
    }
}