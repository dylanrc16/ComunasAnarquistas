// Evita que este archivo se incluya dos veces
#ifndef AVL_H
#define AVL_H

// Nodo del arbol AVL de nombres de libros (archivos del corpus)
struct NodoAvl {
    char nombre[256];  // Nombre del archivo, es la clave de busqueda
    int altura;  // Altura del nodo, sirve para revisar el balance
    struct NodoAvl *izquierdo;  // Nombres menores alfabeticamente
    struct NodoAvl *derecho;  // Nombres mayores alfabeticamente
};

// Funcion para insertar un nombre; devuelve la nueva raiz (puede cambiar al rotar)
struct NodoAvl *insertarAvl(struct NodoAvl *raiz, const char *nombre);
// Funcion para buscar un nombre exacto; devuelve el nodo o NULL
struct NodoAvl *buscarAvl(struct NodoAvl *raiz, const char *nombre);
// Funcion para contar cuantos nombres hay en el arbol
int contarAvl(struct NodoAvl *raiz);
// Funcion para obtener la altura del arbol
int alturaAvl(struct NodoAvl *raiz);
// Funcion para imprimir el arbol de lado, para ver su forma
void imprimirAvl(struct NodoAvl *raiz, int nivel);
// Funcion para liberar todo el arbol
void liberarAvl(struct NodoAvl *raiz);

// Fin de la proteccion contra inclusion doble
#endif
