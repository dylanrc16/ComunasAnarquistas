#include <stdio.h>
#include "Indices.h"

#define NECESIDAD_MINIMA 5.0f
#define NECESIDAD_MAXIMA 95.0f

/* Suma lo que falta para llegar al maximo y el maximo total de una lista */
static void sumarRecursos(struct Recurso *lista, int *faltante, int *total) {
    struct Recurso *actual = lista;

    while (actual != NULL) {
        *faltante += actual->maximo - actual->existencia;
        *total += actual->maximo;
        actual = actual->siguiente;
    }
}

static float limitar(float valor, float minimo, float maximo) {
    if (valor < minimo) {
        return minimo;
    }
    if (valor > maximo) {
        return maximo;
    }
    return valor;
}

/* necesidad = 100 * (lo que falta) / (lo maximo), entre 5 y 95 */
float calcularNecesidad(struct Comuna *comuna) {
    int faltante = 0;
    int total = 0;

    sumarRecursos(comuna->bienes, &faltante, &total);
    sumarRecursos(comuna->servicios, &faltante, &total);

    if (total == 0) {
        return NECESIDAD_MAXIMA;
    }

    float necesidad = 100.0f * faltante / total;
    return limitar(necesidad, NECESIDAD_MINIMA, NECESIDAD_MAXIMA);
}

/* Promedio de bienestar (100 - necesidad) de las 2 comunas anteriores
   y las 2 siguientes. Si no tiene vecinas devuelve su propio bienestar. */
static float bienestarVecinas(struct Comuna *comuna) {
    float suma = 0;
    int cantidad = 0;

    struct Comuna *vecina = comuna->anterior;
    for (int i = 0; i < 2 && vecina != NULL; i++) {
        suma += 100.0f - vecina->necesidad;
        cantidad++;
        vecina = vecina->anterior;
    }

    vecina = comuna->siguiente;
    for (int i = 0; i < 2 && vecina != NULL; i++) {
        suma += 100.0f - vecina->necesidad;
        cantidad++;
        vecina = vecina->siguiente;
    }

    if (cantidad == 0) {
        return 100.0f - comuna->necesidad;
    }

    return suma / cantidad;
}

/* satisfaccion = 0.6 * propia + 0.4 * vecinas + solidaridad - penalizacion */
float calcularSatisfaccion(struct Comuna *comuna) {
    float propia = 100.0f - comuna->necesidad;
    float vecinas = bienestarVecinas(comuna);

    float satisfaccion = 0.6f * propia + 0.4f * vecinas
                         + comuna->solidaridad - comuna->penalizacion;

    return limitar(satisfaccion, 0.0f, 100.0f);
}

static float bajar(float valor, float cantidad) {
    valor -= cantidad;
    if (valor < 0) {
        valor = 0;
    }
    return valor;
}

/* Se llama una vez por turno. Primero todas las necesidades (porque la
   satisfaccion de una comuna usa la necesidad de sus vecinas) y luego
   las satisfacciones. */
void actualizarIndices(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        comuna->necesidad = calcularNecesidad(comuna);
        comuna = comuna->siguiente;
    }

    comuna = sociedad;
    while (comuna != NULL) {
        comuna->satisfaccion = calcularSatisfaccion(comuna);

        /* los efectos de emergencias y regalos se van apagando */
        comuna->penalizacion = bajar(comuna->penalizacion, 2.0f);
        comuna->solidaridad = bajar(comuna->solidaridad, 2.0f);

        comuna = comuna->siguiente;
    }
}