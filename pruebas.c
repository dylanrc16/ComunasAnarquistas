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
#include "Jugador.h"
#include "Cola.h"
#include "Notificaciones.h"

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

// ---------- Comunas del jugador ----------

// Funcion para probar la lista de comunas que administra el jugador:
// agregar (maximo 3, sin repetir), quitar una (como al disolverse) y reemplazarla
static void probarJugador(void) {
    printf("\n== Lista simple: Jugador (comunas administradas) ==\n");

    // Comunas de prueba (en el juego viven en la lista doble de la sociedad)
    struct Comuna *a = crearComuna("A", 10);
    struct Comuna *b = crearComuna("B", 10);
    struct Comuna *c = crearComuna("C", 10);
    struct Comuna *d = crearComuna("D", 10);

    struct Jugador jugador;
    iniciarJugador(&jugador);
    verificar("el jugador empieza sin comunas y sin vidas perdidas",
              jugador.comunas == NULL && jugador.vidasPerdidas == 0 && contarAdministradas(&jugador) == 0);

    // Agregar
    verificar("agregar A funciona", agregarAdministrada(&jugador, a) == 1);
    verificar("agregar A otra vez no se permite (no se repite)", agregarAdministrada(&jugador, a) == 0);
    verificar("agregar B y C funciona", agregarAdministrada(&jugador, b) == 1 && agregarAdministrada(&jugador, c) == 1);
    verificar("ya administra 3 comunas", contarAdministradas(&jugador) == COMUNAS_ADMINISTRADAS);
    verificar("agregar una cuarta (D) no se permite: maximo 3", agregarAdministrada(&jugador, d) == 0);
    verificar("agregar NULL no se permite", agregarAdministrada(&jugador, NULL) == 0);

    // Se agregan al final: el orden es A, B, C
    verificar("el orden de la lista es A, B, C",
              jugador.comunas->comuna == a && jugador.comunas->siguiente->comuna == b &&
              jugador.comunas->siguiente->siguiente->comuna == c &&
              jugador.comunas->siguiente->siguiente->siguiente == NULL);

    // estaAdministrada compara punteros
    verificar("A esta administrada", estaAdministrada(&jugador, a) == 1);
    verificar("D no esta administrada", estaAdministrada(&jugador, d) == 0);

    // Quitar la del medio (como si B se disolviera): la lista se reengancha
    verificar("quitar B funciona", quitarAdministrada(&jugador, b) == 1);
    verificar("despues de quitar B quedan A y C", contarAdministradas(&jugador) == 2 &&
              jugador.comunas->comuna == a && jugador.comunas->siguiente->comuna == c);
    verificar("B ya no esta administrada", estaAdministrada(&jugador, b) == 0);
    verificar("quitar B otra vez devuelve 0 (ya no estaba)", quitarAdministrada(&jugador, b) == 0);
    verificar("quitar no libera la comuna: B sigue existiendo", strcmp(b->nombre, "B") == 0);

    // Reemplazo: con un espacio libre ya se puede agregar D
    verificar("con un espacio libre se puede agregar D como reemplazo", agregarAdministrada(&jugador, d) == 1);
    verificar("vuelve a tener 3 comunas", contarAdministradas(&jugador) == 3);

    // Quitar la primera (cambia el inicio de la lista)
    verificar("quitar A (la primera) funciona", quitarAdministrada(&jugador, a) == 1);
    verificar("ahora la lista empieza en C", jugador.comunas->comuna == c);

    // Liberar la lista: solo los nodos, las comunas siguen vivas
    liberarJugador(&jugador);
    verificar("liberarJugador deja la lista vacia", jugador.comunas == NULL && contarAdministradas(&jugador) == 0);
    verificar("liberarJugador no libera las comunas: C sigue existiendo", strcmp(c->nombre, "C") == 0);

    liberarComuna(a);
    liberarComuna(b);
    liberarComuna(c);
    liberarComuna(d);
}

// ---------- Cola de pedidos ----------

// Funcion para escribir en texto el orden de la cola, por ejemplo "A:Agua B:Pan"
static void ordenCola(const struct Cola *cola, char *destino, int tamano) {
    destino[0] = '\0';

    for (struct Pedido *p = cola->frente; p != NULL; p = p->siguiente) {
        char parte[160];
        snprintf(parte, sizeof(parte), "%s%s:%s", destino[0] != '\0' ? " " : "", p->comuna->nombre, p->bien);
        strncat(destino, parte, tamano - strlen(destino) - 1);
    }
}

// Funcion para probar la cola FIFO de pedidos de la red de ayuda mutua
static void probarCola(void) {
    printf("\n== Cola FIFO: pedidos de ayuda mutua ==\n");

    struct Comuna *a = crearComuna("A", 10);
    struct Comuna *b = crearComuna("B", 10);
    char orden[200];

    struct Cola cola;
    iniciarCola(&cola);
    verificar("la cola nueva esta vacia", colaVacia(&cola) && cola.frente == NULL && cola.final == NULL);
    verificar("desencolar una cola vacia devuelve NULL", desencolarPedido(&cola) == NULL);

    // Encolar: cada pedido va al final
    encolarPedido(&cola, a, "Agua");
    encolarPedido(&cola, b, "Pan");
    encolarPedido(&cola, a, "Semillas");
    ordenCola(&cola, orden, sizeof(orden));
    verificar("encolar 3 pedidos los deja en orden de llegada", strcmp(orden, "A:Agua B:Pan A:Semillas") == 0);
    verificar("el frente es el primero y el final el ultimo",
              cola.frente->comuna == a && strcmp(cola.final->bien, "Semillas") == 0 && cola.cantidad == 3);
    verificar("la misma comuna no puede pedir dos veces el mismo bien", encolarPedido(&cola, a, "Agua") == 0);
    verificar("existePedido encuentra B:Pan", existePedido(&cola, b, "Pan") == 1);

    // Desencolar: sale el primero que entro (FIFO)
    struct Pedido *primero = desencolarPedido(&cola);
    verificar("desencolar saca el primero que llego (A:Agua)", primero->comuna == a && strcmp(primero->bien, "Agua") == 0);
    verificar("despues quedan 2 y el frente es B:Pan", cola.cantidad == 2 && cola.frente->comuna == b);

    // Reencolar: un pedido no atendido vuelve al final
    primero->diasEsperando++;
    reencolarPedido(&cola, primero);
    ordenCola(&cola, orden, sizeof(orden));
    verificar("reencolar pone el pedido al final", strcmp(orden, "B:Pan A:Semillas A:Agua") == 0);
    verificar("el pedido reencolado conserva sus dias de espera", cola.final->diasEsperando == 1);

    // Cancelar uno del medio: los demas conservan su orden
    verificar("cancelar A:Semillas funciona", cancelarPedido(&cola, a, "Semillas") == 1);
    ordenCola(&cola, orden, sizeof(orden));
    verificar("los demas conservan su orden", strcmp(orden, "B:Pan A:Agua") == 0);
    verificar("cancelar algo que no esta devuelve 0", cancelarPedido(&cola, a, "Semillas") == 0);

    // Quitar todos los pedidos de una comuna (como al disolverse)
    encolarPedido(&cola, a, "Pan");
    verificar("quitar los pedidos de A quita 2", quitarPedidosComuna(&cola, a) == 2);
    ordenCola(&cola, orden, sizeof(orden));
    verificar("solo queda B:Pan y el final apunta a el",
              strcmp(orden, "B:Pan") == 0 && cola.final == cola.frente && cola.cantidad == 1);

    // Al vaciarse, frente y final quedan en NULL
    free(desencolarPedido(&cola));
    verificar("al sacar el ultimo, frente y final quedan en NULL", cola.frente == NULL && cola.final == NULL);

    // Liberar con pedidos adentro
    encolarPedido(&cola, a, "Agua");
    encolarPedido(&cola, b, "Agua");
    liberarCola(&cola);
    verificar("liberarCola deja la cola vacia", colaVacia(&cola) && cola.cantidad == 0);

    liberarComuna(a);
    liberarComuna(b);
}

// ---------- Pila de notificaciones ----------

// Funcion para probar la pila (LIFO) de notificaciones del celular
static void probarNotificaciones(void) {
    printf("\n== Pila LIFO: notificaciones ==\n");

    struct Bandeja bandeja;
    iniciarBandeja(&bandeja);
    verificar("la bandeja nueva esta vacia", bandeja.tope == NULL && bandeja.cantidad == 0 && bandeja.noLeidas == 0);
    verificar("desapilar una bandeja vacia devuelve NULL", desapilarNotificacion(&bandeja) == NULL);

    // Apilar: la ultima que llega queda en el tope
    apilarNotificacion(&bandeja, 1, "primera");
    apilarNotificacion(&bandeja, 2, "segunda");
    apilarNotificacion(&bandeja, 3, "tercera");
    verificar("la ultima que llego queda en el tope", strcmp(bandeja.tope->texto, "tercera") == 0 && bandeja.tope->dia == 3);
    verificar("debajo del tope esta la anterior", strcmp(bandeja.tope->siguiente->texto, "segunda") == 0);
    verificar("hay 3 notificaciones sin leer", bandeja.cantidad == 3 && bandeja.noLeidas == 3);

    // Desapilar: sale la mas reciente (LIFO)
    struct Notificacion *sacada = desapilarNotificacion(&bandeja);
    verificar("desapilar saca la mas reciente (tercera)", strcmp(sacada->texto, "tercera") == 0);
    verificar("despues el tope es segunda", strcmp(bandeja.tope->texto, "segunda") == 0 && bandeja.cantidad == 2);
    free(sacada);

    // Tope maximo: al pasarse se descarta la del fondo
    for (int i = 0; i < NOTIFICACIONES_MAXIMAS; i++) {
        char texto[20];
        snprintf(texto, sizeof(texto), "aviso %d", i);
        apilarNotificacion(&bandeja, 10, texto);
    }

    struct Notificacion *fondo = bandeja.tope;
    while (fondo->siguiente != NULL) {
        fondo = fondo->siguiente;
    }

    verificar("nunca guarda mas de NOTIFICACIONES_MAXIMAS", bandeja.cantidad == NOTIFICACIONES_MAXIMAS);
    verificar("se descartaron las mas viejas: el fondo es aviso 0", strcmp(fondo->texto, "aviso 0") == 0);
    verificar("el tope es la ultima que llego", strcmp(bandeja.tope->texto, "aviso 29") == 0);

    // notificar() arma el texto como printf y le pone el dia actual
    struct Comuna *a = crearComuna("A", 10);
    fijarDiaNotificaciones(7);
    notificar(a, "%s recibio %d de %s", "A", 16, "Agua");
    verificar("notificar arma el texto con formato", strcmp(a->bandeja.tope->texto, "A recibio 16 de Agua") == 0);
    verificar("notificar le pone el dia actual", a->bandeja.tope->dia == 7);
    liberarComuna(a);

    liberarBandeja(&bandeja);
    verificar("liberarBandeja deja la bandeja vacia", bandeja.tope == NULL && bandeja.cantidad == 0);
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
    probarJugador();
    probarCola();
    probarNotificaciones();
 
    printf("\n");
    if (fallos == 0) {
        printf("Todas las pruebas pasaron\n");
    } else {
        printf("Pruebas fallidas: %d\n", fallos);
    }
 
    return fallos != 0;
}
 