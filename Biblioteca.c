#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include "Biblioteca.h"

// Tamano maximo de una linea al leer un libro
#define LARGO_LINEA 4096

// ---------- Auxiliares ----------

// Funcion para pasar un texto a minusculas (las tildes quedan igual)
static void aMinusculas(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = (char) tolower((unsigned char) texto[i]);
    }
}

// Funcion para saber si un nombre de archivo termina en .txt
static int terminaEnTxt(const char *nombre) {
    size_t largo = strlen(nombre);

    if (largo < 4) {
        return 0;
    }

    char extension[5];
    snprintf(extension, sizeof(extension), "%s", nombre + largo - 4);
    aMinusculas(extension);

    return strcmp(extension, ".txt") == 0;
}

// Funcion para armar la ruta de un libro: carpeta/nombre
// (la barra / sirve tanto en Linux como en Windows)
static void armarRuta(char *ruta, size_t tamano, const char *carpeta, const char *nombre) {
    snprintf(ruta, tamano, "%s/%s", carpeta, nombre);
}

// ---------- Catalogo (AVL) ----------

// Funcion para leer los nombres de los .txt de una carpeta y meterlos en el AVL
int cargarCatalogo(struct Catalogo *catalogo, const char *carpeta) {
    catalogo->libros = NULL;
    catalogo->cantidadLibros = 0;
    snprintf(catalogo->carpeta, sizeof(catalogo->carpeta), "%s", carpeta);

    // Abre la carpeta para recorrer sus archivos
    DIR *directorio = opendir(carpeta);

    if (directorio == NULL) {
        return 0;
    }

    struct dirent *entrada;

    while ((entrada = readdir(directorio)) != NULL) {
        const char *nombre = entrada->d_name;

        // Ignora ".", ".." y los archivos ocultos
        if (nombre[0] == '.') {
            continue;
        }

        // Solo libros .txt con nombres que quepan en el nodo del AVL
        if (!terminaEnTxt(nombre) || strlen(nombre) >= sizeof(catalogo->libros->nombre)) {
            continue;
        }

        // Revisa que sea un archivo y no una subcarpeta
        char ruta[1024];
        struct stat informacion;
        armarRuta(ruta, sizeof(ruta), carpeta, nombre);

        if (stat(ruta, &informacion) != 0 || !S_ISREG(informacion.st_mode)) {
            continue;
        }

        catalogo->libros = insertarAvl(catalogo->libros, nombre);
        catalogo->cantidadLibros++;
    }

    closedir(directorio);
    return catalogo->cantidadLibros;
}

// Funcion para liberar el AVL del catalogo
void liberarCatalogo(struct Catalogo *catalogo) {
    liberarAvl(catalogo->libros);
    catalogo->libros = NULL;
    catalogo->cantidadLibros = 0;
}

// Funcion para buscar un libro por nombre exacto en el AVL
int existeLibro(const struct Catalogo *catalogo, const char *nombre) {
    return buscarAvl(catalogo->libros, nombre) != NULL;
}

// ---------- Busqueda de texto ----------

// Funcion para revisar si un libro contiene el texto (ya en minusculas).
// Lee linea por linea y deja de leer apenas lo encuentra.
static int libroContiene(const char *ruta, const char *texto) {
    FILE *archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        return 0;
    }

    char linea[LARGO_LINEA];

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        aMinusculas(linea);

        if (strstr(linea, texto) != NULL) {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

// Funcion para recorrer el AVL en orden (alfabetico) revisando cada libro.
// Los libros que contienen el texto se agregan a la lista de resultados.
static void buscarEnLibros(const char *carpeta, struct NodoAvl *nodo, const char *texto, struct LibroEncontrado **libros, int *cantidad) {
    if (nodo == NULL) {
        return;
    }

    buscarEnLibros(carpeta, nodo->izquierdo, texto, libros, cantidad);

    char ruta[1024];
    armarRuta(ruta, sizeof(ruta), carpeta, nodo->nombre);

    if (libroContiene(ruta, texto)) {
        *libros = agregarLibroEncontrado(*libros, nodo->nombre);
        (*cantidad)++;
    }

    buscarEnLibros(carpeta, nodo->derecho, texto, libros, cantidad);
}

// Funcion para buscar un texto en los libros, usando el splay como cache
struct NodoSplay *buscarTexto(const struct Catalogo *catalogo, struct NodoSplay *cache, const char *texto, int *fueAcierto) {
    *fueAcierto = 0;

    // El texto se guarda en minusculas: "Amor" y "amor" son la misma busqueda
    char clave[256];
    snprintf(clave, sizeof(clave), "%s", texto);
    aMinusculas(clave);

    // Un texto vacio no se busca
    if (clave[0] == '\0') {
        return cache;
    }

    // 1. Buscar en la cache: si estaba, queda en la raiz
    cache = buscarSplay(cache, clave);

    if (cache != NULL && strcmp(cache->texto, clave) == 0) {
        *fueAcierto = 1;
        return cache;
    }

    // 2. No estaba: leer los libros y guardar el resultado en la cache
    struct LibroEncontrado *libros = NULL;
    int cantidad = 0;
    buscarEnLibros(catalogo->carpeta, catalogo->libros, clave, &libros, &cantidad);

    return insertarSplay(cache, clave, libros, cantidad);
}
