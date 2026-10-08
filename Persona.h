// Evita que este archivo se incluya dos veces
#ifndef PERSONA_H
#define PERSONA_H

// Nodo de la lista simple de personas
struct Persona {
    char nombre[50];  // Nombre de la persona
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

// Fin de la proteccion contra inclusion doble
#endif