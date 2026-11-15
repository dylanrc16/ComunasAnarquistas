// Evita que este archivo se incluya dos veces
#ifndef CELULAR_H
#define CELULAR_H

#include "Comunas.h"
#include "TablaPersonas.h"
#include "Biblioteca.h"
#include "Cola.h"

// Bateria llena
#define BATERIA_MAXIMA 100
// Lo que carga en un dia con los paneles solares al maximo
#define CARGA_SOLAR_MAXIMA 30
// Lo que carga en un dia si la partida no tiene el bien "Paneles solares"
#define CARGA_SIN_PANELES 10
// Lo que gasta encender el celular
#define GASTO_ENCENDER 2
// Lo que gasta ver una pagina (de la lista de libros o de un libro)
#define GASTO_PAGINA 1
// Lo que gasta una busqueda que acierta en la cache (no lee archivos)
#define GASTO_BUSQUEDA_CACHE 1
// Lo que gasta una busqueda que tiene que leer los archivos (fallo de cache)
#define GASTO_BUSQUEDA_ARCHIVOS 5

// Libros que se muestran por pagina en la lista
#define LIBROS_POR_PAGINA 8
// Lineas de texto por pagina al leer un libro
#define LINEAS_POR_PAGINA 15
// Paginas que hay que leer para que un libro cuente como leido
#define PAGINAS_PARA_LEER 3

// Link y archivo del meme secreto (opcion 4)
#define LINK_MEME "https://github.com/dylanrc16/ComunasAnarquistas/blob/proyecto2/memes/meme.png"
#define ARCHIVO_MEME "memes/meme.png"

// Funcion para dejar la bateria de todos los celulares llena (al iniciar)
void iniciarCelulares(struct Comuna *sociedad);
// Funcion para cargar los celulares con energia solar (una vez por dia):
// carga segun los paneles solares de cada comuna, sin pasar del maximo
void cargarCelulares(struct Comuna *sociedad);
// Funcion para abrir el celular comunitario de una comuna.
// Necesita la sociedad y la tabla (para que leer cuente para aprender)
// y el catalogo (para la app Biblioteca) y la cola de pedidos (para la red de ayuda mutua).
// Devuelve 1 si se eligio la autodestruccion (opcion 5) y 0 si se apago normal.
int abrirCelular(struct Comuna *comuna, struct Comuna *sociedad, struct TablaPersonas *tabla, const struct Catalogo *catalogo, struct Cola *pedidos);

// Fin de la proteccion contra inclusion doble
#endif
