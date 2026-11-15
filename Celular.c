#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "Celular.h"
#include "Sociedad.h"
#include "Indices.h"
#include "Economia.h"

// Ancho de la pantalla del celular (sin los bordes)
#define ANCHO_PANTALLA 30

// ---------- Bateria y energia solar ----------

// Funcion para dejar la bateria de todos los celulares llena
void iniciarCelulares(struct Comuna *sociedad) {
    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        comuna->bateria = BATERIA_MAXIMA;
    }
}

// Funcion para cargar los celulares: con los paneles al 100% carga CARGA_SOLAR_MAXIMA,
// con los paneles a la mitad carga la mitad, y asi. Sin el bien en la partida, carga poco.
void cargarCelulares(struct Comuna *sociedad) {
    for (struct Comuna *comuna = sociedad; comuna != NULL; comuna = comuna->siguiente) {
        struct Recurso *paneles = buscarRecurso(comuna->bienes, "Paneles solares");
        int carga = CARGA_SIN_PANELES;

        if (paneles != NULL && paneles->maximo > 0) {
            carga = CARGA_SOLAR_MAXIMA * paneles->existencia / paneles->maximo;
        }

        comuna->bateria += carga;

        if (comuna->bateria > BATERIA_MAXIMA) {
            comuna->bateria = BATERIA_MAXIMA;
        }
    }
}

// Funcion para gastar bateria. Devuelve 1 si alcanzo y 0 si no habia suficiente.
static int gastarBateria(struct Comuna *comuna, int gasto) {
    if (comuna->bateria < gasto) {
        return 0;
    }
    comuna->bateria -= gasto;
    return 1;
}

// ---------- Dibujo de la pantalla ----------

// Funcion para dibujar el borde de arriba o de abajo del celular
static void bordeCelular(void) {
    printf("   +");
    for (int i = 0; i < ANCHO_PANTALLA + 2; i++) {
        printf("-");
    }
    printf("+\n");
}

// Funcion para saber si un byte es la continuacion de una letra UTF-8
// (letras como la a con tilde o la enie ocupan 2 bytes; los de continuacion son 10xxxxxx)
static int esContinuacionUtf8(unsigned char byte) {
    return (byte & 0xC0) == 0x80;
}

// Funcion para dibujar una linea de texto dentro de la pantalla.
// Cuenta letras y no bytes, para que las tildes no descuadren el borde.
static void lineaCelular(const char *texto) {
    int letras = 0;
    int i = 0;

    printf("   | ");

    // Copia hasta ANCHO_PANTALLA letras (una letra puede tener varios bytes)
    while (texto[i] != '\0') {
        if (!esContinuacionUtf8((unsigned char) texto[i])) {
            if (letras == ANCHO_PANTALLA) {
                break;
            }
            letras++;
        }
        putchar(texto[i]);
        i++;
    }

    // Rellena con espacios hasta el ancho de la pantalla
    for (; letras < ANCHO_PANTALLA; letras++) {
        putchar(' ');
    }

    printf(" |\n");
}

// Funcion para dibujar la barra de arriba: nombre de la comuna y bateria
static void barraEstado(const struct Comuna *comuna) {
    char barra[11];
    int llenos = comuna->bateria / 10;

    for (int i = 0; i < 10; i++) {
        barra[i] = (i < llenos) ? '#' : ' ';
    }
    barra[10] = '\0';

    char linea[64];
    snprintf(linea, sizeof(linea), "%-13.13s [%s]%3d%%", comuna->nombre, barra, comuna->bateria);
    lineaCelular(linea);
}

// Funcion para dibujar la pantalla de inicio con las apps
static void pantallaInicio(const struct Comuna *comuna) {
    printf("\n");
    bordeCelular();
    barraEstado(comuna);
    lineaCelular("------------------------------");
    lineaCelular("");
    lineaCelular("  1. Biblioteca");
    lineaCelular("  2. Red de ayuda mutua");
    // Si hay notificaciones nuevas, se muestra cuantas
    char notificaciones[64];
    if (comuna->bandeja.noLeidas > 0) {
        snprintf(notificaciones, sizeof(notificaciones), "  3. Notificaciones (%d)", comuna->bandeja.noLeidas);
    } else {
        snprintf(notificaciones, sizeof(notificaciones), "  3. Notificaciones");
    }
    lineaCelular(notificaciones);
    lineaCelular("  4. ???");
    lineaCelular("  5. ???");
    lineaCelular("");
    lineaCelular("  0. Apagar");
    bordeCelular();
}

// ---------- Secretos ----------

// Funcion para esperar unos milisegundos (sirve igual en Windows y Linux)
static void pausa(int milisegundos) {
    fflush(stdout);
    clock_t fin = clock() + (clock_t) milisegundos * CLOCKS_PER_SEC / 1000;

    while (clock() < fin) {
        // Espera activa: solo se usa en la animacion de la opcion 5
    }
}

// Funcion para mostrar el meme secreto (opcion 4)
static void mostrarMeme(void) {
    printf("\nAbriste algo que no debias...\n");
    printf("Link:    %s\n", LINK_MEME);
    printf("Archivo: %s\n", ARCHIVO_MEME);
    printf("(Ctrl + clic para abrirlo)\n");
}

// Funcion para la animacion de la explosion (opcion 5), en tres cuadros:
// la ciudad con una chispa, el hongo de la explosion y el polvo de la memoria liberada
static void explosion(void) {
    printf("\n\n");
// Cuadro: ciudad
    printf("                          .\n");
    printf("                         (*)\n");
    printf("                          '\n");
    printf("                          :\n");
    printf("                          .\n");
    printf("      ___    _                            _    ___\n");
    printf("     |# #|  | |__   _____          _____ __| |  |# #|\n");
    printf("     |# #|__|#  #| |# # #|  ____  |# # #|#  #|__|# #|\n");
    printf("   __|# #|# #|# #|_|# # #|_|#  #|_|# # #|# #|# #|# #|__\n");
    printf("   ..::::::::::::::::::::::::::::::::::::::::::::::::..\n");
    printf("\n            ...algo se esta calentando...\n\n");
    pausa(900);

// Cuadro: hongo
    printf("           _ . - ~ ~ ~ ~ ~ ~ ~ ~ - . _\n");
    printf("      . ~ ( (    (    )    )    ) )  ~ .\n");
    printf("    (   (    B   O   O   O   O   M  !   )   )\n");
    printf("     ~ .  (    (    (    )    )    )  . ~\n");
    printf("         ~ - . _ ( ( ( ) ) ) _ . - ~\n");
    printf("                     |  |   |  |\n");
    printf("                     |  |   |  |\n");
    printf("                  .--'  '   '  '--.\n");
    printf("      ___    _   (  (  (  ( )  )  )  )   _    ___\n");
    printf("     |# #|  | |_  ~ - . _______ . - ~  _| |  |# #|\n");
    printf("     |# #|__|#  | |# #|  |  |  |# #|  |  #|__|# #|\n");
    printf("   __|# #|# #|# |_|# #|__|__|__|# #|__|# #|# #|# #|__\n");
    printf("   ..::::::::::::::::::::::::::::::::::::::::::::::::..\n");
    printf("\n");
    pausa(1000);

// Cuadro: polvo
    printf("        .     .      .     .      .     .     .\n");
    printf("     .     free()      free()      free()      .\n");
    printf("        .      .    .        .    .      .     .\n");
    printf("     .    free()   .   free()   .   free()   .\n");
    printf("        .     .      .     .      .     .     .\n");
    printf("            la memoria vuelve al vacio\n");
    printf("\n");
    pausa(500);
}

// ---------- App Biblioteca ----------

// Funcion para empezar una pantalla: borde, barra de estado, titulo y separador
static void inicioPantalla(const struct Comuna *comuna, const char *titulo) {
    printf("\n");
    bordeCelular();
    barraEstado(comuna);
    lineaCelular("------------------------------");
    lineaCelular(titulo);
    lineaCelular("");
}

// Funcion para leer una opcion de texto (la primera letra en minuscula).
// Devuelve 'x' si se cierra la entrada.
static char leerLetra(const char *mensaje) {
    char linea[100];
    printf("%s: ", mensaje);

    if (fgets(linea, sizeof(linea), stdin) == NULL) {
        return 'x';
    }

    return (char) tolower((unsigned char) linea[0]);
}

// Funcion para pedir un texto en el celular (quita el salto de linea)
static void leerTextoCelular(const char *mensaje, char *destino, int tamano) {
    printf("%s: ", mensaje);

    if (fgets(destino, tamano, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    destino[strcspn(destino, "\r\n")] = '\0';
}

// Funcion para explicar por que una opcion de paginas no hizo nada
// (se llama solo cuando la opcion no se pudo hacer)
static void avisoOpcion(char letra) {
    if (letra == 's') {
        printf("No hay mas paginas. Use A para volver atras o X para salir.\n");
    } else if (letra == 'a') {
        printf("Ya esta en la primera pagina. Use S para avanzar o X para salir.\n");
    } else {
        printf("Opcion no valida: use S, A o X.\n");
    }
}

// Funcion para recorrer el AVL en orden e imprimir solo los libros de una pagina.
// contador lleva la posicion del libro actual en orden alfabetico.
static void listarPagina(struct NodoAvl *nodo, int desde, int hasta, int *contador) {
    if (nodo == NULL || *contador >= hasta) {
        return;
    }

    listarPagina(nodo->izquierdo, desde, hasta, contador);

    if (*contador >= desde && *contador < hasta) {
        char linea[64];
        snprintf(linea, sizeof(linea), "%3d. %.50s", *contador + 1, nodo->nombre);
        lineaCelular(linea);
    }
    (*contador)++;

    listarPagina(nodo->derecho, desde, hasta, contador);
}

// Funcion para ver los libros del catalogo, de 8 en 8, en orden alfabetico
static void verLibros(struct Comuna *comuna, const struct Catalogo *catalogo) {
    int paginas = (catalogo->cantidadLibros + LIBROS_POR_PAGINA - 1) / LIBROS_POR_PAGINA;
    int pagina = 0;

    while (1) {
        if (!gastarBateria(comuna, GASTO_PAGINA)) {
            printf("Se acabo la bateria.\n");
            return;
        }

        char titulo[64];
        snprintf(titulo, sizeof(titulo), "LIBROS  (pagina %d de %d)", pagina + 1, paginas);
        inicioPantalla(comuna, titulo);

        int contador = 0;
        listarPagina(catalogo->libros, pagina * LIBROS_POR_PAGINA, (pagina + 1) * LIBROS_POR_PAGINA, &contador);

        lineaCelular("");
        lineaCelular("[S] Sig  [A] Ant  [X] Volver");
        bordeCelular();

        // Pide la opcion hasta que sea valida (asi no se gasta bateria redibujando)
        while (1) {
            char letra = leerLetra("Opcion");

            if (letra == 'x') {
                return;
            }
            if (letra == 's' && pagina < paginas - 1) {
                pagina++;
                break;
            }
            if (letra == 'a' && pagina > 0) {
                pagina--;
                break;
            }
            avisoOpcion(letra);
        }
    }
}

// Funcion para buscar un libro por nombre exacto en el AVL
static void buscarNombreCelular(struct Comuna *comuna, const struct Catalogo *catalogo) {
    if (!gastarBateria(comuna, GASTO_BUSQUEDA_CACHE)) {
        printf("Se acabo la bateria.\n");
        return;
    }

    char nombre[256];
    leerTextoCelular("Nombre exacto del libro", nombre, sizeof(nombre));

    inicioPantalla(comuna, "BUSCAR POR NOMBRE (AVL)");
    lineaCelular(nombre);
    lineaCelular(existeLibro(catalogo, nombre) ? "Si esta en la biblioteca" : "No esta en la biblioteca");
    bordeCelular();
}

// Funcion para buscar un texto con la cache de la biblioteca de la comuna.
// Un acierto gasta poca bateria; un fallo gasta mas porque lee los archivos.
static void buscarTextoCelular(struct Comuna *comuna, const struct Catalogo *catalogo) {
    if (comuna->bateria < GASTO_BUSQUEDA_ARCHIVOS) {
        printf("No hay bateria suficiente para buscar (hace falta %d%%).\n", GASTO_BUSQUEDA_ARCHIVOS);
        return;
    }

    char texto[256];
    leerTextoCelular("Texto a buscar", texto, sizeof(texto));

    if (texto[0] == '\0') {
        return;
    }

    int fueAcierto;
    comuna->biblioteca.cache = buscarTexto(catalogo, comuna->biblioteca.cache, texto, &fueAcierto);
    comuna->biblioteca.busquedas++;

    if (fueAcierto) {
        comuna->biblioteca.aciertos++;
        gastarBateria(comuna, GASTO_BUSQUEDA_CACHE);
    } else {
        gastarBateria(comuna, GASTO_BUSQUEDA_ARCHIVOS);
    }

    struct NodoSplay *resultado = comuna->biblioteca.cache;
    char linea[64];

    inicioPantalla(comuna, "BUSCAR TEXTO (cache splay)");
    snprintf(linea, sizeof(linea), "\"%.50s\"", resultado->texto);
    lineaCelular(linea);
    snprintf(linea, sizeof(linea), "%s: %d libros", fueAcierto ? "Acierto, -1%" : "Fallo, -5%", resultado->cantidadLibros);
    lineaCelular(linea);
    lineaCelular("");

    int mostrados = 0;
    for (struct LibroEncontrado *libro = resultado->libros; libro != NULL && mostrados < 5; libro = libro->siguiente) {
        snprintf(linea, sizeof(linea), " - %.50s", libro->nombre);
        lineaCelular(linea);
        mostrados++;
    }

    if (resultado->cantidadLibros > mostrados) {
        snprintf(linea, sizeof(linea), "   ... y %d mas", resultado->cantidadLibros - mostrados);
        lineaCelular(linea);
    }

    bordeCelular();
}

// Funcion para calcular cuantos bytes del texto caben en una linea de la pantalla,
// cortando en el ultimo espacio para no partir palabras.
// En *siguiente deja donde empieza el resto del texto (sin el espacio del corte).
static int cortarLinea(const char *texto, const char **siguiente) {
    int letras = 0;
    int ultimoEspacio = -1;
    int i = 0;

    while (texto[i] != '\0') {
        if (!esContinuacionUtf8((unsigned char) texto[i])) {
            if (letras == ANCHO_PANTALLA) {
                break;
            }
            letras++;
        }
        if (texto[i] == ' ') {
            ultimoEspacio = i;
        }
        i++;
    }

    // Todo el resto cabe en la linea
    if (texto[i] == '\0') {
        *siguiente = texto + i;
        return i;
    }

    // Corta en el ultimo espacio; si la palabra es mas larga que la pantalla, la parte
    if (ultimoEspacio > 0) {
        *siguiente = texto + ultimoEspacio + 1;
        return ultimoEspacio;
    }

    *siguiente = texto + i;
    return i;
}

// Funcion para mostrar una pagina de un libro. Las lineas largas del archivo se
// parten en varias lineas de la pantalla. Devuelve 1 si hay mas paginas.
static int mostrarPaginaLibro(const char *ruta, int pagina) {
    FILE *archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        lineaCelular("(no se pudo abrir el libro)");
        return 0;
    }

    int desde = pagina * LINEAS_POR_PAGINA;
    int hasta = desde + LINEAS_POR_PAGINA;
    int numero = 0;     // numero de la linea de pantalla actual
    int hayMas = 0;
    char linea[1024];

    while (!hayMas && fgets(linea, sizeof(linea), archivo) != NULL) {
        linea[strcspn(linea, "\r\n")] = '\0';

        // Cambia tabulaciones y otros caracteres de control por espacios
        for (int i = 0; linea[i] != '\0'; i++) {
            if ((unsigned char) linea[i] < 32) {
                linea[i] = ' ';
            }
        }

        // Parte la linea en pedazos que caben (una linea vacia cuenta como una)
        const char *resto = linea;

        do {
            // Ya se lleno la pagina y todavia queda texto: hay otra pagina
            if (numero >= hasta) {
                hayMas = 1;
                break;
            }

            const char *siguiente;
            int bytes = cortarLinea(resto, &siguiente);

            if (numero >= desde) {
                char pedazo[sizeof(linea)];
                snprintf(pedazo, sizeof(pedazo), "%.*s", bytes, resto);
                lineaCelular(pedazo);
            }

            numero++;
            resto = siguiente;
        } while (*resto != '\0');
    }

    fclose(archivo);

    // Si la pagina quedo corta, completa con lineas vacias para que el celular no cambie de tamano
    for (; numero < hasta; numero++) {
        if (numero >= desde) {
            lineaCelular("");
        }
    }

    return hayMas;
}

// Funcion para leer un numero entre minimo y maximo (repite hasta que sea valido).
// Devuelve minimo si se cierra la entrada.
static int leerNumeroCelular(const char *mensaje, int minimo, int maximo) {
    char linea[100];

    while (1) {
        printf("%s (%d-%d): ", mensaje, minimo, maximo);

        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            return minimo;
        }

        char *fin;
        long numero = strtol(linea, &fin, 10);

        if (fin != linea && numero >= minimo && numero <= maximo) {
            return (int) numero;
        }

        printf("Escriba un numero entre %d y %d.\n", minimo, maximo);
    }
}

// Funcion para elegir quien lee: muestra las personas de la comuna con un numero.
// Devuelve la persona elegida o NULL si se eligio 0 (nadie en especial).
static struct Persona *elegirLector(struct TablaPersonas *tabla, const struct Comuna *comuna) {
    printf("\nPersonas de %s:\n", comuna->nombre);
    printf("   0. Nadie en especial (la lectura no cuenta para aprender)\n");

    int cantidad = imprimirPersonasComuna(tabla, comuna);
    int opcion = leerNumeroCelular("Quien lee?", 0, cantidad);

    if (opcion == 0) {
        return NULL;
    }

    return personaNumero(tabla, comuna, opcion);
}

// Funcion para leer un libro pagina por pagina. Si lee una persona de la comuna
// y llega a PAGINAS_PARA_LEER paginas, el libro cuenta para su servicio en estudio.
static void leerLibroCelular(struct Comuna *comuna, struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Catalogo *catalogo) {
    char nombre[256];
    leerTextoCelular("Nombre exacto del libro", nombre, sizeof(nombre));

    // El AVL confirma que el libro existe antes de abrirlo
    if (!existeLibro(catalogo, nombre)) {
        printf("El libro \"%s\" no esta en la biblioteca.\n", nombre);
        return;
    }

    // Quien lee: se elige por numero entre las personas de la comuna
    struct Persona *lector = elegirLector(tabla, comuna);

    char ruta[1024];
    snprintf(ruta, sizeof(ruta), "%s/%s", catalogo->carpeta, nombre);

    int pagina = 0;
    int paginasLeidas = 0;
    int contado = 0;

    while (1) {
        if (!gastarBateria(comuna, GASTO_PAGINA)) {
            printf("Se acabo la bateria. El celular se apago a media lectura.\n");
            return;
        }

        char titulo[64];
        snprintf(titulo, sizeof(titulo), "%.20s  pag. %d", nombre, pagina + 1);
        inicioPantalla(comuna, titulo);
        int hayMas = mostrarPaginaLibro(ruta, pagina);
        lineaCelular("");
        lineaCelular(hayMas ? "[S] Sig  [A] Ant  [X] Cerrar" : "(fin)    [A] Ant  [X] Cerrar");
        bordeCelular();

        paginasLeidas++;

        // Al leer suficientes paginas (o el libro completo), cuenta como leido
        if (lector != NULL && !contado && (paginasLeidas >= PAGINAS_PARA_LEER || !hayMas)) {
            contado = 1;

            if (yaLeyo(lector, nombre)) {
                printf("%s ya habia leido este libro.\n", lector->nombre);
            } else {
                char servicio[50];
                snprintf(servicio, sizeof(servicio), "%s", lector->servicioEnEstudio);

                if (leerLibro(lector, nombre)) {
                    printf("%s termino de leer y aprendio %s!\n", lector->nombre, servicio);
                    notificar(comuna, "%s aprendio %s leyendo en el celular", lector->nombre, servicio);
                    recalcularServicios(sociedad, tabla);
                    actualizarIndices(sociedad);
                } else if (servicio[0] != '\0') {
                    printf("%s leyo el libro (%d/%d para %s).\n", lector->nombre,
                           lector->librosParaServicio, LIBROS_PARA_APRENDER, servicio);
                } else {
                    printf("%s leyo el libro (no esta estudiando ningun servicio).\n", lector->nombre);
                }
            }
        }

        // Pide la opcion hasta que sea valida (asi no se gasta bateria redibujando)
        while (1) {
            char letra = leerLetra("Opcion");

            if (letra == 'x') {
                return;
            }
            if (letra == 's' && hayMas) {
                pagina++;
                break;
            }
            if (letra == 'a' && pagina > 0) {
                pagina--;
                break;
            }
            avisoOpcion(letra);
        }
    }
}

// Funcion para la app Biblioteca: su propio menu dentro del celular
static void appBiblioteca(struct Comuna *comuna, struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Catalogo *catalogo) {
    while (1) {
        inicioPantalla(comuna, "BIBLIOTECA");
        lineaCelular("  1. Ver los libros");
        lineaCelular("  2. Buscar por nombre");
        lineaCelular("  3. Buscar un texto");
        lineaCelular("  4. Leer un libro");
        lineaCelular("");
        lineaCelular("  0. Volver");
        bordeCelular();

        char letra = leerLetra("Opcion");

        if (letra == '0' || letra == 'x') {
            return;
        }
        if (letra == '1') {
            verLibros(comuna, catalogo);
        }
        if (letra == '2') {
            buscarNombreCelular(comuna, catalogo);
        }
        if (letra == '3') {
            buscarTextoCelular(comuna, catalogo);
        }
        if (letra == '4') {
            leerLibroCelular(comuna, sociedad, tabla, catalogo);
        }
    }
}

// ---------- App Red de ayuda mutua ----------

// Pedidos que se muestran en la pantalla de la cola (cada uno ocupa 2 lineas)
#define PEDIDOS_EN_PANTALLA 6

// Funcion para ver la cola de pedidos de toda la sociedad, del frente al final.
// Cada pedido ocupa dos lineas: la comuna que pide y que bien, con los dias que lleva.
// Los pedidos de esta comuna se marcan con *.
static void verColaCelular(struct Comuna *comuna, const struct Cola *pedidos) {
    if (!gastarBateria(comuna, GASTO_PAGINA)) {
        printf("Se acabo la bateria.\n");
        return;
    }

    char linea[64];
    snprintf(linea, sizeof(linea), "COLA DE PEDIDOS (%d)", pedidos->cantidad);
    inicioPantalla(comuna, linea);

    if (colaVacia(pedidos)) {
        lineaCelular("No hay pedidos.");
    } else {
        lineaCelular("El 1 recibe primero");
        lineaCelular("");
    }

    // Recorre del frente (el siguiente en atenderse) hacia el final
    int numero = 0;
    for (struct Pedido *p = pedidos->frente; p != NULL && numero < PEDIDOS_EN_PANTALLA; p = p->siguiente) {
        numero++;
        snprintf(linea, sizeof(linea), "%d. %.24s%s", numero, p->comuna->nombre, p->comuna == comuna ? " *" : "");
        lineaCelular(linea);

        // Dias que lleva esperando, en palabras: hoy, 1 dia, N dias
        char espera[20];
        if (p->diasEsperando == 0) {
            snprintf(espera, sizeof(espera), "hoy");
        } else if (p->diasEsperando == 1) {
            snprintf(espera, sizeof(espera), "1 dia");
        } else {
            snprintf(espera, sizeof(espera), "%d dias", p->diasEsperando);
        }

        snprintf(linea, sizeof(linea), "   %.20s - %s", p->bien, espera);
        lineaCelular(linea);
    }

    if (pedidos->cantidad > numero) {
        snprintf(linea, sizeof(linea), "   ... y %d mas", pedidos->cantidad - numero);
        lineaCelular(linea);
    }

    lineaCelular("");
    lineaCelular("* = pedido de esta comuna");
    lineaCelular("dias = tiempo esperando");
    bordeCelular();
}

// Funcion para pedir un bien: elige uno de los bienes de la comuna por numero
static void pedirBienCelular(struct Comuna *comuna, struct Cola *pedidos) {
    if (!gastarBateria(comuna, GASTO_PAGINA)) {
        printf("Se acabo la bateria.\n");
        return;
    }

    // Lista los bienes con cuanto tiene de cada uno
    printf("\nBienes de %s:\n", comuna->nombre);
    printf("   0. Volver\n");

    int cantidad = 0;
    for (struct Recurso *bien = comuna->bienes; bien != NULL; bien = bien->siguiente) {
        cantidad++;
        int porcentaje = bien->maximo > 0 ? 100 * bien->existencia / bien->maximo : 0;
        printf("%4d. %-24s %3d%%%s\n", cantidad, bien->nombre, porcentaje,
               existePedido(pedidos, comuna, bien->nombre) ? "  (ya pedido)" : "");
    }

    int opcion = leerNumeroCelular("Que bien necesita?", 0, cantidad);

    if (opcion == 0) {
        return;
    }

    struct Recurso *bien = comuna->bienes;
    for (int i = 1; i < opcion; i++) {
        bien = bien->siguiente;
    }

    // Solo se pide lo que hace falta: si tiene de sobra, no es un pedido de ayuda
    if (bien->maximo > 0 && 100 * bien->existencia >= AUTONOMA_EXCEDENTE * bien->maximo) {
        printf("%s tiene suficiente %s (%d%% o mas): no hace falta pedirlo.\n",
               comuna->nombre, bien->nombre, AUTONOMA_EXCEDENTE);
        return;
    }

    if (encolarPedido(pedidos, comuna, bien->nombre)) {
        printf("Pedido enviado: %s necesita %s. Esta en el lugar %d de la cola.\n",
               comuna->nombre, bien->nombre, pedidos->cantidad);
    } else {
        printf("%s ya habia pedido %s.\n", comuna->nombre, bien->nombre);
    }
}

// Funcion para cancelar uno de los pedidos de esta comuna, elegido por numero
static void cancelarPedidoCelular(struct Comuna *comuna, struct Cola *pedidos) {
    printf("\nPedidos de %s:\n", comuna->nombre);
    printf("   0. Volver\n");

    // Numera solo los pedidos de esta comuna, en el orden de la cola
    int cantidad = 0;
    for (struct Pedido *p = pedidos->frente; p != NULL; p = p->siguiente) {
        if (p->comuna == comuna) {
            cantidad++;
            printf("%4d. %s (lleva %d dias esperando)\n", cantidad, p->bien, p->diasEsperando);
        }
    }

    if (cantidad == 0) {
        printf("No tiene pedidos en la cola.\n");
        return;
    }

    int opcion = leerNumeroCelular("Cual cancelar?", 0, cantidad);

    if (opcion == 0) {
        return;
    }

    // Busca el pedido elegido; se copia el nombre porque cancelar libera el nodo
    struct Pedido *p = pedidos->frente;
    int contador = 0;

    while (p != NULL) {
        if (p->comuna == comuna && ++contador == opcion) {
            break;
        }
        p = p->siguiente;
    }

    char bien[50];
    snprintf(bien, sizeof(bien), "%s", p->bien);
    cancelarPedido(pedidos, comuna, bien);
    printf("Se cancelo el pedido de %s.\n", bien);
}

// Funcion para la app Red de ayuda mutua: su propio menu dentro del celular
static void appAyudaMutua(struct Comuna *comuna, struct Cola *pedidos) {
    while (1) {
        char linea[64];
        inicioPantalla(comuna, "RED DE AYUDA MUTUA");
        snprintf(linea, sizeof(linea), "Pedidos en la cola: %d", pedidos->cantidad);
        lineaCelular(linea);
        lineaCelular("");
        lineaCelular("  1. Ver la cola");
        lineaCelular("  2. Pedir un bien");
        lineaCelular("  3. Cancelar un pedido");
        lineaCelular("");
        lineaCelular("  0. Volver");
        bordeCelular();

        char letra = leerLetra("Opcion");

        if (letra == '0' || letra == 'x') {
            return;
        }
        if (letra == '1') {
            verColaCelular(comuna, pedidos);
        }
        if (letra == '2') {
            pedirBienCelular(comuna, pedidos);
        }
        if (letra == '3') {
            cancelarPedidoCelular(comuna, pedidos);
        }
    }
}

// ---------- App Notificaciones ----------

// Notificaciones que se muestran por pagina
#define NOTIFICACIONES_POR_PAGINA 4

// Funcion para mostrar una pagina de notificaciones, de la mas reciente a la mas vieja.
// Cada notificacion muestra su dia y su texto partido en lineas que caben en la pantalla.
// Las primeras "nuevas" se marcan como nuevas. Devuelve 1 si hay mas paginas.
static int mostrarPaginaNotificaciones(const struct Bandeja *bandeja, int pagina, int nuevas) {
    int desde = pagina * NOTIFICACIONES_POR_PAGINA;
    int hasta = desde + NOTIFICACIONES_POR_PAGINA;
    int posicion = 0;  // posicion de la notificacion (0 = la mas reciente)

    // Recorre la pila desde el tope: primero lo mas reciente
    for (struct Notificacion *n = bandeja->tope; n != NULL; n = n->siguiente, posicion++) {
        if (posicion < desde) {
            continue;
        }
        if (posicion >= hasta) {
            return 1;
        }

        char linea[64];
        snprintf(linea, sizeof(linea), "Dia %d%s", n->dia, posicion < nuevas ? "  (nueva)" : "");
        lineaCelular(linea);

        // El texto se parte en lineas que caben en la pantalla
        const char *resto = n->texto;

        while (*resto != '\0') {
            const char *siguiente;
            int bytes = cortarLinea(resto, &siguiente);
            char pedazo[LARGO_NOTIFICACION];
            snprintf(pedazo, sizeof(pedazo), " %.*s", bytes, resto);
            lineaCelular(pedazo);
            resto = siguiente;
        }
    }

    return 0;
}

// Funcion para la app Notificaciones: muestra la bandeja de la comuna por paginas
// y, al abrirla, las nuevas pasan a leidas
static void appNotificaciones(struct Comuna *comuna) {
    int nuevas = comuna->bandeja.noLeidas;
    comuna->bandeja.noLeidas = 0;
    int pagina = 0;

    while (1) {
        if (!gastarBateria(comuna, GASTO_PAGINA)) {
            printf("Se acabo la bateria.\n");
            return;
        }

        char titulo[64];
        snprintf(titulo, sizeof(titulo), "NOTIFICACIONES  pag. %d", pagina + 1);
        inicioPantalla(comuna, titulo);

        int hayMas = 0;

        if (comuna->bandeja.tope == NULL) {
            lineaCelular("No hay notificaciones.");
        } else {
            hayMas = mostrarPaginaNotificaciones(&comuna->bandeja, pagina, nuevas);
        }

        lineaCelular("");
        lineaCelular(hayMas ? "[S] Sig  [A] Ant  [X] Volver" : "(fin)    [A] Ant  [X] Volver");
        bordeCelular();

        // Pide la opcion hasta que sea valida (asi no se gasta bateria redibujando)
        while (1) {
            char letra = leerLetra("Opcion");

            if (letra == 'x') {
                return;
            }
            if (letra == 's' && hayMas) {
                pagina++;
                break;
            }
            if (letra == 'a' && pagina > 0) {
                pagina--;
                break;
            }
            avisoOpcion(letra);
        }
    }
}

// ---------- Celular ----------

// Funcion para abrir el celular comunitario de una comuna
int abrirCelular(struct Comuna *comuna, struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Catalogo *catalogo, struct Cola *pedidos) {
    if (!gastarBateria(comuna, GASTO_ENCENDER)) {
        printf("El celular de %s no tiene bateria (%d%%). Espere a que el sol lo cargue.\n",
               comuna->nombre, comuna->bateria);
        return 0;
    }

    while (1) {
        pantallaInicio(comuna);
        printf("Opcion: ");

        char linea[100];
        int opcion;

        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            return 0;
        }

        if (sscanf(linea, "%d", &opcion) != 1) {
            continue;
        }

        if (opcion == 0) {
            printf("El celular de %s se apago.\n", comuna->nombre);
            return 0;
        }
        if (opcion == 1) {
            appBiblioteca(comuna, sociedad, tabla, catalogo);
        }
        if (opcion == 2) {
            appAyudaMutua(comuna, pedidos);
        }
        if (opcion == 3) {
            appNotificaciones(comuna);
        }
        if (opcion == 4) {
            mostrarMeme();
        }
        if (opcion == 5) {
            explosion();
            return 1;
        }
    }
}
