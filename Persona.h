#ifndef PERSONA_H
#define PERSONA_H

struct Persona {
    char nombre[50];
    struct Persona *siguiente;
};

void liberarPersonas(struct Persona *inicio);
struct Persona *crearPersona(const char *nombre);
struct Persona *agregarPersonaFinal(struct Persona *inicio, const char *nombre);
struct Persona *cargarPersonas(void);

#endif