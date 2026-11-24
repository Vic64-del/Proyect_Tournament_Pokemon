#ifndef _STD_POKE
#define _STD_POKE
#include "stdtypes/stdtypes.h"
#include <stdio.h>

#define POKES_CANT 150            //150 especies de pokemon
#define GRADO_VARIATION 5       //Grado de variacion para las estadisticas de un pokemon
#define LEVEL_CAP 100       //Nivel maximo = 100

typedef Especie Pokedex[POKES_CANT];    //Definimos la pokedex como arreglo de Especies. 150 bichos
typedef Especie * ptr_pok;

//Funciones para la pokedex y tipos 
void iniciar_pokedex(Pokedex mi_pokedex);
void cargar_pokedex(FILE * base_datos, Pokedex mi_pokedex);
void mostrar_especie(Pokedex mi_pokedex, int i);
void mostrar_ejemplar(Ejemplar * sample);
void mostrar_dex_completa(Pokedex mi_pokedex);
void buscar_pk_nombre(Pokedex mi_pokedex, char nombre[]);
void buscar_pk_numero(Pokedex dex, char numero[]);
void construir_ejemplar(ptr_pok dex, Ejemplar * animal, int id, int nivel);
void construir_equipo(ptr_pok dex, Trainer * ptr, int n, int nivel);
void mostrar_equipo(Trainer * ptr);
void asociar_tipos_especie(Pokedex dex, Lista_tipos coleccion_tipos);
void asigar_tipo(Pokedex dex, Lista_tipos coleccion, int id);
int calcular_stat(int nivel, int stat_base, int cons);


#endif