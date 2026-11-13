#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Celular.h"

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

// Funcion para dibujar una linea de texto dentro de la pantalla
static void lineaCelular(const char *texto) {
    printf("   | %-*.*s |\n", ANCHO_PANTALLA, ANCHO_PANTALLA, texto);
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
    lineaCelular("  3. Notificaciones");
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

// ---------- Celular ----------

// Funcion para abrir el celular comunitario de una comuna
int abrirCelular(struct Comuna *comuna) {
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
        if (opcion >= 1 && opcion <= 3) {
            printf("Esta app todavia se esta instalando...\n");
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
