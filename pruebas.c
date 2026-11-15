#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Persona.h"
#include "Recurso.h"
#include "Comunas.h"
#include "Sociedad.h"
#include "Avl.h"
#include "Splay.h"
#include "TablaPersonas.h"

// Cuenta cuantas verificaciones fallaron
static int fallos = 0;

// Funcion para imprimir [OK] o [FALLO] segun la condicion
static void verificar(const char *descripcion, int condicion) {
    if (condicion) {
        printf("  [OK]    %s\n", descripcion);
    } else {
        printf("  [FALLO] %s\n", descripcion);
        fallos++;
    }
}

// ---------- Lista simple de personas ----------

// Funcion para probar la lista simple de personas
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

    // Prueba la carga desde el archivo personas
    struct Persona *cargadas = cargarPersonas();
    verificar("cargarPersonas lee el archivo personas", cargadas != NULL);

    cantidad = 0;
    for (struct Persona *p = cargadas; p != NULL; p = p->siguiente) {
        cantidad++;
    }
    printf("          (personas cargadas: %d)\n", cantidad);

    liberarPersonas(cargadas);
}

// ---------- Lista simple de recursos ----------

// Funcion para probar la lista simple de recursos
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

    // Prueba la carga desde archivo y el caso de archivo inexistente
    struct Recurso *bienes = cargarRecursos("bienes");
    verificar("cargarRecursos lee el archivo bienes", bienes != NULL);
    liberarRecursos(bienes);

    verificar("cargarRecursos devuelve NULL si el archivo no existe",
              cargarRecursos("archivo_que_no_existe") == NULL);
}

// ---------- Lista doble de comunas ----------

// Funcion para probar la lista doble de comunas
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

    // Recorrido hacia atras, partiendo de la ultima
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

    // Prueba la carga desde el archivo comunas
    struct Comuna *cargadas = cargarComunas();
    verificar("cargarComunas lee el archivo comunas", cargadas != NULL);
    liberarComunas(cargadas);
}


// ---------- Sociedad (usa la lista doble) ----------
 
// Funcion para probar crearSociedad y asignarRecursos (usa la lista doble)
static void probarSociedad(void) {
    printf("\n== Sociedad ==\n");
 
    struct Comuna *nombres = NULL;
    nombres = agregarComunaFinal(nombres, "A", 0);
    nombres = agregarComunaFinal(nombres, "B", 0);
    nombres = agregarComunaFinal(nombres, "C", 0);
    nombres = agregarComunaFinal(nombres, "D", 0);
    nombres = agregarComunaFinal(nombres, "E", 0);
    nombres = agregarComunaFinal(nombres, "F", 0);
    nombres = agregarComunaFinal(nombres, "G", 0);
 
    // Con 7 comunas, 500 / 7 deja sobrante y prueba el reparto
    struct Comuna *sociedad = crearSociedad(nombres, 7);
    verificar("crearSociedad con 7 comunas funciona", sociedad != NULL);
 
    int total = 0;
    int comunas = 0;
    for (struct Comuna *x = sociedad; x != NULL; x = x->siguiente) {
        total += x->cantidadPersonas;
        comunas++;
    }
 
    verificar("se crean 7 comunas", comunas == 7);
    verificar("las 7 comunas suman exactamente 500 personas", total == 500);
    verificar("crearSociedad rechaza 0 comunas", crearSociedad(nombres, 0) == NULL);
    verificar("crearSociedad rechaza 51 comunas", crearSociedad(nombres, 51) == NULL);
    verificar("crearSociedad rechaza mas comunas que nombres", crearSociedad(nombres, 8) == NULL);
 
    struct Recurso *bienes = NULL;
    bienes = agregarRecursoFinal(bienes, "Arroz", 0, 0);
    bienes = agregarRecursoFinal(bienes, "Agua", 0, 0);
 
    struct Recurso *servicios = NULL;
    servicios = agregarRecursoFinal(servicios, "Salud", 0, 0);
 
    // Asigna 2 bienes y 1 servicio a cada comuna
    asignarRecursos(sociedad, bienes, 2, servicios, 1);
 
    struct Recurso *arroz = buscarRecurso(sociedad->bienes, "Arroz");
    verificar("cada comuna recibe sus bienes", arroz != NULL);
    verificar("el maximo de un bien es 2 por persona",
              arroz != NULL && arroz->maximo == sociedad->cantidadPersonas * 2);
    verificar("cada comuna recibe sus servicios", buscarRecurso(sociedad->servicios, "Salud") != NULL);
 
    liberarComunas(sociedad);
    liberarComunas(nombres);
    liberarRecursos(bienes);
    liberarRecursos(servicios);
}
 
// ---------- Arbol AVL ----------

// Funcion para probar el AVL de nombres de libros
static void probarAvl(void) {
    printf("\n== Arbol AVL ==\n");

    // Peor caso para un arbol sin balancear: nombres en orden alfabetico
    struct NodoAvl *raiz = NULL;
    char nombre[64];

    for (int i = 0; i < 1000; i++) {
        snprintf(nombre, sizeof(nombre), "libro%04d.txt", i);
        raiz = insertarAvl(raiz, nombre);
    }

    verificar("insertar 1000 nombres deja 1000 en el arbol", contarAvl(raiz) == 1000);
    // Un AVL de 1000 nodos mide como maximo 14 (sin balancear mediria 1000)
    verificar("insertados en orden, la altura sigue siendo baja (<= 14)", alturaAvl(raiz) <= 14);
    printf("          (altura con 1000 nombres: %d)\n", alturaAvl(raiz));

    raiz = insertarAvl(raiz, "libro0500.txt");
    verificar("un nombre repetido no se inserta dos veces", contarAvl(raiz) == 1000);
    verificar("buscarAvl encuentra libro0999.txt", buscarAvl(raiz, "libro0999.txt") != NULL);
    verificar("buscarAvl devuelve NULL si el libro no existe", buscarAvl(raiz, "otro.txt") == NULL);

    liberarAvl(raiz);

    // Arbol pequeno para ver su forma: 7 nombres en orden quedan perfectamente balanceados
    raiz = NULL;
    const char *nombres[] = {"a.txt", "b.txt", "c.txt", "d.txt", "e.txt", "f.txt", "g.txt"};

    for (int i = 0; i < 7; i++) {
        raiz = insertarAvl(raiz, nombres[i]);
    }

    verificar("7 nombres en orden quedan con altura 3", alturaAvl(raiz) == 3);
    printf("          (arbol con 7 nombres insertados en orden)\n");
    imprimirAvl(raiz, 0);

    liberarAvl(raiz);
}

// ---------- Arbol splay ----------

// Funcion para crear a mano un nodo del ejemplo de papel
static struct NodoSplay *nodoEjemplo(const char *texto, struct NodoSplay *izquierdo, struct NodoSplay *derecho) {
    struct NodoSplay *nodo = calloc(1, sizeof(struct NodoSplay));
    snprintf(nodo->texto, sizeof(nodo->texto), "%s", texto);
    nodo->izquierdo = izquierdo;
    nodo->derecho = derecho;
    return nodo;
}

// Funcion para probar el arbol splay con el ejemplo hecho en papel
static void probarSplay(void) {
    printf("\n== Arbol splay (chaflaneado) ==\n");

    /* Ejemplo de papel, armado a mano:

              F
             / \
            D   G
           / \
          B   E
         / \
        A   C
    */
    struct NodoSplay *raiz =
        nodoEjemplo("F",
            nodoEjemplo("D",
                nodoEjemplo("B", nodoEjemplo("A", NULL, NULL), nodoEjemplo("C", NULL, NULL)),
                nodoEjemplo("E", NULL, NULL)),
            nodoEjemplo("G", NULL, NULL));

    printf("          (arbol antes de buscar A)\n");
    imprimirSplay(raiz, 0);

    // Se imprimen las rotaciones para mostrar que si chaflanea
    mostrarRotacionesSplay(1);
    printf("          (buscando A)\n");
    raiz = buscarSplay(raiz, "A");
    mostrarRotacionesSplay(0);

    printf("          (arbol despues de buscar A)\n");
    imprimirSplay(raiz, 0);

    // Resultado esperado en papel: A arriba, F a su derecha, B y G bajo F, D bajo B
    verificar("A queda en la raiz", strcmp(raiz->texto, "A") == 0);
    verificar("F queda a la derecha de A", strcmp(raiz->derecho->texto, "F") == 0);
    verificar("B queda a la izquierda de F", strcmp(raiz->derecho->izquierdo->texto, "B") == 0);
    verificar("D queda a la derecha de B", strcmp(raiz->derecho->izquierdo->derecho->texto, "D") == 0);

    // Buscar algo que no esta: es un fallo de cache y la raiz tiene otro texto
    raiz = buscarSplay(raiz, "Z");
    verificar("buscar un texto que no esta deja otro texto en la raiz", strcmp(raiz->texto, "Z") != 0);

    // Guardar un resultado: el nodo nuevo queda en la raiz con su lista de libros
    struct LibroEncontrado *libros = NULL;
    libros = agregarLibroEncontrado(libros, "quijote.txt");
    libros = agregarLibroEncontrado(libros, "hamlet.txt");
    raiz = insertarSplay(raiz, "amor", libros, 2);

    verificar("el texto insertado queda en la raiz", strcmp(raiz->texto, "amor") == 0);
    verificar("guarda la lista de 2 libros", raiz->cantidadLibros == 2 && raiz->libros != NULL);
    verificar("el arbol tiene 8 textos", contarSplay(raiz) == 8);

    // Buscarlo de nuevo es un acierto de cache
    raiz = buscarSplay(raiz, "C");
    raiz = buscarSplay(raiz, "amor");
    verificar("al buscar amor otra vez vuelve a la raiz (acierto)", strcmp(raiz->texto, "amor") == 0);

    liberarSplay(raiz);
}

// ---------- Tabla hash de personas ----------

// Funcion para probar la tabla hash: funcion djb2, insertar, buscar,
// nombres repetidos, colisiones (encadenamiento) y carga del archivo
static void probarTablaHash(void) {
    printf("\n== Tabla hash: TablaPersonas ==\n");

    struct TablaPersonas tabla;
    iniciarTablaPersonas(&tabla);

    // Tabla recien iniciada: todas las casillas vacias
    int vacias = 1;
    for (int i = 0; i < TAMANO_TABLA; i++) {
        if (tabla.casillas[i] != NULL) {
            vacias = 0;
        }
    }
    verificar("la tabla nueva tiene todas las casillas en NULL y 0 personas", vacias && tabla.cantidad == 0);

    // La funcion hash siempre da lo mismo para el mismo nombre y cae dentro de la tabla
    int casilla = calcularCasilla("Diego Mora");
    verificar("calcularCasilla da el mismo numero para el mismo nombre", casilla == calcularCasilla("Diego Mora"));
    verificar("calcularCasilla queda entre 0 y TAMANO_TABLA - 1", casilla >= 0 && casilla < TAMANO_TABLA);

    // Insertar y buscar
    struct Persona *diego = insertarPersonaTabla(&tabla, "Diego Mora");
    verificar("insertar devuelve la persona creada", diego != NULL && strcmp(diego->nombre, "Diego Mora") == 0);
    verificar("la persona queda al inicio de la lista de su casilla", tabla.casillas[casilla] == diego);
    verificar("buscar Diego Mora encuentra el mismo nodo", buscarPersonaTabla(&tabla, "Diego Mora") == diego);
    verificar("buscar un nombre que no esta devuelve NULL", buscarPersonaTabla(&tabla, "Nadie Nunca") == NULL);
    verificar("insertar un nombre vacio devuelve NULL", insertarPersonaTabla(&tabla, "") == NULL);

    // Nombres repetidos: el segundo y el tercero se renombran para que sean unicos
    struct Persona *ana1 = insertarPersonaTabla(&tabla, "Ana Rojas");
    struct Persona *ana2 = insertarPersonaTabla(&tabla, "Ana Rojas");
    struct Persona *ana3 = insertarPersonaTabla(&tabla, "Ana Rojas");
    verificar("el primer nombre repetido se queda igual", strcmp(ana1->nombre, "Ana Rojas") == 0);
    verificar("el segundo se renombra como \"Ana Rojas 2\"", strcmp(ana2->nombre, "Ana Rojas 2") == 0);
    verificar("el tercero se renombra como \"Ana Rojas 3\"", strcmp(ana3->nombre, "Ana Rojas 3") == 0);
    verificar("buscar \"Ana Rojas 2\" encuentra a la segunda", buscarPersonaTabla(&tabla, "Ana Rojas 2") == ana2);
    verificar("la tabla cuenta 4 personas", tabla.cantidad == 4);

    // Colisiones: con mas nombres que casillas, al menos dos caen en la misma casilla
    // (principio del palomar). Se insertan 600 nombres y se busca una casilla con 2 o mas.
    char nombre[50];
    for (int i = 0; i < 600; i++) {
        snprintf(nombre, sizeof(nombre), "Persona %d", i);
        insertarPersonaTabla(&tabla, nombre);
    }

    struct Persona *primera = NULL;
    for (int i = 0; i < TAMANO_TABLA && primera == NULL; i++) {
        if (tabla.casillas[i] != NULL && tabla.casillas[i]->siguiente != NULL) {
            primera = tabla.casillas[i];
        }
    }

    verificar("con 604 personas hay una casilla con 2 o mas (colision)", primera != NULL);

    if (primera != NULL) {
        struct Persona *segunda = primera->siguiente;
        printf("          (\"%s\" y \"%s\" chocan en la casilla %d)\n",
               primera->nombre, segunda->nombre, calcularCasilla(primera->nombre));
        verificar("las dos personas que chocan dan la misma casilla",
                  calcularCasilla(primera->nombre) == calcularCasilla(segunda->nombre));
        verificar("buscar cada una encuentra su propio nodo (encadenamiento)",
                  buscarPersonaTabla(&tabla, primera->nombre) == primera &&
                  buscarPersonaTabla(&tabla, segunda->nombre) == segunda);
    }

    // Todas las personas se pueden encontrar
    int encontradas = 0;
    for (int i = 0; i < 600; i++) {
        snprintf(nombre, sizeof(nombre), "Persona %d", i);
        if (buscarPersonaTabla(&tabla, nombre) != NULL) {
            encontradas++;
        }
    }
    verificar("las 600 personas insertadas se encuentran", encontradas == 600);
    verificar("la tabla cuenta 604 personas", tabla.cantidad == 604);

    liberarTablaPersonas(&tabla);

    // Carga desde el archivo personas
    iniciarTablaPersonas(&tabla);
    int cargadas = cargarTablaPersonas(&tabla, "personas");
    verificar("cargarTablaPersonas lee las 500 personas del archivo", cargadas == 500);
    liberarTablaPersonas(&tabla);

    iniciarTablaPersonas(&tabla);
    verificar("cargarTablaPersonas devuelve 0 si el archivo no existe",
              cargarTablaPersonas(&tabla, "no_existe.txt") == 0);
    liberarTablaPersonas(&tabla);
}

// Funcion principal de las pruebas: corre todas y devuelve 1 si alguna fallo
int main(void) {
    probarPersonas();
    probarRecursos();
    probarComunas();
    probarSociedad();
    probarAvl();
    probarSplay();
    probarTablaHash();
 
    printf("\n");
    if (fallos == 0) {
        printf("Todas las pruebas pasaron\n");
    } else {
        printf("Pruebas fallidas: %d\n", fallos);
    }
 
    return fallos != 0;
}
 