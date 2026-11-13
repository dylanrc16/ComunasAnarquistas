#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Juego.h"
#include "Persona.h"
#include "TablaPersonas.h"
#include "Recurso.h"
#include "Comunas.h"
#include "Sociedad.h"
#include "Indices.h"
#include "Consumo.h"
#include "Produccion.h"
#include "Economia.h"
#include "Emergencias.h"
#include "Biblioteca.h"
#include "Jugador.h"
#include "Celular.h"

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

// Funcion para pedir un texto (una linea). Quita el salto de linea del final.
static void leerTexto(const char *mensaje, char *destino, int tamano) {
    printf("%s: ", mensaje);

    // Si se cierra la entrada (Ctrl+Z / Ctrl+D), sale del programa
    if (fgets(destino, tamano, stdin) == NULL) {
        printf("\nSe cerro la entrada, saliendo.\n");
        exit(0);
    }

    destino[strcspn(destino, "\r\n")] = '\0';
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
// Las comunas del jugador se marcan con *.
static void mostrarEstado(struct Comuna *sociedad, int dia, const struct Jugador *jugador) {
    printf("\n===== Dia %d =====   Vidas: %d de %d   (* = sus comunas)\n",
           dia, VIDAS - jugador->vidasPerdidas, VIDAS);

    int numero = 1;
    float sumaSatisfaccion = 0;
    struct Comuna *comuna = sociedad;

    while (comuna != NULL) {
        printf("%2d.%s%-22s %3d personas  necesidad %5.1f  satisfaccion %5.1f  reciprocidad %5.1f\n",
               numero, estaAdministrada(jugador, comuna) ? "*" : " ", comuna->nombre, comuna->cantidadPersonas,
               comuna->necesidad, comuna->satisfaccion, comuna->reciprocidad);

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
    printf("Consumo: %.2f%%   Produccion: %.2f%%\n",
           comuna->porcentajeConsumo, comuna->porcentajeProduccion);
    printf("Regalos dados: %d   Regalos recibidos: %d   Reciprocidad: %.1f\n",
           comuna->regalosDados, comuna->regalosRecibidos, comuna->reciprocidad);
    printf("Solidaridad: %.1f   Apoyo recibido: %.1f   Penalizacion: %.1f\n",
           comuna->solidaridad, comuna->apoyoRecibido, comuna->penalizacion);

    printf("\nBienes:\n");
    imprimirNumerados(comuna->bienes);

    printf("\nServicios:\n");
    imprimirNumerados(comuna->servicios);
}

// ---------- Intercambio ----------

// Funcion para regalar un servicio: una persona que lo sabe se muda a la otra comuna
static void regalarServicioMenu(struct Comuna *sociedad, struct TablaPersonas *tabla, struct Comuna *da, struct Comuna *recibe) {
    printf("\nServicios de %s (personas que lo saben / lo que necesita):\n", da->nombre);
    imprimirNumerados(da->servicios);

    struct Recurso *servicio = elegirRecurso(da->servicios);
    struct Persona *persona = regalarServicio(tabla, da, recibe, servicio->nombre);

    if (persona == NULL) {
        printf("No se pudo: en %s tiene que quedar al menos otra persona que sepa %s\n",
               da->nombre, servicio->nombre);
        return;
    }

    printf("%s se mudo de %s a %s y lleva sus servicios (%s y %d mas)\n",
           persona->nombre, da->nombre, recibe->nombre, servicio->nombre, contarServicios(persona) - 1);

    // Cambiaron las personas de las dos comunas: se recalculan servicios e indices
    recalcularServicios(sociedad, tabla);
    actualizarIndices(sociedad);
}

// Funcion para pedir los datos de un regalo y hacerlo entre dos comunas.
// Un bien se regala por cantidad; un servicio se regala mudando a una persona.
static void hacerRegalo(struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Jugador *jugador) {
    int tipo = leerEntero("1 = bien, 2 = servicio", 1, 2);

    // Pide quien regala y quien recibe
    struct Comuna *da = elegirComuna(sociedad, "Numero de la comuna que regala");

    // El jugador solo decide por sus comunas; las demas regalan solas
    if (!estaAdministrada(jugador, da)) {
        printf("Solo puede regalar desde sus comunas (las marcadas con *)\n");
        return;
    }

    struct Comuna *recibe = elegirComuna(sociedad, "Numero de la comuna que recibe");

    // No se puede regalar a si misma
    if (da == recibe) {
        printf("Deben ser comunas distintas\n");
        return;
    }

    if (tipo == 2) {
        regalarServicioMenu(sociedad, tabla, da, recibe);
        return;
    }

    printf("\nBienes de %s:\n", da->nombre);
    imprimirNumerados(da->bienes);

    struct Recurso *recurso = elegirRecurso(da->bienes);

    // Si no tiene nada de ese bien, no hay regalo
    if (recurso->existencia <= 0) {
        printf("%s no tiene nada de %s para regalar\n", da->nombre, recurso->nombre);
        return;
    }

    int cantidad = leerEntero("Cantidad a regalar", 1, recurso->existencia);
    // Economia.c decide cuanto se puede regalar de verdad
    int regalado = regalarRecurso(da, recibe, 1, recurso->nombre, cantidad);

    if (regalado == 0) {
        printf("No se pudo regalar: la comuna que regala debe conservar su reserva "
               "y la que recibe no puede pasar de su maximo\n");
        return;
    }

    printf("%s regalo %d de %s a %s\n", da->nombre, regalado, recurso->nombre, recibe->nombre);

    // El regalo cambia necesidad y satisfaccion
    actualizarIndices(sociedad);
}

// ---------- Personas ----------

// Funcion para imprimir una persona: nombre y servicios que sabe
static void imprimirPersona(const struct Persona *persona) {
    printf("  %-24s ", persona->nombre);

    if (persona->servicios == NULL) {
        printf("(no sabe ningun servicio)");
    }

    for (struct Servicio *servicio = persona->servicios; servicio != NULL; servicio = servicio->siguiente) {
        printf("%s%s", servicio->nombre, servicio->siguiente != NULL ? ", " : "");
    }

    // Si esta estudiando algo, muestra cuanto le falta
    if (persona->servicioEnEstudio[0] != '\0') {
        printf("  [estudia %s: %d/%d libros]", persona->servicioEnEstudio,
               persona->librosParaServicio, LIBROS_PARA_APRENDER);
    }

    printf("\n");
}

// Funcion para ver las personas de una comuna: recorre la tabla filtrando por comuna
static void verPersonasComuna(struct Comuna *sociedad, const struct TablaPersonas *tabla) {
    struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna");
    printf("\nPersonas de %s (%d):\n", comuna->nombre, comuna->cantidadPersonas);

    for (int i = 0; i < TAMANO_TABLA; i++) {
        for (struct Persona *persona = tabla->casillas[i]; persona != NULL; persona = persona->siguiente) {
            if (persona->comuna == comuna) {
                imprimirPersona(persona);
            }
        }
    }
}

// Funcion para buscar una persona por nombre en la tabla hash
static void buscarPersonaMenu(const struct TablaPersonas *tabla) {
    char nombre[50];
    leerTexto("Nombre de la persona (ej. Ana Sanchez 2)", nombre, sizeof(nombre));

    struct Persona *persona = buscarPersonaTabla(tabla, nombre);

    if (persona == NULL) {
        printf("No hay ninguna persona llamada \"%s\"\n", nombre);
        return;
    }

    printf("Esta en la casilla %d de la tabla hash y vive en %s\n",
           calcularCasilla(persona->nombre), persona->comuna != NULL ? persona->comuna->nombre : "(ninguna comuna)");
    imprimirPersona(persona);
}

// ---------- Biblioteca ----------

// Maximo de libros que se muestran en pantalla por busqueda
#define LIBROS_A_MOSTRAR 20

// Funcion para pedir la carpeta del corpus y cargar el catalogo (AVL).
// Si presiona Enter usa la carpeta "corpus"; repite hasta que haya libros.
static void cargarBiblioteca(struct Catalogo *catalogo) {
    char carpeta[512];

    while (1) {
        leerTexto("Carpeta de la biblioteca (Enter para usar \"" CARPETA_CORPUS_POR_DEFECTO "\")", carpeta, sizeof(carpeta));

        if (carpeta[0] == '\0') {
            snprintf(carpeta, sizeof(carpeta), "%s", CARPETA_CORPUS_POR_DEFECTO);
        }

        int cantidad = cargarCatalogo(catalogo, carpeta);

        if (cantidad > 0) {
            printf("Libros cargados en el catalogo (AVL): %d\n\n", cantidad);
            return;
        }

        printf("No se encontraron libros .txt en \"%s\"\n", carpeta);
    }
}

// Funcion para buscar un libro por nombre exacto en el AVL
static void buscarLibroPorNombre(const struct Catalogo *catalogo) {
    char nombre[256];
    leerTexto("Nombre exacto del libro (ej. el_faro.txt)", nombre, sizeof(nombre));

    if (existeLibro(catalogo, nombre)) {
        printf("El libro \"%s\" si esta en la biblioteca\n", nombre);
    } else {
        printf("El libro \"%s\" no esta en la biblioteca\n", nombre);
    }
}

// Funcion para buscar un texto en los libros. Muestra las rotaciones del splay,
// si la respuesta vino de la cache (acierto) o de leer los archivos (fallo),
// y la lista de libros encontrados.
static void buscarTextoEnLibros(const struct Catalogo *catalogo, struct Biblioteca *biblioteca) {
    char texto[256];
    leerTexto("Texto a buscar", texto, sizeof(texto));

    if (texto[0] == '\0') {
        printf("No se escribio ningun texto\n");
        return;
    }

    int fueAcierto;

    // Se imprimen las rotaciones para mostrar que el splay chaflanea
    mostrarRotacionesSplay(1);
    biblioteca->cache = buscarTexto(catalogo, biblioteca->cache, texto, &fueAcierto);
    mostrarRotacionesSplay(0);

    biblioteca->busquedas++;

    if (fueAcierto) {
        biblioteca->aciertos++;
        printf("Acierto: el resultado ya estaba en la cache\n");
    } else {
        printf("Fallo: se leyeron los %d libros y el resultado se guardo en la cache\n", catalogo->cantidadLibros);
    }

    // El resultado siempre queda en la raiz de la cache
    struct NodoSplay *resultado = biblioteca->cache;
    printf("\"%s\" aparece en %d libros\n", resultado->texto, resultado->cantidadLibros);

    int mostrados = 0;
    struct LibroEncontrado *libro = resultado->libros;

    while (libro != NULL && mostrados < LIBROS_A_MOSTRAR) {
        printf("  - %s\n", libro->nombre);
        mostrados++;
        libro = libro->siguiente;
    }

    if (resultado->cantidadLibros > mostrados) {
        printf("  ... y %d libros mas\n", resultado->cantidadLibros - mostrados);
    }
}

// Funcion para ver la cache: estadisticas y el arbol splay dibujado de lado
static void verCache(const struct Biblioteca *biblioteca) {
    printf("\nBusquedas: %d  Aciertos de cache: %d  Textos guardados: %d\n",
           biblioteca->busquedas, biblioteca->aciertos, contarSplay(biblioteca->cache));

    if (biblioteca->cache == NULL) {
        printf("La cache esta vacia\n");
        return;
    }

    printf("Arbol splay (la raiz a la izquierda, la ultima busqueda queda en la raiz):\n");
    imprimirSplay(biblioteca->cache, 0);
}

// ---------- Aprender en la biblioteca ----------

// Funcion para que una persona elija que servicio estudiar (uno que no sepa).
// Devuelve 1 si quedo estudiando algo y 0 si no.
static int elegirServicioAEstudiar(struct Persona *persona) {
    struct Comuna *comuna = persona->comuna;

    printf("\nServicios que se pueden aprender:\n");
    int numero = 1;

    for (struct Recurso *servicio = comuna->servicios; servicio != NULL; servicio = servicio->siguiente) {
        printf("%2d. %s%s\n", numero, servicio->nombre,
               sabeServicio(persona, servicio->nombre) ? " (ya lo sabe)" : "");
        numero++;
    }

    struct Recurso *elegido = elegirRecurso(comuna->servicios);

    if (sabeServicio(persona, elegido->nombre)) {
        printf("%s ya sabe %s\n", persona->nombre, elegido->nombre);
        return 0;
    }

    snprintf(persona->servicioEnEstudio, sizeof(persona->servicioEnEstudio), "%s", elegido->nombre);
    persona->librosParaServicio = 0;
    return 1;
}

// Funcion para ir a la biblioteca: una persona busca textos en la biblioteca de su
// comuna. Cada busqueda con resultados le hace leer un libro que no habia leido.
// Al leer 3 libros aprende el servicio que estaba estudiando.
static void irABiblioteca(struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Catalogo *catalogo, const struct Jugador *jugador) {
    char nombre[50];
    leerTexto("Nombre de la persona (ej. Ana Sanchez 2)", nombre, sizeof(nombre));

    // La persona se busca en la tabla hash
    struct Persona *persona = buscarPersonaTabla(tabla, nombre);

    if (persona == NULL || persona->comuna == NULL) {
        printf("No hay ninguna persona llamada \"%s\"\n", nombre);
        return;
    }

    struct Comuna *comuna = persona->comuna;

    // Solo las personas de las comunas del jugador
    if (!estaAdministrada(jugador, comuna)) {
        printf("%s vive en %s, que no es una de sus comunas\n", persona->nombre, comuna->nombre);
        return;
    }

    printf("%s va a la biblioteca de %s\n", persona->nombre, comuna->nombre);

    // Si no esta estudiando nada, elige que servicio quiere aprender
    if (persona->servicioEnEstudio[0] == '\0' && !elegirServicioAEstudiar(persona)) {
        return;
    }

    printf("%s estudia %s (%d/%d libros)\n", persona->nombre, persona->servicioEnEstudio,
           persona->librosParaServicio, LIBROS_PARA_APRENDER);

    // Cada vuelta es una busqueda; Enter vacio para salir de la biblioteca
    while (1) {
        char texto[256];
        leerTexto("\nTexto a buscar (Enter para salir de la biblioteca)", texto, sizeof(texto));

        if (texto[0] == '\0') {
            return;
        }

        // Busca en la cache de la biblioteca de su comuna, mostrando las rotaciones
        int fueAcierto;
        mostrarRotacionesSplay(1);
        comuna->biblioteca.cache = buscarTexto(catalogo, comuna->biblioteca.cache, texto, &fueAcierto);
        mostrarRotacionesSplay(0);

        comuna->biblioteca.busquedas++;
        if (fueAcierto) {
            comuna->biblioteca.aciertos++;
        }

        struct NodoSplay *resultado = comuna->biblioteca.cache;
        printf("%s: \"%s\" aparece en %d libros\n", fueAcierto ? "Acierto de cache" : "Fallo de cache",
               resultado->texto, resultado->cantidadLibros);

        if (resultado->cantidadLibros == 0) {
            printf("Ningun libro tiene ese texto: no cuenta como lectura\n");
            continue;
        }

        // Busca el primer libro del resultado que todavia no haya leido
        struct LibroEncontrado *libro = resultado->libros;

        while (libro != NULL && yaLeyo(persona, libro->nombre)) {
            libro = libro->siguiente;
        }

        if (libro == NULL) {
            printf("%s ya leyo todos los libros con ese texto; pruebe con otro\n", persona->nombre);
            continue;
        }

        // Guarda el nombre del servicio antes de leer: leerLibro lo borra si aprende
        char servicio[50];
        snprintf(servicio, sizeof(servicio), "%s", persona->servicioEnEstudio);

        int aprendio = leerLibro(persona, libro->nombre);
        printf("%s leyo \"%s\"", persona->nombre, libro->nombre);

        if (!aprendio) {
            printf(" (%d/%d libros)\n", persona->librosParaServicio, LIBROS_PARA_APRENDER);
            continue;
        }

        printf("\n%s aprendio %s! Ahora su comuna cuenta con ese servicio.\n", persona->nombre, servicio);

        // Cambio lo que saben las personas de la comuna: se recalculan servicios e indices
        recalcularServicios(sociedad, tabla);
        actualizarIndices(sociedad);
        return;
    }
}

// ---------- Jugador ----------

// Funcion para que el jugador elija sus 3 comunas al inicio
static void elegirComunasIniciales(struct Comuna *sociedad, struct Jugador *jugador) {
    printf("\nComunas de la sociedad:\n");
    int numero = 1;

    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        printf("%2d. %s (%d personas)\n", numero, comuna->nombre, comuna->cantidadPersonas);
        numero++;
    }

    printf("Elija las %d comunas que va a administrar\n", COMUNAS_ADMINISTRADAS);

    while (contarAdministradas(jugador) < COMUNAS_ADMINISTRADAS) {
        struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna");

        if (agregarAdministrada(jugador, comuna)) {
            printf("Ahora administra %s\n", comuna->nombre);
        } else {
            printf("Ya administra %s; elija otra\n", comuna->nombre);
        }
    }
}

// ---------- Paso de los dias ----------

// Funcion para que el jugador elija otra comuna cuando se disuelve una de las suyas
static void elegirReemplazo(struct Comuna *sociedad, struct Jugador *jugador) {
    printf("Elija una comuna nueva para administrar (no puede ser una de las suyas)\n");

    while (1) {
        struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna");

        if (agregarAdministrada(jugador, comuna)) {
            printf("Ahora administra %s\n", comuna->nombre);
            return;
        }

        printf("Ya administra %s; elija otra\n", comuna->nombre);
    }
}

// Funcion para revisar que comunas estan en crisis y disolver las que llevan
// 3 dias seguidos. Devuelve el nuevo inicio de la sociedad y deja *perdio en 1
// si la partida se perdio.
static struct Comuna *revisarDisoluciones(struct Comuna *sociedad, struct TablaPersonas *tabla, struct Jugador *jugador, int *perdio) {
    struct Comuna *comuna = sociedad;
    int huboDisolucion = 0;

    while (comuna != NULL) {
        // Se guarda la siguiente antes, porque la comuna se puede liberar
        struct Comuna *siguiente = comuna->siguiente;

        if (comuna->necesidad >= CRISIS_NECESIDAD || comuna->satisfaccion <= CRISIS_SATISFACCION) {
            comuna->diasEnCrisis++;

            if (comuna->diasEnCrisis < DIAS_PARA_DISOLVER) {
                printf("!!! %s esta en crisis (dia %d de %d) !!!\n",
                       comuna->nombre, comuna->diasEnCrisis, DIAS_PARA_DISOLVER);
            } else {
                int esDelJugador = estaAdministrada(jugador, comuna);
                printf("*** %s se disolvio: sus %d personas se mudaron a las comunas cercanas ***\n",
                       comuna->nombre, comuna->cantidadPersonas);

                // Si era del jugador, se saca de su lista y pierde una vida
                if (esDelJugador) {
                    quitarAdministrada(jugador, comuna);
                    jugador->vidasPerdidas++;
                    printf("Era una de sus comunas. Vidas restantes: %d de %d\n", VIDAS - jugador->vidasPerdidas, VIDAS);
                }

                sociedad = disolverComuna(sociedad, comuna, tabla);
                huboDisolucion = 1;
            }
        } else {
            // Salio de la crisis: la cuenta vuelve a empezar
            comuna->diasEnCrisis = 0;
        }

        comuna = siguiente;
    }

    if (!huboDisolucion) {
        return sociedad;
    }

    // Las personas se mudaron: se recalculan servicios e indices
    recalcularServicios(sociedad, tabla);
    actualizarIndices(sociedad);

    // Dos formas de perder: 3 vidas o menos de 3 comunas en la sociedad (lo pide el PDF)
    if (jugador->vidasPerdidas >= VIDAS) {
        printf("\n=== PARTIDA PERDIDA: se disolvieron %d de sus comunas ===\n", VIDAS);
        *perdio = 1;
        return sociedad;
    }

    if (contarComunas(sociedad) < COMUNAS_ADMINISTRADAS) {
        printf("\n=== FRACASARON: la sociedad tiene menos de %d comunas ===\n", COMUNAS_ADMINISTRADAS);
        *perdio = 1;
        return sociedad;
    }

    // Si perdio comunas pero le quedan vidas, elige reemplazos
    while (contarAdministradas(jugador) < COMUNAS_ADMINISTRADAS) {
        mostrarEstado(sociedad, 0, jugador);
        elegirReemplazo(sociedad, jugador);
    }

    return sociedad;
}

// Funcion para simular un dia completo. Devuelve el nuevo inicio de la sociedad
// (cambia si se disuelve la primera comuna).
static struct Comuna *simularDia(struct Comuna *sociedad, struct TablaPersonas *tabla, struct Jugador *jugador, int dia, int *perdio) {
    printf("\n--- Dia %d ---\n", dia);

    consumirRecursos(sociedad);
    producirRecursos(sociedad);
    // El sol carga los celulares segun los paneles solares de cada comuna
    cargarCelulares(sociedad);
    decaerEfectos(sociedad);
    // Los servicios vuelven a ser lo que saben las personas
    // (una emergencia de ayer solo los interrumpio ese dia)
    recalcularServicios(sociedad, tabla);
    // Las comunas autonomas se ayudan solas
    ayudaAutonoma(sociedad, tabla, jugador);
    avanzarEmergencias(sociedad);
    actualizarIndices(sociedad);

    // Al final del dia: crisis y disoluciones
    return revisarDisoluciones(sociedad, tabla, jugador, perdio);
}

// ---------- Menu del dia ----------

// Valor que devuelve el menu cuando el celular pide la autodestruccion
#define AUTODESTRUCCION -1

// Funcion para mostrar el menu del dia. Devuelve cuantos dias hay que avanzar
// (1 con la opcion 4, de 1 a 30 con la opcion 11), 0 para salir
// o AUTODESTRUCCION si en el celular se eligio la opcion 5
static int menuDia(struct Comuna *sociedad, int dia, const struct Catalogo *catalogo, struct TablaPersonas *tabla, const struct Jugador *jugador) {
    // Repite el menu hasta pasar de dia o salir
    while (1) {
        printf("\n1. Hacer un regalo entre comunas\n");
        printf("2. Ver los recursos de una comuna\n");
        printf("3. Ver el estado de las comunas\n");
        printf("4. Pasar al siguiente dia\n");
        printf("5. Buscar un libro por nombre (AVL)\n");
        printf("6. Buscar un texto en los libros (cache splay)\n");
        printf("7. Ver la cache de busquedas\n");
        printf("8. Ver las personas de una comuna\n");
        printf("9. Buscar una persona por nombre (tabla hash)\n");
        printf("10. Ir a la biblioteca (aprender un servicio)\n");
        printf("11. Pasar varios dias\n");
        printf("12. Abrir el celular de una de sus comunas\n");
        printf("0. Salir\n");

        int opcion = leerEntero("Opcion", 0, 12);

        if (opcion == 0) {
            return 0;
        }
        if (opcion == 1) {
            hacerRegalo(sociedad, tabla, jugador);
        }
        if (opcion == 2) {
            verComuna(sociedad);
        }
        if (opcion == 3) {
            mostrarEstado(sociedad, dia, jugador);
        }
        if (opcion == 4) {
            return 1;
        }
        if (opcion == 5) {
            buscarLibroPorNombre(catalogo);
        }
        if (opcion == 6) {
            struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna (su biblioteca)");
            buscarTextoEnLibros(catalogo, &comuna->biblioteca);
        }
        if (opcion == 7) {
            struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna (su biblioteca)");
            printf("\nBiblioteca de %s", comuna->nombre);
            verCache(&comuna->biblioteca);
        }
        if (opcion == 8) {
            verPersonasComuna(sociedad, tabla);
        }
        if (opcion == 9) {
            buscarPersonaMenu(tabla);
        }
        if (opcion == 10) {
            irABiblioteca(sociedad, tabla, catalogo, jugador);
        }
        if (opcion == 11) {
            return leerEntero("Cuantos dias", 1, 30);
        }
        if (opcion == 12) {
            struct Comuna *comuna = elegirComuna(sociedad, "Numero de la comuna (su celular)");

            if (!estaAdministrada(jugador, comuna)) {
                printf("Solo puede usar el celular de sus comunas (las marcadas con *)\n");
            } else if (abrirCelular(comuna)) {
                return AUTODESTRUCCION;
            }
        }
    }
}

// Funcion para anunciar cada liberacion de memoria (solo en la autodestruccion)
static void anunciarLiberacion(int anunciar, const char *texto) {
    if (anunciar) {
        printf("%s... listo\n", texto);
    }
}

// ---------- Juego ----------

// Funcion para iniciar el juego: carga datos, crea la sociedad y corre los dias
void iniciarJuego(void) {
    // Usa la hora como semilla para que cada partida sea distinta.
    // Va primero porque los porcentajes de consumo y produccion tambien son al azar
    srand((unsigned int) time(NULL));

    // Carga los cuatro archivos de datos
    struct TablaPersonas tabla;
    iniciarTablaPersonas(&tabla);
    int totalPersonas = cargarTablaPersonas(&tabla, "personas");
    struct Recurso *catalogoBienes = cargarRecursos("bienes");
    struct Recurso *catalogoServicios = cargarRecursos("servicios");
    struct Comuna *nombresComunas = cargarComunas();

    // Si falta algun archivo, libera lo cargado y termina
    if (totalPersonas == 0 || catalogoBienes == NULL ||
        catalogoServicios == NULL || nombresComunas == NULL) {
        printf("Faltan datos, no se puede iniciar la simulacion\n");
        liberarTablaPersonas(&tabla);
        liberarRecursos(catalogoBienes);
        liberarRecursos(catalogoServicios);
        liberarComunas(nombresComunas);
        return;
    }

    // Cuenta lo que se cargo para mostrarlo y validar
    int totalBienes = contarRecursos(catalogoBienes);
    int totalServicios = contarRecursos(catalogoServicios);
    int totalComunas = contarComunas(nombresComunas);

    printf("Personas cargadas: %d\n", totalPersonas);
    imprimirEstadisticasTabla(&tabla);
    printf("Bienes disponibles: %d\n", totalBienes);
    printf("Servicios disponibles: %d\n", totalServicios);
    printf("Comunas disponibles: %d\n\n", totalComunas);

    // Carga el catalogo de libros (AVL), compartido por las bibliotecas de todas las comunas
    struct Catalogo catalogo;
    cargarBiblioteca(&catalogo);

    // No mas de 50 comunas ni mas que los nombres disponibles
    int maximoComunas = totalComunas < 50 ? totalComunas : 50;

    // Hacen falta al menos 3 comunas: con menos, la partida ya estaria perdida
    if (maximoComunas < COMUNAS_ADMINISTRADAS) {
        printf("El archivo comunas debe tener al menos %d nombres\n", COMUNAS_ADMINISTRADAS);
        liberarCatalogo(&catalogo);
        liberarTablaPersonas(&tabla);
        liberarRecursos(catalogoBienes);
        liberarRecursos(catalogoServicios);
        liberarComunas(nombresComunas);
        return;
    }

    // Pide la configuracion de la partida
    int cantidadComunas = leerEntero("Cuantas comunas", COMUNAS_ADMINISTRADAS, maximoComunas);
    int cantidadBienes = leerEntero("Cuantos bienes", 1, totalBienes);
    int cantidadServicios = leerEntero("Cuantos servicios", 1, totalServicios);

    // Se activa si en el celular se elige la opcion 5: se anuncia cada liberacion
    int autodestruccion = 0;

    // Crea las comunas con sus habitantes
    struct Comuna *sociedad = crearSociedad(nombresComunas, cantidadComunas);

    if (sociedad != NULL) {
        // Prepara recursos, porcentajes, emergencias e indices iniciales
        asignarRecursos(sociedad, catalogoBienes, cantidadBienes, catalogoServicios, cantidadServicios);

        // Reparte las personas reales y sus servicios; los servicios de cada
        // comuna salen de lo que saben sus personas
        asignarPersonas(sociedad, &tabla);
        asignarServiciosPersonas(&tabla, catalogoServicios, cantidadServicios);
        recalcularServicios(sociedad, &tabla);

        asignarConsumo(sociedad);
        asignarProduccion(sociedad);
        iniciarEmergencias();

        // Dos veces: la primera calcula una satisfaccion de partida y la segunda
        // ya puede usar la satisfaccion de las vecinas
        actualizarIndices(sociedad);
        actualizarIndices(sociedad);

        // Todos los celulares empiezan con la bateria llena
        iniciarCelulares(sociedad);

        // El jugador elige las 3 comunas que va a administrar
        struct Jugador jugador;
        iniciarJugador(&jugador);
        elegirComunasIniciales(sociedad, &jugador);

        // Bucle principal: el dia 1 solo muestra el estado inicial;
        // despues el menu dice cuantos dias avanzar (0 para salir)
        int dia = 1;
        int perdio = 0;
        mostrarEstado(sociedad, dia, &jugador);

        while (!perdio) {
            int diasAvanzar = menuDia(sociedad, dia, &catalogo, &tabla, &jugador);

            if (diasAvanzar == 0) {
                break;
            }

            if (diasAvanzar == AUTODESTRUCCION) {
                autodestruccion = 1;
                break;
            }

            for (int i = 0; i < diasAvanzar && !perdio; i++) {
                dia++;
                sociedad = simularDia(sociedad, &tabla, &jugador, dia, &perdio);
            }

            mostrarEstado(sociedad, dia, &jugador);
        }

        if (perdio) {
            printf("La sociedad llego hasta el dia %d\n", dia);
        }

        // Al salir, libera la lista del jugador (solo sus nodos) y la sociedad
        anunciarLiberacion(autodestruccion, "Liberando la lista de comunas del jugador");
        liberarJugador(&jugador);
        anunciarLiberacion(autodestruccion, "Liberando las comunas: bienes, servicios y la cache de cada biblioteca (lista doble + splay)");
        liberarComunas(sociedad);
    }

    // Libera el catalogo (AVL); la cache de cada comuna ya se libero con liberarComunas
    anunciarLiberacion(autodestruccion, "Liberando el catalogo de libros (AVL)");
    liberarCatalogo(&catalogo);

    // Libera los datos cargados de los archivos
    anunciarLiberacion(autodestruccion, "Liberando las personas y sus servicios (tabla hash)");
    liberarTablaPersonas(&tabla);
    anunciarLiberacion(autodestruccion, "Liberando los catalogos de bienes y servicios");
    liberarRecursos(catalogoBienes);
    liberarRecursos(catalogoServicios);
    anunciarLiberacion(autodestruccion, "Liberando los nombres de las comunas");
    liberarComunas(nombresComunas);

    if (autodestruccion) {
        printf("\nLa sociedad se disolvio en el vacio. No quedo ni un byte.\n");
    }
}
