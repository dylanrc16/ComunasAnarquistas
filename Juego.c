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

// ---------- Utilidades ----------

// Funcion para pedir un numero entre minimo y maximo hasta que sea valido
static int leerEntero(const char *mensaje, int minimo, int maximo) {
    char linea[100];
    int valor;

    while (1) {
        printf("%s (%d-%d): ", mensaje, minimo, maximo);

        // Si se cierra la entrada (Ctrl+Z / Ctrl+D), sale del programa
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            printf("\nSe cerro la entrada, saliendo.\n");
            exit(0);
        }

        // Acepta solo si es un numero dentro del rango
        if (sscanf(linea, "%d", &valor) == 1 && valor >= minimo && valor <= maximo) {
            return valor;
        }

        printf("Valor invalido\n");
    }
}

// Funcion para contar cuantas personas hay en la lista
static int contarPersonas(struct Persona *inicio) {
    int cantidad = 0;

    while (inicio != NULL) {
        cantidad++;
        inicio = inicio->siguiente;
    }

    return cantidad;
}

// Funcion para contar cuantos recursos hay en la lista
static int contarRecursos(struct Recurso *inicio) {
    int cantidad = 0;

    while (inicio != NULL) {
        cantidad++;
        inicio = inicio->siguiente;
    }

    return cantidad;
}

// Funcion para contar cuantas comunas hay en la lista
static int contarComunas(struct Comuna *inicio) {
    int cantidad = 0;

    while (inicio != NULL) {
        cantidad++;
        inicio = inicio->siguiente;
    }

    return cantidad;
}

// ---------- Mostrar informacion ----------

// Funcion para mostrar la necesidad y satisfaccion de cada comuna y el promedio
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

    // numero - 1 es la cantidad de comunas
    printf("Satisfaccion promedio de la sociedad: %.1f\n",
           sumaSatisfaccion / (numero - 1));
}

// Funcion para imprimir una lista de recursos numerada desde 1
static void imprimirNumerados(struct Recurso *lista) {
    int numero = 1;

    while (lista != NULL) {
        printf("%2d. %s: %d/%d\n", numero, lista->nombre, lista->existencia, lista->maximo);
        numero++;
        lista = lista->siguiente;
    }
}

// Funcion para que el usuario elija una comuna por su numero
static struct Comuna *elegirComuna(struct Comuna *sociedad, const char *mensaje) {
    int numero = leerEntero(mensaje, 1, contarComunas(sociedad));
    struct Comuna *comuna = sociedad;

    // Avanza hasta la comuna elegida
    for (int i = 1; i < numero; i++) {
        comuna = comuna->siguiente;
    }

    return comuna;
}

// Funcion para que el usuario elija un recurso por su numero
static struct Recurso *elegirRecurso(struct Recurso *lista) {
    int numero = leerEntero("Numero del recurso", 1, contarRecursos(lista));
    struct Recurso *recurso = lista;

    // Avanza hasta el recurso elegido
    for (int i = 1; i < numero; i++) {
        recurso = recurso->siguiente;
    }

    return recurso;
}

// Funcion para mostrar los datos, bienes y servicios de una comuna
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

// ---------- Intercambio ----------

// Funcion para pedir los datos de un regalo y hacerlo entre dos comunas
static void hacerRegalo(struct Comuna *sociedad) {
    int tipo = leerEntero("1 = bien, 2 = servicio", 1, 2);

    // Pide quien regala y quien recibe
    struct Comuna *da = elegirComuna(sociedad, "Numero de la comuna que regala");
    struct Comuna *recibe = elegirComuna(sociedad, "Numero de la comuna que recibe");

    // No se puede regalar a si misma
    if (da == recibe) {
        printf("Deben ser comunas distintas\n");
        return;
    }

    // Usa bienes o servicios segun lo elegido
    struct Recurso *lista = (tipo == 1) ? da->bienes : da->servicios;

    printf("\nRecursos de %s:\n", da->nombre);
    imprimirNumerados(lista);

    struct Recurso *recurso = elegirRecurso(lista);

    // Si no tiene nada de ese recurso, no hay regalo
    if (recurso->existencia <= 0) {
        printf("%s no tiene nada de %s para regalar\n", da->nombre, recurso->nombre);
        return;
    }

    int cantidad = leerEntero("Cantidad a regalar", 1, recurso->existencia);
    // Economia.c decide cuanto se puede regalar de verdad
    int regalado = regalarRecurso(da, recibe, tipo == 1, recurso->nombre, cantidad);

    if (regalado == 0) {
        printf("No se pudo regalar: la comuna que regala debe conservar su reserva "
               "y la que recibe no puede pasar de su maximo\n");
        return;
    }

    printf("%s regalo %d de %s a %s\n", da->nombre, regalado, recurso->nombre, recibe->nombre);

    // El regalo cambia necesidad y satisfaccion
    actualizarIndices(sociedad);
}

// ---------- Menu del dia ----------

// Funcion para mostrar el menu del dia. Devuelve 1 para seguir al siguiente dia y 0 para salir
static int menuDia(struct Comuna *sociedad, int dia) {
    // Repite el menu hasta pasar de dia o salir
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

// ---------- Juego ----------

// Funcion para iniciar el juego: carga datos, crea la sociedad y corre los dias
void iniciarJuego(void) {
    // Carga los cuatro archivos de datos
    struct Persona *personas = cargarPersonas();
    struct Recurso *catalogoBienes = cargarRecursos("bienes");
    struct Recurso *catalogoServicios = cargarRecursos("servicios");
    struct Comuna *nombresComunas = cargarComunas();

    // Si falta algun archivo, libera lo cargado y termina
    if (personas == NULL || catalogoBienes == NULL ||
        catalogoServicios == NULL || nombresComunas == NULL) {
        printf("Faltan datos, no se puede iniciar la simulacion\n");
        liberarPersonas(personas);
        liberarRecursos(catalogoBienes);
        liberarRecursos(catalogoServicios);
        liberarComunas(nombresComunas);
        return;
    }

    // Cuenta lo que se cargo para mostrarlo y validar
    int totalBienes = contarRecursos(catalogoBienes);
    int totalServicios = contarRecursos(catalogoServicios);
    int totalComunas = contarComunas(nombresComunas);

    printf("Personas cargadas: %d\n", contarPersonas(personas));
    printf("Bienes disponibles: %d\n", totalBienes);
    printf("Servicios disponibles: %d\n", totalServicios);
    printf("Comunas disponibles: %d\n\n", totalComunas);

    // No mas de 50 comunas ni mas que los nombres disponibles
    int maximoComunas = totalComunas < 50 ? totalComunas : 50;

    // Pide la configuracion de la partida
    int cantidadComunas = leerEntero("Cuantas comunas", 1, maximoComunas);
    int cantidadBienes = leerEntero("Cuantos bienes", 1, totalBienes);
    int cantidadServicios = leerEntero("Cuantos servicios", 1, totalServicios);

    // Crea las comunas con sus habitantes
    struct Comuna *sociedad = crearSociedad(nombresComunas, cantidadComunas);

    if (sociedad != NULL) {
        // Prepara recursos, porcentajes, emergencias e indices iniciales
        asignarRecursos(sociedad, catalogoBienes, cantidadBienes, catalogoServicios, cantidadServicios);
        asignarConsumo(sociedad);
        asignarProduccion(sociedad);
        iniciarEmergencias();
        actualizarIndices(sociedad);

        // Bucle principal: un ciclo por dia
        int dia = 1;
        int jugando = 1;

        while (jugando) {
            // El dia 1 solo muestra el estado inicial; desde el 2 se simula
            if (dia > 1) {
                consumirBienes(sociedad);
                producirRecursos(sociedad);
                avanzarEmergencias(sociedad);
                actualizarIndices(sociedad);
            }

            // Muestra el estado y abre el menu del dia
            mostrarEstado(sociedad, dia);
            jugando = menuDia(sociedad, dia);
            dia++;
        }

        // Al salir, libera la sociedad
        liberarComunas(sociedad);
    }

    // Libera los datos cargados de los archivos
    liberarPersonas(personas);
    liberarRecursos(catalogoBienes);
    liberarRecursos(catalogoServicios);
    liberarComunas(nombresComunas);
}