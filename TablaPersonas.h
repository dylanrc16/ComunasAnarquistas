// Evita que este archivo se incluya dos veces
#ifndef TABLAPERSONAS_H
#define TABLAPERSONAS_H

#include "Persona.h"

// Cantidad de casillas: un numero primo cercano a 500 personas,
// asi en promedio queda una persona por casilla
#define TAMANO_TABLA 509

// Tabla hash de personas. Las colisiones se resuelven por encadenamiento:
// cada casilla tiene una lista simple de las personas que caen ahi.
struct TablaPersonas {
    struct Persona *casillas[TAMANO_TABLA];  // Inicio de la lista de cada casilla
    int cantidad;  // Cuantas personas hay en total
};

// Funcion para dejar la tabla vacia (todas las casillas en NULL)
void iniciarTablaPersonas(struct TablaPersonas *tabla);
// Funcion para calcular en que casilla va un nombre (funcion hash djb2)
int calcularCasilla(const char *nombre);
// Funcion para insertar una persona. Si el nombre ya existe, le agrega
// " 2", " 3"... para que sea unico. Devuelve la persona creada (o NULL).
struct Persona *insertarPersonaTabla(struct TablaPersonas *tabla, const char *nombre);
// Funcion para buscar una persona por nombre exacto (NULL si no esta)
struct Persona *buscarPersonaTabla(const struct TablaPersonas *tabla, const char *nombre);
// Funcion para cargar las personas de un archivo (una por linea).
// Devuelve cuantas cargo (0 si no pudo abrir el archivo).
int cargarTablaPersonas(struct TablaPersonas *tabla, const char *nombreArchivo);
// Funcion para mostrar como quedo repartida la tabla (casillas usadas y colisiones)
void imprimirEstadisticasTabla(const struct TablaPersonas *tabla);
// Funcion para liberar todas las personas de la tabla (con sus servicios)
void liberarTablaPersonas(struct TablaPersonas *tabla);

// Fin de la proteccion contra inclusion doble
#endif
