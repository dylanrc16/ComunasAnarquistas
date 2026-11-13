// Evita que este archivo se incluya dos veces
#ifndef CELULAR_H
#define CELULAR_H

#include "Comunas.h"

// Bateria llena
#define BATERIA_MAXIMA 100
// Lo que carga en un dia con los paneles solares al maximo
#define CARGA_SOLAR_MAXIMA 30
// Lo que carga en un dia si la partida no tiene el bien "Paneles solares"
#define CARGA_SIN_PANELES 10
// Lo que gasta encender el celular
#define GASTO_ENCENDER 2

// Link y archivo del meme secreto (opcion 4)
#define LINK_MEME "https://github.com/dylanrc16/ComunasAnarquistas/blob/proyecto2/memes/meme.png"
#define ARCHIVO_MEME "memes/meme.png"

// Funcion para dejar la bateria de todos los celulares llena (al iniciar)
void iniciarCelulares(struct Comuna *sociedad);
// Funcion para cargar los celulares con energia solar (una vez por dia):
// carga segun los paneles solares de cada comuna, sin pasar del maximo
void cargarCelulares(struct Comuna *sociedad);
// Funcion para abrir el celular comunitario de una comuna.
// Devuelve 1 si se eligio la autodestruccion (opcion 5) y 0 si se apago normal.
int abrirCelular(struct Comuna *comuna);

// Fin de la proteccion contra inclusion doble
#endif
