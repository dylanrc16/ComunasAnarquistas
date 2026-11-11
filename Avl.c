#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Avl.h"

// ---------- Auxiliares de altura ----------

// Funcion para obtener la altura de un nodo (un arbol vacio mide 0)
static int altura(struct NodoAvl *nodo) {
    if (nodo == NULL) {
        return 0;
    }
    return nodo->altura;
}

// Funcion para devolver el mayor de dos enteros
static int mayor(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

// Funcion para recalcular la altura de un nodo a partir de sus hijos
static void actualizarAltura(struct NodoAvl *nodo) {
    nodo->altura = 1 + mayor(altura(nodo->izquierdo), altura(nodo->derecho));
}

// Funcion para calcular el factor de balance: altura izquierda - altura derecha.
// En un AVL debe ser -1, 0 o 1. Si es 2 o -2, hay que rotar.
static int factorBalance(struct NodoAvl *nodo) {
    return altura(nodo->izquierdo) - altura(nodo->derecho);
}

// ---------- Rotaciones ----------

/* Funcion para rotar a la derecha. Se usa cuando el lado izquierdo esta muy alto.

          y                x
         / \              / \
        x   C    --->    A   y
       / \                  / \
      A   B                B   C
*/
static struct NodoAvl *rotarDerecha(struct NodoAvl *y) {
    struct NodoAvl *x = y->izquierdo;
    struct NodoAvl *b = x->derecho;

    // x sube y y baja a su derecha; B cambia de padre
    x->derecho = y;
    y->izquierdo = b;

    // Primero y, porque ahora esta abajo de x
    actualizarAltura(y);
    actualizarAltura(x);

    return x;
}

/* Funcion para rotar a la izquierda. Es el espejo de rotarDerecha.

        x                    y
       / \                  / \
      A   y      --->      x   C
         / \              / \
        B   C            A   B
*/
static struct NodoAvl *rotarIzquierda(struct NodoAvl *x) {
    struct NodoAvl *y = x->derecho;
    struct NodoAvl *b = y->izquierdo;

    // y sube y x baja a su izquierda; B cambia de padre
    y->izquierdo = x;
    x->derecho = b;

    actualizarAltura(x);
    actualizarAltura(y);

    return y;
}

// Funcion para balancear un nodo despues de insertar. Revisa los cuatro casos:
//   Izquierda-Izquierda: una rotacion a la derecha
//   Derecha-Derecha:     una rotacion a la izquierda
//   Izquierda-Derecha:   primero izquierda en el hijo, luego derecha
//   Derecha-Izquierda:   primero derecha en el hijo, luego izquierda
static struct NodoAvl *balancear(struct NodoAvl *nodo) {
    actualizarAltura(nodo);
    int balance = factorBalance(nodo);

    // Lado izquierdo muy alto
    if (balance > 1) {
        // Caso Izquierda-Derecha: el hijo esta inclinado al otro lado
        if (factorBalance(nodo->izquierdo) < 0) {
            nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
        }
        return rotarDerecha(nodo);
    }

    // Lado derecho muy alto
    if (balance < -1) {
        // Caso Derecha-Izquierda: el hijo esta inclinado al otro lado
        if (factorBalance(nodo->derecho) > 0) {
            nodo->derecho = rotarDerecha(nodo->derecho);
        }
        return rotarIzquierda(nodo);
    }

    // Ya esta balanceado
    return nodo;
}

// ---------- Funciones publicas ----------

// Funcion para crear un nodo nuevo (una hoja mide 1)
static struct NodoAvl *crearNodoAvl(const char *nombre) {
    struct NodoAvl *nuevo = calloc(1, sizeof(struct NodoAvl));

    if (nuevo == NULL) {
        return NULL;
    }

    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", nombre);
    nuevo->altura = 1;
    return nuevo;
}

// Funcion para insertar un nombre. Baja como en un arbol binario de busqueda
// y, al volver de la recursion, balancea cada nodo del camino.
// Los nombres repetidos se ignoran.
struct NodoAvl *insertarAvl(struct NodoAvl *raiz, const char *nombre) {
    if (nombre == NULL) {
        return raiz;
    }

    // Llego al lugar vacio: aqui va el nodo nuevo
    if (raiz == NULL) {
        return crearNodoAvl(nombre);
    }

    int comparacion = strcmp(nombre, raiz->nombre);

    if (comparacion < 0) {
        raiz->izquierdo = insertarAvl(raiz->izquierdo, nombre);
    } else if (comparacion > 0) {
        raiz->derecho = insertarAvl(raiz->derecho, nombre);
    } else {
        // Ya existe: no se inserta dos veces
        return raiz;
    }

    // Al volver, este nodo pudo quedar desbalanceado
    return balancear(raiz);
}

// Funcion para buscar un nombre exacto. Cada comparacion descarta la mitad del arbol.
struct NodoAvl *buscarAvl(struct NodoAvl *raiz, const char *nombre) {
    struct NodoAvl *actual = raiz;

    while (actual != NULL) {
        int comparacion = strcmp(nombre, actual->nombre);

        if (comparacion == 0) {
            return actual;
        }

        if (comparacion < 0) {
            actual = actual->izquierdo;
        } else {
            actual = actual->derecho;
        }
    }

    return NULL;
}

// Funcion para contar los nodos del arbol
int contarAvl(struct NodoAvl *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contarAvl(raiz->izquierdo) + contarAvl(raiz->derecho);
}

// Funcion para obtener la altura del arbol completo
int alturaAvl(struct NodoAvl *raiz) {
    return altura(raiz);
}

// Funcion para imprimir el arbol de lado: la raiz a la izquierda,
// los mayores arriba y los menores abajo. Se llama con nivel 0.
void imprimirAvl(struct NodoAvl *raiz, int nivel) {
    if (raiz == NULL) {
        return;
    }

    imprimirAvl(raiz->derecho, nivel + 1);

    for (int i = 0; i < nivel; i++) {
        printf("        ");
    }
    printf("%s (h=%d)\n", raiz->nombre, raiz->altura);

    imprimirAvl(raiz->izquierdo, nivel + 1);
}

// Funcion para liberar el arbol: primero los hijos y despues el nodo
void liberarAvl(struct NodoAvl *raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarAvl(raiz->izquierdo);
    liberarAvl(raiz->derecho);
    free(raiz);
}
