#include <stdio.h>
#include <stdlib.h>
#include "Juego.h"
#include "Persona.h"
#include "Recurso.h"
#include "Comunas.h"
#include "Sociedad.h"
#include "Indices.h"
#include "Consumo.h"
#include "Produccion.h"
#include "Economia.h"
#include "Emergencias.h"

/* ---------- Utilidades ---------- */

/* Pide un numero entre minimo y maximo hasta que sea valido */
static int leerEntero(const char *mensaje, int minimo, int maximo) {
    char linea[100];
    int valor;

    while (1) {
        printf("%s (%d-%d): ", mensaje, minimo, maximo);

        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            printf("\nSe cerro la entrada, saliendo.\n");
            exit(0);
        }

        if (sscanf(linea, "%d", &valor) == 1 && valor >= minimo && valor <= maximo) {
            return valor;
        }

        printf("Valor invalido\n");
    }
}

static int contarPersonas(struct Persona *inicio) {
    int cantidad = 0;

    while (inicio != NULL) {
        cantidad++;
        inicio = inicio->siguiente;
    }

    return cantidad;
}

static int contarRecursos(struct Recurso *inicio) {
    int cantidad = 0;

    while (inicio != NULL) {
        cantidad++;
        inicio = inicio->siguiente;
    }

    return cantidad;
}

static int contarComunas(struct Comuna *inicio) {
    int cantidad = 0;

    while (inicio != NULL) {
        cantidad++;
        inicio = inicio->siguiente;
    }

    return cantidad;
}

/* ---------- Mostrar informacion ---------- */

static void mostrarEstado(struct Comuna *sociedad, int dia) {
    printf("\n===== Dia %d =====\n", dia);

    int numero = 1;
    float sumaSatisfaccion = 0;
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        printf("%2d. %-22s %3d personas  necesidad %5.1f  satisfaccion %5.1f\n",
               numero, comuna->nombre, comuna->cantidadPersonas,
               comuna->necesidad, comuna->satisfaccion);

        sumaSatisfaccion += comuna->satisfaccion;
        numero++;
        comuna = comuna->siguiente;
    }

    printf("Satisfaccion promedio de la sociedad: %.1f\n",
           sumaSatisfaccion / (numero - 1));
}

static void imprimirNumerados(struct Recurso *lista) {
    int numero = 1;

    while (lista != NULL) {
        printf("%2d. %s: %d/%d\n", numero, lista->nombre, lista->existencia, lista->maximo);
        numero++;
        lista = lista->siguiente;
    }
}

static struct Comuna *elegirComuna(struct Comuna *sociedad, const char *mensaje) {
    int numero = leerEntero(mensaje, 1, contarComunas(sociedad));
    struct Comuna *comuna = sociedad;

    for (int i = 1; i < numero; i++) {
        comuna = comuna->siguiente;
    }

    return comuna;
}

static struct Recurso *elegirRecurso(struct Recurso *lista) {
    int numero = leerEntero("Numero del recurso", 1, contarRecursos(lista));
    struct Recurso *recurso = lista;

    for (int i = 1; i < numero; i++) {
        recurso = recurso->siguiente;
    }

    return recurso;
}

static void verComuna(struct Comuna *sociedad) {
    struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna");

    printf("\n%s (%d personas)\n", comuna->nombre, comuna->cantidadPersonas);
    printf("Consumo: %d%%   Produccion: %d%%\n",
           comuna->porcentajeConsumo, comuna->porcentajeProduccion);

    printf("\nBienes:\n");
    imprimirNumerados(comuna->bienes);

    printf("\nServicios:\n");
    imprimirNumerados(comuna->servicios);
}

/* ---------- Intercambio ---------- */

static void hacerRegalo(struct Comuna *sociedad) {
    int tipo = leerEntero("1 = bien, 2 = servicio", 1, 2);

    struct Comuna *da = elegirComuna(sociedad, "Numero de la comuna que regala");
    struct Comuna *recibe = elegirComuna(sociedad, "Numero de la comuna que recibe");

    if (da == recibe) {
        printf("Deben ser comunas distintas\n");
        return;
    }

    struct Recurso *lista = (tipo == 1) ? da->bienes : da->servicios;

    printf("\nRecursos de %s:\n", da->nombre);
    imprimirNumerados(lista);

    struct Recurso *recurso = elegirRecurso(lista);

    if (recurso->existencia <= 0) {
        printf("%s no tiene nada de %s para regalar\n", da->nombre, recurso->nombre);
        return;
    }

    int cantidad = leerEntero("Cantidad a regalar", 1, recurso->existencia);
    int regalado = regalarRecurso(da, recibe, tipo == 1, recurso->nombre, cantidad);

    if (regalado == 0) {
        printf("No se pudo regalar: la comuna que regala debe conservar su reserva "
               "y la que recibe no puede pasar de su maximo\n");
        return;
    }

    printf("%s regalo %d de %s a %s\n", da->nombre, regalado, recurso->nombre, recibe->nombre);

    actualizarIndices(sociedad);
}

/* ---------- Menu del dia ---------- */

/* Devuelve 1 para seguir al siguiente dia y 0 para salir */
static int menuDia(struct Comuna *sociedad, int dia) {
    while (1) {
        printf("\n1. Hacer un regalo entre comunas\n");
        printf("2. Ver los recursos de una comuna\n");
        printf("3. Ver el estado de las comunas\n");
        printf("4. Pasar al siguiente dia\n");
        printf("0. Salir\n");

        int opcion = leerEntero("Opcion", 0, 4);

        if (opcion == 0) {
            return 0;
        }
        if (opcion == 1) {
            hacerRegalo(sociedad);
        }
        if (opcion == 2) {
            verComuna(sociedad);
        }
        if (opcion == 3) {
            mostrarEstado(sociedad, dia);
        }
        if (opcion == 4) {
            return 1;
        }
    }
}

/* ---------- Juego ---------- */

void iniciarJuego(void) {
    struct Persona *personas = cargarPersonas();
    struct Recurso *catalogoBienes = cargarRecursos("bienes");
    struct Recurso *catalogoServicios = cargarRecursos("servicios");
    struct Comuna *nombresComunas = cargarComunas();

    if (personas == NULL || catalogoBienes == NULL ||
        catalogoServicios == NULL || nombresComunas == NULL) {
        printf("Faltan datos, no se puede iniciar la simulacion\n");
        liberarPersonas(personas);
        liberarRecursos(catalogoBienes);
        liberarRecursos(catalogoServicios);
        liberarComunas(nombresComunas);
        return;
    }

    int totalBienes = contarRecursos(catalogoBienes);
    int totalServicios = contarRecursos(catalogoServicios);
    int totalComunas = contarComunas(nombresComunas);

    printf("Personas cargadas: %d\n", contarPersonas(personas));
    printf("Bienes disponibles: %d\n", totalBienes);
    printf("Servicios disponibles: %d\n", totalServicios);
    printf("Comunas disponibles: %d\n\n", totalComunas);

    int maximoComunas = totalComunas < 50 ? totalComunas : 50;

    int cantidadComunas = leerEntero("Cuantas comunas", 1, maximoComunas);
    int cantidadBienes = leerEntero("Cuantos bienes", 1, totalBienes);
    int cantidadServicios = leerEntero("Cuantos servicios", 1, totalServicios);

    struct Comuna *sociedad = crearSociedad(nombresComunas, cantidadComunas);

    if (sociedad != NULL) {
        asignarRecursos(sociedad, catalogoBienes, cantidadBienes,
                        catalogoServicios, cantidadServicios);
        asignarConsumo(sociedad);
        asignarProduccion(sociedad);
        iniciarEmergencias();
        actualizarIndices(sociedad);

        int dia = 1;
        int jugando = 1;

        while (jugando) {
            if (dia > 1) {
                consumirBienes(sociedad);
                producirRecursos(sociedad);
                avanzarEmergencias(sociedad);
                actualizarIndices(sociedad);
            }

            mostrarEstado(sociedad, dia);
            jugando = menuDia(sociedad, dia);
            dia++;
        }

        liberarComunas(sociedad);
    }

    liberarPersonas(personas);
    liberarRecursos(catalogoBienes);
    liberarRecursos(catalogoServicios);
    liberarComunas(nombresComunas);
}