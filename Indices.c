#include <stdio.h>
#include "Indices.h"

// Limites de la necesidad
#define NECESIDAD_MINIMA 5.0f
#define NECESIDAD_MAXIMA 95.0f

// Regalos (dados + recibidos) a partir de los cuales la reciprocidad cuenta completa
#define REGALOS_PARA_RECIPROCIDAD 6

// Funcion para sumar lo que falta para llegar al maximo y el maximo total de una lista
static void sumarRecursos(struct Recurso *lista, int *faltante, int *total) {
    struct Recurso *actual = lista;

    while (actual != NULL) {
        *faltante += actual->maximo - actual->existencia;
        *total += actual->maximo;
        actual = actual->siguiente;
    }
}

// Funcion para dejar un valor dentro de un rango [minimo, maximo]
static float limitar(float valor, float minimo, float maximo) {
    if (valor < minimo) {
        return minimo;
    }
    if (valor > maximo) {
        return maximo;
    }
    return valor;
}

// Funcion para calcular la necesidad = 100 * (lo que falta) / (lo maximo), entre 5 y 95
float calcularNecesidad(struct Comuna *comuna) {
    int faltante = 0;
    int total = 0;

    // Suma bienes y servicios juntos
    sumarRecursos(comuna->bienes, &faltante, &total);
    sumarRecursos(comuna->servicios, &faltante, &total);

    // Sin recursos, la necesidad es la maxima
    if (total == 0) {
        return NECESIDAD_MAXIMA;
    }

    float necesidad = 100.0f * faltante / total;
    return limitar(necesidad, NECESIDAD_MINIMA, NECESIDAD_MAXIMA);
}

// Funcion para promediar el bienestar (100 - necesidad) de las 2 comunas anteriores
// y las 2 siguientes. Si no tiene vecinas devuelve su propio bienestar.
static float bienestarVecinas(struct Comuna *comuna) {
    float suma = 0;
    int cantidad = 0;

    // Hacia atras: hasta 2 vecinas
    struct Comuna *vecina = comuna->anterior;
    for (int i = 0; i < 2 && vecina != NULL; i++) {
        suma += 100.0f - vecina->necesidad;
        cantidad++;
        vecina = vecina->anterior;
    }

    // Hacia adelante: hasta 2 vecinas
    vecina = comuna->siguiente;
    for (int i = 0; i < 2 && vecina != NULL; i++) {
        suma += 100.0f - vecina->necesidad;
        cantidad++;
        vecina = vecina->siguiente;
    }

    // Comuna sola: usa su propio bienestar
    if (cantidad == 0) {
        return 100.0f - comuna->necesidad;
    }

    return suma / cantidad;
}

// Funcion para promediar la ultima satisfaccion calculada de las 2 comunas anteriores
// y las 2 siguientes. Se usa la anterior para evitar el circulo: la satisfaccion
// de A depende de la de B y la de B depende de la de A.
static float satisfaccionVecinas(struct Comuna *comuna) {
    float suma = 0;
    int cantidad = 0;

    // Hacia atras: hasta 2 vecinas
    struct Comuna *vecina = comuna->anterior;
    for (int i = 0; i < 2 && vecina != NULL; i++) {
        suma += vecina->satisfaccionAnterior;
        cantidad++;
        vecina = vecina->anterior;
    }

    // Hacia adelante: hasta 2 vecinas
    vecina = comuna->siguiente;
    for (int i = 0; i < 2 && vecina != NULL; i++) {
        suma += vecina->satisfaccionAnterior;
        cantidad++;
        vecina = vecina->siguiente;
    }

    // Comuna sola: usa su propia satisfaccion anterior
    if (cantidad == 0) {
        return comuna->satisfaccionAnterior;
    }

    return suma / cantidad;
}

// Funcion para calcular la reciprocidad (0 a 100). Parte de 50 (neutral) y se mueve
// de a poco segun cuanto ha intercambiado la comuna:
//   balance       = 1 - |dados - recibidos| / (dados + recibidos)   (1 equilibrado, 0 de un solo lado)
//   participacion = (dados + recibidos) / REGALOS_PARA_RECIPROCIDAD, con tope de 1
//   reciprocidad  = 50 + 50 * participacion * (2 * balance - 1)
// Un regalo aislado casi no la mueve; lo que cuenta es el patron a lo largo del tiempo.
float calcularReciprocidad(struct Comuna *comuna) {
    int dados = comuna->regalosDados;
    int recibidos = comuna->regalosRecibidos;
    int total = dados + recibidos;

    // Sin intercambios todavia: neutral, para no castigar a nadie al inicio
    if (total == 0) {
        return 50.0f;
    }

    int diferencia = dados - recibidos;
    if (diferencia < 0) {
        diferencia = -diferencia;
    }

    float balance = 1.0f - (float) diferencia / total;
    float participacion = (float) total / REGALOS_PARA_RECIPROCIDAD;

    if (participacion > 1.0f) {
        participacion = 1.0f;
    }

    return 50.0f + 50.0f * participacion * (2.0f * balance - 1.0f);
}

// Funcion para calcular la satisfaccion:
//   0.4 * bienestar propio (100 - necesidad local)
// + 0.2 * bienestar de las vecinas (100 - necesidad de las cercanas)
// + 0.2 * satisfaccion de las vecinas (la ultima calculada)
// + 0.2 * reciprocidad
// + apoyo recibido + solidaridad - penalizacion
float calcularSatisfaccion(struct Comuna *comuna) {
    float propia = 100.0f - comuna->necesidad;
    float vecinas = bienestarVecinas(comuna);
    float satisfaccionCercanas = satisfaccionVecinas(comuna);

    float satisfaccion = 0.4f * propia + 0.2f * vecinas + 0.2f * satisfaccionCercanas
                         + 0.2f * comuna->reciprocidad
                         + comuna->apoyoRecibido + comuna->solidaridad - comuna->penalizacion;

    return limitar(satisfaccion, 0.0f, 100.0f);
}

// Funcion para restar una cantidad sin bajar de 0
static float bajar(float valor, float cantidad) {
    valor -= cantidad;
    if (valor < 0) {
        valor = 0;
    }
    return valor;
}

// Funcion para que los efectos de emergencias y regalos se vayan apagando.
// Se llama UNA vez por dia (no despues de cada regalo): baja 2 puntos
// la penalizacion, la solidaridad y el apoyo recibido de cada comuna, sin bajar de 0.
void decaerEfectos(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        comuna->penalizacion = bajar(comuna->penalizacion, 2.0f);
        comuna->solidaridad = bajar(comuna->solidaridad, 2.0f);
        comuna->apoyoRecibido = bajar(comuna->apoyoRecibido, 2.0f);
        comuna = comuna->siguiente;
    }
}

// Funcion para actualizar los indices. Se llama cada dia y despues de cada regalo.
// 1. Necesidades y reciprocidades de todas las comunas.
// 2. Copia de las satisfacciones actuales (las vecinas usan esta copia).
// 3. Satisfacciones nuevas.
// Asi el orden de las comunas en la lista no cambia el resultado.
void actualizarIndices(struct Comuna *sociedad) {
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        // Primera pasada: necesidad y reciprocidad
        comuna->necesidad = calcularNecesidad(comuna);
        comuna->reciprocidad = calcularReciprocidad(comuna);
        comuna = comuna->siguiente;
    }

    comuna = sociedad;
    while (comuna != NULL) {
        // Segunda pasada: guarda la satisfaccion actual antes de cambiarla
        comuna->satisfaccionAnterior = comuna->satisfaccion;
        comuna = comuna->siguiente;
    }

    comuna = sociedad;
    while (comuna != NULL) {
        // Tercera pasada: satisfacciones nuevas
        comuna->satisfaccion = calcularSatisfaccion(comuna);
        comuna = comuna->siguiente;
    }
}
