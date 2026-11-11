// Evita que este archivo se incluya dos veces
#ifndef SPLAY_H
#define SPLAY_H

// Nodo de la lista simple de libros donde aparece un texto
struct LibroEncontrado {
    char nombre[256];  // Nombre del archivo del libro
    struct LibroEncontrado *siguiente;  // Siguiente libro de la lista
};

// Nodo del arbol splay que funciona como cache de busquedas de texto
struct NodoSplay {
    char texto[256];  // Texto buscado, es la clave
    struct LibroEncontrado *libros;  // Libros donde aparece (NULL si no aparece en ninguno)
    int cantidadLibros;  // Cuantos libros hay en la lista
    struct NodoSplay *izquierdo;  // Textos menores alfabeticamente
    struct NodoSplay *derecho;  // Textos mayores alfabeticamente
};

// Funcion para agregar un libro al final de una lista de resultados
struct LibroEncontrado *agregarLibroEncontrado(struct LibroEncontrado *inicio, const char *nombre);
// Funcion para liberar una lista de resultados
void liberarLibrosEncontrados(struct LibroEncontrado *inicio);

// Funcion para chaflanear: sube a la raiz el nodo con ese texto
// (o el ultimo nodo visitado, si el texto no esta). Devuelve la nueva raiz.
struct NodoSplay *splay(struct NodoSplay *raiz, const char *texto);
// Funcion para buscar en la cache: chaflanea y devuelve la nueva raiz.
// Si el texto estaba, queda en la raiz; si no, la raiz tiene otro texto.
struct NodoSplay *buscarSplay(struct NodoSplay *raiz, const char *texto);
// Funcion para guardar un resultado en la cache; el nodo nuevo queda en la raiz
struct NodoSplay *insertarSplay(struct NodoSplay *raiz, const char *texto, struct LibroEncontrado *libros, int cantidadLibros);
// Funcion para activar o desactivar que se impriman las rotaciones (para la demostracion)
void mostrarRotacionesSplay(int activar);
// Funcion para contar cuantos textos hay en la cache
int contarSplay(struct NodoSplay *raiz);
// Funcion para imprimir el arbol de lado, para ver como cambia al chaflanear
void imprimirSplay(struct NodoSplay *raiz, int nivel);
// Funcion para liberar todo el arbol y sus listas de libros
void liberarSplay(struct NodoSplay *raiz);

// Fin de la proteccion contra inclusion doble
#endif
