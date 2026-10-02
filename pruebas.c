#include <stdio.h>
#include <string.h>
#include "Persona.h"
#include "Recurso.h"
#include "Comunas.h"
#include "Sociedad.h"

static int fallos = 0;

static void verificar(const char *descripcion, int condicion) {
    if (condicion) {
        printf("  [OK]    %s\n", descripcion);
    } else {
        printf("  [FALLO] %s\n", descripcion);
        fallos++;
    }
}

/* ---------- Lista simple de personas ---------- */

static void probarPersonas(void) {
    printf("\n== Lista simple: Persona ==\n");

    struct Persona *lista = NULL;
    lista = agregarPersonaFinal(lista, "Ana");
    lista = agregarPersonaFinal(lista, "Beto");
    lista = agregarPersonaFinal(lista, "Carla");

    int cantidad = 0;
    struct Persona *ultima = NULL;

    for (struct Persona *p = lista; p != NULL; p = p->siguiente) {
        cantidad++;
        ultima = p;
    }

    verificar("agregar 3 personas deja 3 en la lista", cantidad == 3);
    verificar("la primera es Ana", strcmp(lista->nombre, "Ana") == 0);
    verificar("la segunda es Beto", strcmp(lista->siguiente->nombre, "Beto") == 0);
    verificar("la ultima es Carla y apunta a NULL",
              strcmp(ultima->nombre, "Carla") == 0 && ultima->siguiente == NULL);

    liberarPersonas(lista);

    struct Persona *cargadas = cargarPersonas();
    verificar("cargarPersonas lee el archivo personas", cargadas != NULL);

    cantidad = 0;
    for (struct Persona *p = cargadas; p != NULL; p = p->siguiente) {
        cantidad++;
    }
    printf("          (personas cargadas: %d)\n", cantidad);

    liberarPersonas(cargadas);
}

/* ---------- Lista simple de recursos ---------- */

static void probarRecursos(void) {
    printf("\n== Lista simple: Recurso ==\n");

    struct Recurso *lista = NULL;
    lista = agregarRecursoFinal(lista, "Arroz", 10, 20);
    lista = agregarRecursoFinal(lista, "Agua", 5, 30);
    lista = agregarRecursoFinal(lista, "Frijoles", 8, 16);

    int cantidad = 0;
    for (struct Recurso *r = lista; r != NULL; r = r->siguiente) {
        cantidad++;
    }

    verificar("agregar 3 recursos deja 3 en la lista", cantidad == 3);

    struct Recurso *agua = buscarRecurso(lista, "Agua");
    verificar("buscarRecurso encuentra Agua", agua != NULL);
    verificar("Agua tiene existencia 5 y maximo 30",
              agua != NULL && agua->existencia == 5 && agua->maximo == 30);
    verificar("buscarRecurso devuelve NULL si no existe", buscarRecurso(lista, "Oro") == NULL);

    verificar("crearRecurso rechaza nombre NULL", crearRecurso(NULL, 1, 1) == NULL);
    verificar("crearRecurso rechaza existencia negativa", crearRecurso("X", -1, 1) == NULL);

    printf("          (imprimirRecursos)\n");
    imprimirRecursos(lista);

    liberarRecursos(lista);

    struct Recurso *bienes = cargarRecursos("bienes");
    verificar("cargarRecursos lee el archivo bienes", bienes != NULL);
    liberarRecursos(bienes);

    verificar("cargarRecursos devuelve NULL si el archivo no existe",
              cargarRecursos("archivo_que_no_existe") == NULL);
}

/* ---------- Lista doble de comunas ---------- */

static void probarComunas(void) {
    printf("\n== Lista doble: Comuna ==\n");

    struct Comuna *lista = NULL;
    lista = agregarComunaFinal(lista, "A", 100);
    lista = agregarComunaFinal(lista, "B", 200);
    lista = agregarComunaFinal(lista, "C", 300);

    struct Comuna *a = lista;
    struct Comuna *b = a->siguiente;
    struct Comuna *c = b->siguiente;

    verificar("la primera no tiene anterior", a->anterior == NULL);
    verificar("A->siguiente es B", strcmp(a->siguiente->nombre, "B") == 0);
    verificar("B->anterior es A", b->anterior == a);
    verificar("B->siguiente es C", b->siguiente == c);
    verificar("C->anterior es B", c->anterior == b);
    verificar("la ultima no tiene siguiente", c->siguiente == NULL);

    /* recorrido hacia atras, partiendo de la ultima */
    char orden[10] = "";
    for (struct Comuna *x = c; x != NULL; x = x->anterior) {
        strcat(orden, x->nombre);
    }
    verificar("recorrido hacia atras da C, B, A", strcmp(orden, "CBA") == 0);

    verificar("buscarComuna encuentra B", buscarComuna(lista, "B") == b);
    verificar("buscarComuna devuelve NULL si no existe", buscarComuna(lista, "Z") == NULL);
    verificar("crearComuna rechaza nombre NULL", crearComuna(NULL, 10) == NULL);
    verificar("crearComuna rechaza personas negativas", crearComuna("X", -5) == NULL);

    printf("          (imprimirComunas)\n");
    imprimirComunas(lista);

    liberarComunas(lista);

    struct Comuna *cargadas = cargarComunas();
    verificar("cargarComunas lee el archivo comunas", cargadas != NULL);
    liberarComunas(cargadas);
}
