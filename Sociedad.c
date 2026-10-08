#include <stdio.h>
#include "Sociedad.h"

// Funcion para crear la sociedad: reparte 500 personas entre las comunas elegidas
struct Comuna *crearSociedad(struct Comuna *nombresComunas, int cantidadComunas) {
    // Solo se permiten de 1 a 50 comunas
    if (cantidadComunas < 1 || cantidadComunas > 50) {
        printf("La cantidad de comunas debe estar entre 1 y 50\n");
        return NULL;
    }

    // Reparto parejo y lo que sobra de la division
    int personasPorComuna = 500 / cantidadComunas;
    int personasSobrantes = 500 % cantidadComunas;

    struct Comuna *sociedad = NULL;
    struct Comuna *actual = nombresComunas;

    // Crea una comuna por cada nombre que se va a usar
    for (int i = 0; i < cantidadComunas; i++) {
        // Si se acaban los nombres, se cancela todo
        if (actual == NULL) {
            printf("No hay suficientes nombres en el archivo comunas\n");
            liberarComunas(sociedad);
            return NULL;
        }

        int habitantes = personasPorComuna;

        // Las primeras comunas reciben una persona extra para llegar a 500
        if (i < personasSobrantes) {
            habitantes++;
        }

        // Agrega la comuna con su nombre y habitantes
        sociedad = agregarComunaFinal(sociedad, actual->nombre, habitantes);
        actual = actual->siguiente;
    }

    return sociedad;
}

// Funcion para dar a cada comuna sus bienes y servicios con existencia inicial
void asignarRecursos(struct Comuna *sociedad,struct Recurso *catalogoBienes, int cantidadBienes,struct Recurso *catalogoServicios, int cantidadServicios) {
    struct Comuna *comuna = sociedad;
    int numeroComuna = 0;

    while (comuna != NULL) {
        // Cada bien puede llegar a 2 por persona
        int maximoBien = comuna->cantidadPersonas * 2;
        struct Recurso *bien = catalogoBienes;

        // Recorre los primeros cantidadBienes del catalogo
        for (int i = 0; i < cantidadBienes && bien != NULL; i++) {
            // Empieza entre 40% y 80% del maximo, variando por comuna y recurso
            int porcentaje = 40 + 10 * ((numeroComuna + i) % 5);
            int existencia = maximoBien * porcentaje / 100;

            comuna->bienes = agregarRecursoFinal(comuna->bienes, bien->nombre, existencia, maximoBien);
            bien = bien->siguiente;
        }

        // Cada servicio puede llegar a 1 por cada 10 personas (redondeado hacia arriba)
        int maximoServicio = (comuna->cantidadPersonas + 9) / 10;
        struct Recurso *servicio = catalogoServicios;

        // Recorre los primeros cantidadServicios del catalogo
        for (int i = 0; i < cantidadServicios && servicio != NULL; i++) {
            // Misma regla de 40% a 80% que los bienes
            int porcentaje = 40 + 10 * ((numeroComuna + i) % 5);
            int existencia = maximoServicio * porcentaje / 100;

            comuna->servicios = agregarRecursoFinal(comuna->servicios, servicio->nombre, existencia, maximoServicio);
            servicio = servicio->siguiente;
        }

        numeroComuna++;
        comuna = comuna->siguiente;
    }
}