// Evita que este archivo se incluya dos veces
#ifndef NOTIFICACIONES_H
#define NOTIFICACIONES_H

// Cuantas notificaciones guarda cada comuna (las mas viejas se descartan)
#define NOTIFICACIONES_MAXIMAS 30
// Largo maximo del texto de una notificacion
#define LARGO_NOTIFICACION 100

// Se declara sin definirla: Comunas.h incluye este archivo
struct Comuna;

// Nodo de la pila de notificaciones
struct Notificacion {
    int dia;  // Dia en que ocurrio
    char texto[LARGO_NOTIFICACION];  // Lo que paso
    struct Notificacion *siguiente;  // La notificacion anterior (mas vieja)
};

// Bandeja de notificaciones de una comuna. Es una PILA (LIFO): la mas reciente
// queda en el tope, porque en un celular lo primero que se ve es lo ultimo que llego.
struct Bandeja {
    struct Notificacion *tope;  // La notificacion mas reciente
    int cantidad;  // Cuantas hay guardadas
    int noLeidas;  // Cuantas llegaron desde la ultima vez que se abrio la app
};

// Funcion para dejar la bandeja vacia
void iniciarBandeja(struct Bandeja *bandeja);
// Funcion para apilar una notificacion nueva en el tope. Si se pasa de
// NOTIFICACIONES_MAXIMAS, descarta la mas vieja (la del fondo).
void apilarNotificacion(struct Bandeja *bandeja, int dia, const char *texto);
// Funcion para sacar la notificacion del tope (NULL si esta vacia). Quien llama la libera.
struct Notificacion *desapilarNotificacion(struct Bandeja *bandeja);
// Funcion para liberar todas las notificaciones de la bandeja
void liberarBandeja(struct Bandeja *bandeja);

// Funcion para fijar el dia actual (se llama al empezar cada dia)
void fijarDiaNotificaciones(int dia);
// Funcion para avisarle algo a una comuna: arma el texto como printf y lo apila
// en su bandeja con el dia actual
void notificar(struct Comuna *comuna, const char *formato, ...);

// Fin de la proteccion contra inclusion doble
#endif
