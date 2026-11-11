// Evita que este archivo se incluya dos veces
#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Avl.h"
#include "Splay.h"

// Carpeta que se usa si el usuario presiona Enter
#define CARPETA_CORPUS_POR_DEFECTO "corpus"

// Catalogo de libros: uno solo para todo el programa, compartido por las bibliotecas
struct Catalogo {
    char carpeta[512];  // Carpeta donde estan los libros (.txt)
    struct NodoAvl *libros;  // AVL con los nombres de los libros
    int cantidadLibros;  // Cuantos libros se cargaron
};

// Biblioteca de una comuna: su propia cache de busquedas y sus estadisticas
struct Biblioteca {
    struct NodoSplay *cache;  // Arbol splay con las busquedas ya hechas
    int busquedas;  // Cuantas busquedas de texto se han hecho
    int aciertos;  // Cuantas se respondieron desde la cache
};

// Funcion para leer los nombres de los .txt de una carpeta y meterlos en el AVL.
// Devuelve cuantos libros cargo (0 si la carpeta no existe o no tiene libros).
int cargarCatalogo(struct Catalogo *catalogo, const char *carpeta);
// Funcion para liberar el AVL del catalogo
void liberarCatalogo(struct Catalogo *catalogo);

// Funcion para buscar un libro por nombre exacto en el AVL (1 si existe, 0 si no)
int existeLibro(const struct Catalogo *catalogo, const char *nombre);

// Funcion para buscar un texto en los libros, usando el splay como cache:
//   1. Busca primero en la cache (chaflanea).
//   2. Si no esta, lee los archivos y guarda el resultado en la cache.
// Devuelve la nueva raiz de la cache; el resultado queda en la raiz.
// En *fueAcierto deja 1 si estaba en la cache y 0 si hubo que leer los archivos.
struct NodoSplay *buscarTexto(const struct Catalogo *catalogo, struct NodoSplay *cache, const char *texto, int *fueAcierto);

// Fin de la proteccion contra inclusion doble
#endif
