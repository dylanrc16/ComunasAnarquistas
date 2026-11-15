// Evita que este archivo se incluya dos veces
#ifndef COLA_H
#define COLA_H

#include "Comunas.h"

// Nodo de la cola: un pedido de ayuda de una comuna por un bien
struct Pedido {
    struct Comuna *comuna;  // Comuna que pide (no es una copia)
    char bien[50];  // Bien que necesita
    int diasEsperando;  // Dias que lleva en la cola sin ser atendido
    struct Pedido *siguiente;  // Pedido que llego despues
};

// Cola FIFO de pedidos: el primero que llega es el primero que se atiende.
// Se guarda el frente (por donde se atiende) y el final (por donde se encola),
// asi encolar y desencolar cuestan O(1), sin recorrer la cola.
struct Cola {
    struct Pedido *frente;  // Primer pedido (el siguiente en atenderse)
    struct Pedido *final;  // Ultimo pedido (el mas reciente)
    int cantidad;  // Cuantos pedidos hay
};

// Funcion para dejar la cola vacia
void iniciarCola(struct Cola *cola);
// Funcion para saber si la cola esta vacia (1 si, 0 no)
int colaVacia(const struct Cola *cola);
// Funcion para saber si una comuna ya pidio un bien (1 si, 0 no)
int existePedido(const struct Cola *cola, const struct Comuna *comuna, const char *bien);
// Funcion para agregar un pedido nuevo al final de la cola.
// Devuelve 1 si lo agrego y 0 si esa comuna ya habia pedido ese bien.
int encolarPedido(struct Cola *cola, struct Comuna *comuna, const char *bien);
// Funcion para sacar el pedido del frente. Devuelve el nodo (quien llama decide
// si lo libera o lo vuelve a encolar) o NULL si la cola esta vacia.
struct Pedido *desencolarPedido(struct Cola *cola);
// Funcion para volver a poner al final un pedido que se saco con desencolarPedido
void reencolarPedido(struct Cola *cola, struct Pedido *pedido);
// Funcion para cancelar el pedido de una comuna por un bien.
// Devuelve 1 si estaba y lo quito, 0 si no estaba.
int cancelarPedido(struct Cola *cola, const struct Comuna *comuna, const char *bien);
// Funcion para quitar todos los pedidos de una comuna (cuando se disuelve).
// Devuelve cuantos quito.
int quitarPedidosComuna(struct Cola *cola, const struct Comuna *comuna);
// Funcion para liberar todos los pedidos (no libera las comunas)
void liberarCola(struct Cola *cola);

// Fin de la proteccion contra inclusion doble
#endif
