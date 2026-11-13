// Evita que este archivo se incluya dos veces
#ifndef PERSONA_H
#define PERSONA_H

// Para la lista de libros leidos se reutiliza la lista de libros del splay
#include "Splay.h"

// Libros que hay que leer para aprender un servicio (lo pide el PDF)
#define LIBROS_PARA_APRENDER 3

// Se declara sin definirla para poder guardar un puntero a la comuna.
// La definicion completa esta en Comunas.h (y Comunas.h no se puede incluir
// aqui, porque las comunas tambien van a necesitar a las personas).
struct Comuna;

// Nodo de la lista simple de servicios que sabe hacer una persona
struct Servicio {
    char nombre[50];  // Nombre del servicio (igual al del archivo servicios)
    struct Servicio *siguiente;  // Siguiente servicio de la lista
};

// Nodo de persona. El puntero siguiente sirve para la lista y para la tabla hash
struct Persona {
    char nombre[50];  // Nombre de la persona (unico en la tabla hash)
    struct Comuna *comuna;  // Comuna donde vive (NULL si todavia no tiene)
    struct Servicio *servicios;  // Servicios que sabe hacer (NULL si ninguno)
    char servicioEnEstudio[50];  // Servicio que esta aprendiendo ("" si ninguno)
    int librosParaServicio;  // Libros leidos para ese servicio (aprende al llegar a 3)
    struct LibroEncontrado *librosLeidos;  // Todos los libros que ha leido
    struct Persona *siguiente;  // Apunta a la siguiente persona
};

// Funcion para liberar toda la lista de personas
void liberarPersonas(struct Persona *inicio);
// Funcion para crear una persona nueva
struct Persona *crearPersona(const char *nombre);
// Funcion para agregar una persona al final
struct Persona *agregarPersonaFinal(struct Persona *inicio, const char *nombre);
// Funcion para cargar las personas del archivo
struct Persona *cargarPersonas(void);

// Funcion para que una persona aprenda un servicio.
// Devuelve 1 si lo aprendio y 0 si ya lo sabia.
int aprenderServicio(struct Persona *persona, const char *servicio);
// Funcion para saber si una persona sabe un servicio (1 si, 0 no)
int sabeServicio(const struct Persona *persona, const char *servicio);
// Funcion para contar cuantos servicios sabe una persona
int contarServicios(const struct Persona *persona);
// Funcion para liberar una lista de servicios
void liberarServicios(struct Servicio *inicio);

// Funcion para saber si una persona ya leyo un libro (1 si, 0 no)
int yaLeyo(const struct Persona *persona, const char *libro);
// Funcion para que una persona lea un libro nuevo. Suma 1 a los libros de su
// servicio en estudio y, al llegar a 3, lo aprende.
// Devuelve 1 si con esta lectura aprendio el servicio y 0 si no.
int leerLibro(struct Persona *persona, const char *libro);

// Fin de la proteccion contra inclusion doble
#endif