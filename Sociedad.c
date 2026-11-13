#include <stdio.h>
#include <stdlib.h>
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

// ---------- Personas ----------

// Funcion para repartir las personas: se recorre la tabla casilla por casilla
// y cada persona va a la siguiente comuna, en ronda (1, 2, 3, ..., 1, 2, 3...).
// Asi las primeras comunas reciben una persona extra cuando no alcanza parejo,
// igual que en crearSociedad.
void asignarPersonas(struct Comuna *sociedad, struct TablaPersonas *tabla) {
    struct Comuna *comuna = sociedad;

    for (int i = 0; i < TAMANO_TABLA && sociedad != NULL; i++) {
        struct Persona *persona = tabla->casillas[i];

        while (persona != NULL) {
            persona->comuna = comuna;

            // Pasa a la siguiente comuna; despues de la ultima vuelve a la primera
            comuna = comuna->siguiente;
            if (comuna == NULL) {
                comuna = sociedad;
            }

            persona = persona->siguiente;
        }
    }
}

// Funcion para obtener el servicio numero "posicion" del catalogo (0 es el primero)
static struct Recurso *servicioEnPosicion(struct Recurso *catalogo, int posicion) {
    struct Recurso *actual = catalogo;

    for (int i = 0; i < posicion && actual != NULL; i++) {
        actual = actual->siguiente;
    }

    return actual;
}

// Funcion para darle servicios al azar a cada persona
void asignarServiciosPersonas(struct TablaPersonas *tabla, struct Recurso *catalogoServicios, int cantidadServicios) {
    if (cantidadServicios <= 0) {
        return;
    }

    for (int i = 0; i < TAMANO_TABLA; i++) {
        struct Persona *persona = tabla->casillas[i];

        while (persona != NULL) {
            // Numero al azar de 0 a 99 para decidir cuantos servicios sabe
            int suerte = rand() % 100;
            int cuantos = 0;

            if (suerte >= 30 && suerte < 80) {
                cuantos = 1;
            } else if (suerte >= 80) {
                cuantos = 2;
            }

            // aprenderServicio no repite: si sale el mismo dos veces, sabe solo uno
            for (int j = 0; j < cuantos; j++) {
                struct Recurso *servicio = servicioEnPosicion(catalogoServicios, rand() % cantidadServicios);

                if (servicio != NULL) {
                    aprenderServicio(persona, servicio->nombre);
                }
            }

            persona = persona->siguiente;
        }
    }
}

// Funcion para recalcular personas y servicios de cada comuna a partir de la tabla
void recalcularServicios(struct Comuna *sociedad, const struct TablaPersonas *tabla) {
    // 1. Pone en 0 los habitantes y la existencia de cada servicio
    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        comuna->cantidadPersonas = 0;

        for (struct Recurso *servicio = comuna->servicios; servicio != NULL; servicio = servicio->siguiente) {
            servicio->existencia = 0;
        }
    }

    // 2. Recorre a todas las personas: cuenta a cada una en su comuna
    //    y suma 1 a cada servicio que sabe hacer
    for (int i = 0; i < TAMANO_TABLA; i++) {
        for (struct Persona *persona = tabla->casillas[i]; persona != NULL; persona = persona->siguiente) {
            struct Comuna *comuna = persona->comuna;

            if (comuna == NULL) {
                continue;
            }

            comuna->cantidadPersonas++;

            for (struct Servicio *sabe = persona->servicios; sabe != NULL; sabe = sabe->siguiente) {
                struct Recurso *servicio = buscarRecurso(comuna->servicios, sabe->nombre);

                if (servicio != NULL) {
                    servicio->existencia++;
                }
            }
        }
    }

    // 3. El maximo sigue siendo 1 por cada 10 personas (lo que la comuna necesita).
    //    Si hay mas personas que lo saben, se cuenta solo hasta el maximo:
    //    tener 3 doctoras no compensa que falte transporte.
    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        int maximo = (comuna->cantidadPersonas + 9) / 10;

        for (struct Recurso *servicio = comuna->servicios; servicio != NULL; servicio = servicio->siguiente) {
            servicio->maximo = maximo;

            if (servicio->existencia > maximo) {
                servicio->existencia = maximo;
            }
        }
    }
}
