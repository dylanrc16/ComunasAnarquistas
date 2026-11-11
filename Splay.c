#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Splay.h"

// Si vale 1, el splay imprime cada zig, zig-zig o zig-zag que hace
static int mostrarRotaciones = 0;

// ---------- Lista de libros encontrados ----------

// Funcion para agregar un libro al final de una lista de resultados
struct LibroEncontrado *agregarLibroEncontrado(struct LibroEncontrado *inicio, const char *nombre) {
    struct LibroEncontrado *nuevo = calloc(1, sizeof(struct LibroEncontrado));

    if (nuevo == NULL) {
        return inicio;
    }

    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", nombre);

    if (inicio == NULL) {
        return nuevo;
    }

    struct LibroEncontrado *actual = inicio;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nuevo;
    return inicio;
}

// Funcion para liberar una lista de resultados
void liberarLibrosEncontrados(struct LibroEncontrado *inicio) {
    while (inicio != NULL) {
        struct LibroEncontrado *siguiente = inicio->siguiente;
        free(inicio);
        inicio = siguiente;
    }
}

// ---------- Rotaciones ----------

// Funcion para rotar a la derecha: el hijo izquierdo sube y el nodo baja a su derecha
static struct NodoSplay *rotarDerecha(struct NodoSplay *nodo) {
    struct NodoSplay *hijo = nodo->izquierdo;
    nodo->izquierdo = hijo->derecho;
    hijo->derecho = nodo;
    return hijo;
}

// Funcion para rotar a la izquierda: el hijo derecho sube y el nodo baja a su izquierda
static struct NodoSplay *rotarIzquierda(struct NodoSplay *nodo) {
    struct NodoSplay *hijo = nodo->derecho;
    nodo->derecho = hijo->izquierdo;
    hijo->izquierdo = nodo;
    return hijo;
}

// ---------- Chaflaneo ----------

// Funcion para chaflanear de abajo hacia arriba, igual que en papel:
//   1. Baja desde la raiz hasta el texto (o hasta el ultimo nodo, si no esta)
//      y guarda el camino recorrido.
//   2. Desde ese nodo sube hacia la raiz, de dos en dos niveles:
//      - Zig:     el padre es la raiz -> una rotacion
//      - Zig-zig: nodo y padre van al mismo lado -> rota el abuelo y luego el padre
//      - Zig-zag: nodo y padre van a lados distintos -> rota el padre y luego el abuelo
struct NodoSplay *splay(struct NodoSplay *raiz, const char *texto) {
    if (raiz == NULL || texto == NULL) {
        return raiz;
    }

    // Primero mide cuantos nodos tiene el camino, para pedir la memoria justa
    int largo = 0;
    struct NodoSplay *actual = raiz;

    while (actual != NULL) {
        largo++;
        int comparacion = strcmp(texto, actual->texto);

        if (comparacion == 0) {
            break;
        }

        if (comparacion < 0) {
            if (actual->izquierdo == NULL) {
                break;
            }
            actual = actual->izquierdo;
        } else {
            if (actual->derecho == NULL) {
                break;
            }
            actual = actual->derecho;
        }
    }

    // Guarda el camino: camino[0] es la raiz y camino[k] el nodo que va a subir
    struct NodoSplay **camino = malloc(largo * sizeof(struct NodoSplay *));

    if (camino == NULL) {
        return raiz;
    }

    actual = raiz;

    for (int i = 0; i < largo; i++) {
        camino[i] = actual;

        if (strcmp(texto, actual->texto) < 0) {
            actual = actual->izquierdo;
        } else {
            actual = actual->derecho;
        }
    }

    int k = largo - 1;
    struct NodoSplay *x = camino[k];

    // Sube x mientras no sea la raiz
    while (k > 0) {
        struct NodoSplay *padre = camino[k - 1];

        // Zig: el padre es la raiz
        if (k == 1) {
            if (mostrarRotaciones) {
                printf("   zig: %s sube sobre %s\n", x->texto, padre->texto);
            }

            if (padre->izquierdo == x) {
                raiz = rotarDerecha(padre);
            } else {
                raiz = rotarIzquierda(padre);
            }
            break;
        }

        struct NodoSplay *abuelo = camino[k - 2];
        int xEsIzquierdo = (padre->izquierdo == x);
        int padreEsIzquierdo = (abuelo->izquierdo == padre);
        struct NodoSplay *nuevoSubarbol;

        if (xEsIzquierdo && padreEsIzquierdo) {
            // Zig-zig izquierda-izquierda: primero el abuelo, luego el padre
            if (mostrarRotaciones) {
                printf("   zig-zig: %s sube sobre %s y %s\n", x->texto, padre->texto, abuelo->texto);
            }
            nuevoSubarbol = rotarDerecha(abuelo);
            nuevoSubarbol = rotarDerecha(nuevoSubarbol);
        } else if (!xEsIzquierdo && !padreEsIzquierdo) {
            // Zig-zig derecha-derecha: primero el abuelo, luego el padre
            if (mostrarRotaciones) {
                printf("   zig-zig: %s sube sobre %s y %s\n", x->texto, padre->texto, abuelo->texto);
            }
            nuevoSubarbol = rotarIzquierda(abuelo);
            nuevoSubarbol = rotarIzquierda(nuevoSubarbol);
        } else if (!xEsIzquierdo && padreEsIzquierdo) {
            // Zig-zag izquierda-derecha: primero el padre, luego el abuelo
            if (mostrarRotaciones) {
                printf("   zig-zag: %s sube sobre %s y %s\n", x->texto, padre->texto, abuelo->texto);
            }
            abuelo->izquierdo = rotarIzquierda(padre);
            nuevoSubarbol = rotarDerecha(abuelo);
        } else {
            // Zig-zag derecha-izquierda: primero el padre, luego el abuelo
            if (mostrarRotaciones) {
                printf("   zig-zag: %s sube sobre %s y %s\n", x->texto, padre->texto, abuelo->texto);
            }
            abuelo->derecho = rotarDerecha(padre);
            nuevoSubarbol = rotarIzquierda(abuelo);
        }

        // x quedo donde estaba el abuelo: hay que engancharlo al bisabuelo
        if (k == 2) {
            raiz = nuevoSubarbol;
        } else {
            struct NodoSplay *bisabuelo = camino[k - 3];

            if (bisabuelo->izquierdo == abuelo) {
                bisabuelo->izquierdo = nuevoSubarbol;
            } else {
                bisabuelo->derecho = nuevoSubarbol;
            }
        }

        // x subio dos niveles
        k -= 2;
        camino[k] = x;
    }

    free(camino);
    return raiz;
}

// Funcion para buscar en la cache: chaflanea con el texto buscado.
// Quien llama revisa si la raiz nueva tiene ese texto (acierto) o no (fallo).
struct NodoSplay *buscarSplay(struct NodoSplay *raiz, const char *texto) {
    return splay(raiz, texto);
}

// Funcion para crear un nodo de la cache con su resultado
static struct NodoSplay *crearNodoSplay(const char *texto, struct LibroEncontrado *libros, int cantidadLibros) {
    struct NodoSplay *nuevo = calloc(1, sizeof(struct NodoSplay));

    if (nuevo == NULL) {
        return NULL;
    }

    snprintf(nuevo->texto, sizeof(nuevo->texto), "%s", texto);
    nuevo->libros = libros;
    nuevo->cantidadLibros = cantidadLibros;
    return nuevo;
}

// Funcion para guardar un resultado en la cache. La lista de libros pasa a ser
// del arbol (se libera con liberarSplay). El nodo nuevo queda en la raiz:
//   1. Se chaflanea con el texto: arriba queda el nodo mas parecido.
//   2. Ese nodo se parte en dos: lo menor queda a la izquierda del nuevo
//      y lo mayor a la derecha.
struct NodoSplay *insertarSplay(struct NodoSplay *raiz, const char *texto, struct LibroEncontrado *libros, int cantidadLibros) {
    if (texto == NULL) {
        return raiz;
    }

    if (raiz == NULL) {
        return crearNodoSplay(texto, libros, cantidadLibros);
    }

    raiz = splay(raiz, texto);
    int comparacion = strcmp(texto, raiz->texto);

    // Si ya estaba, se actualiza su resultado
    if (comparacion == 0) {
        liberarLibrosEncontrados(raiz->libros);
        raiz->libros = libros;
        raiz->cantidadLibros = cantidadLibros;
        return raiz;
    }

    struct NodoSplay *nuevo = crearNodoSplay(texto, libros, cantidadLibros);

    if (nuevo == NULL) {
        liberarLibrosEncontrados(libros);
        return raiz;
    }

    if (comparacion < 0) {
        // La raiz y todo lo mayor quedan a la derecha del nuevo
        nuevo->izquierdo = raiz->izquierdo;
        nuevo->derecho = raiz;
        raiz->izquierdo = NULL;
    } else {
        // La raiz y todo lo menor quedan a la izquierda del nuevo
        nuevo->derecho = raiz->derecho;
        nuevo->izquierdo = raiz;
        raiz->derecho = NULL;
    }

    return nuevo;
}

// Funcion para activar (1) o desactivar (0) que se impriman las rotaciones
void mostrarRotacionesSplay(int activar) {
    mostrarRotaciones = activar;
}

// Funcion para contar cuantos textos hay en la cache
int contarSplay(struct NodoSplay *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contarSplay(raiz->izquierdo) + contarSplay(raiz->derecho);
}

// Funcion para imprimir el arbol de lado: la raiz a la izquierda,
// los mayores arriba y los menores abajo. Se llama con nivel 0.
void imprimirSplay(struct NodoSplay *raiz, int nivel) {
    if (raiz == NULL) {
        return;
    }

    imprimirSplay(raiz->derecho, nivel + 1);

    for (int i = 0; i < nivel; i++) {
        printf("        ");
    }
    printf("%s (%d libros)\n", raiz->texto, raiz->cantidadLibros);

    imprimirSplay(raiz->izquierdo, nivel + 1);
}

// Funcion para liberar el arbol: primero los hijos, despues la lista y el nodo
void liberarSplay(struct NodoSplay *raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarSplay(raiz->izquierdo);
    liberarSplay(raiz->derecho);
    liberarLibrosEncontrados(raiz->libros);
    free(raiz);
}
