#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cola.h"

// Funcion para dejar la cola vacia
void iniciarCola(struct Cola *cola) {
    cola->frente = NULL;
    cola->final = NULL;
    cola->cantidad = 0;
}

// Funcion para saber si la cola esta vacia
int colaVacia(const struct Cola *cola) {
    return cola->frente == NULL;
}

// Funcion para saber si una comuna ya pidio un bien: recorre del frente al final
int existePedido(const struct Cola *cola, const struct Comuna *comuna, const char *bien) {
    for (struct Pedido *actual = cola->frente; actual != NULL; actual = actual->siguiente) {
        if (actual->comuna == comuna && strcmp(actual->bien, bien) == 0) {
            return 1;
        }
    }

    return 0;
}

// Funcion para poner un nodo al final de la cola en O(1), gracias al puntero final
void reencolarPedido(struct Cola *cola, struct Pedido *pedido) {
    pedido->siguiente = NULL;

    // Cola vacia: el nodo es frente y final a la vez
    if (cola->final == NULL) {
        cola->frente = pedido;
    } else {
        cola->final->siguiente = pedido;
    }

    cola->final = pedido;
    cola->cantidad++;
}

// Funcion para crear un pedido nuevo y encolarlo al final
int encolarPedido(struct Cola *cola, struct Comuna *comuna, const char *bien) {
    if (comuna == NULL || bien == NULL || existePedido(cola, comuna, bien)) {
        return 0;
    }

    struct Pedido *nuevo = calloc(1, sizeof(struct Pedido));

    if (nuevo == NULL) {
        return 0;
    }

    nuevo->comuna = comuna;
    snprintf(nuevo->bien, sizeof(nuevo->bien), "%s", bien);
    nuevo->diasEsperando = 0;

    reencolarPedido(cola, nuevo);
    return 1;
}

// Funcion para sacar el pedido del frente en O(1)
struct Pedido *desencolarPedido(struct Cola *cola) {
    if (cola->frente == NULL) {
        return NULL;
    }

    struct Pedido *pedido = cola->frente;
    cola->frente = pedido->siguiente;

    // Si era el unico, la cola queda vacia y el final tambien es NULL
    if (cola->frente == NULL) {
        cola->final = NULL;
    }

    pedido->siguiente = NULL;
    cola->cantidad--;
    return pedido;
}

// Funcion para quitar de la cola los pedidos que cumplen una condicion, usando solo
// operaciones de cola: se da una vuelta completa desencolando cada pedido y
// reencolando los que se quedan. Asi los que se quedan conservan su orden.
// Si bien es NULL, quita todos los de la comuna; si no, solo ese bien.
static int quitarPedidos(struct Cola *cola, const struct Comuna *comuna, const char *bien) {
    int vuelta = cola->cantidad;
    int quitados = 0;

    for (int i = 0; i < vuelta; i++) {
        struct Pedido *pedido = desencolarPedido(cola);

        if (pedido->comuna == comuna && (bien == NULL || strcmp(pedido->bien, bien) == 0)) {
            free(pedido);
            quitados++;
        } else {
            reencolarPedido(cola, pedido);
        }
    }

    return quitados;
}

// Funcion para cancelar el pedido de una comuna por un bien
int cancelarPedido(struct Cola *cola, const struct Comuna *comuna, const char *bien) {
    return quitarPedidos(cola, comuna, bien) > 0;
}

// Funcion para quitar todos los pedidos de una comuna
int quitarPedidosComuna(struct Cola *cola, const struct Comuna *comuna) {
    return quitarPedidos(cola, comuna, NULL);
}

// Funcion para liberar todos los pedidos de la cola
void liberarCola(struct Cola *cola) {
    while (!colaVacia(cola)) {
        free(desencolarPedido(cola));
    }
}
