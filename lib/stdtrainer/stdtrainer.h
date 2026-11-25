#ifndef _STD_TRAIN
#define _STD_TRAIN
#include "stdtypes/stdtypes.h"  //Incluimos types tambien

#define LONG_TEAM 6     //Longitud de equiipo
#define TRAINERS_TOURNAMENT 32  //Capacidad de entrenadores para el torneo
#define POINT_BY_WIN 3      //Puntos que se obtienen al perder o al ganar en cierta fase del torneo
#define POINT_BY_LOSE 0
#define POINT_BY_TIE 1

#define FINAL_OCTAVOS 8
#define FIL_GROUP 8         //8 Filas, 8 grupos
#define COL_GROUP 4         //4 filas, 4 entrenadores por grupo

typedef Trainer tabla_grupos[FIL_GROUP][COL_GROUP];     //Matriz de grupos apra torneo
typedef Trainer lista_trainers[TRAINERS_TOURNAMENT];        //Arreglo de entrenadores
typedef char string_letras[FIL_GROUP+1];          //Arreglo con las letras de la A - H para denotar cada grupo

typedef char cad[20];
typedef cad string_arr[TRAINERS_TOURNAMENT];    //String de nombres para cargar a los entrenadores


typedef struct{
    Trainer trainer_first;
    Trainer trainer_second;
    char grupo;         //Contiene la etiqueta de un grupo
    int i_first;            //Contendra para octavos, el id de un combate
    int j_second;

    char tag_winner;        //para contener el tag de ganador
    int id_combat;          //para contener el id del combate disputado

}miembro_trancision;    

typedef miembro_trancision arreglo_trancision[20];      //Arreglo que contiene miembros cuyos campos son: la posicion y el grupo junto al entrenador
//Este arreglo sera utilizado tambien para obtener los ganadores de los octavos de final


typedef struct{
    Trainer champ;
    Trainer subchamp;
    Trainer third;
    Trainer four;

}podio;         //tipo de dato podio para transportar a los 4 entrenadoes ganadores




//Las siguientes definiciones corresponden a una Cola de trainers, para llevar parte del torneo

typedef struct nodo_cola{
    struct nodo_cola * siguiente;
    Trainer user_train;


}nodo_cola;             //Nodo de una cola que posee un entrenador

typedef struct{
    nodo_cola * frente;
    nodo_cola * final;
 
}Cola_torneo;       //Cabecera de una cola que apunta al principio y al final de la misma


//Funciones de trainer

void iniciar_team(Team * user, int cant_poke);
void iniciar_trainer(Trainer * sample, char nombre[], int id, int cant_poke);
int es_equipo_vacio(Team * sample);
void mostrar_trainer(Trainer * user);
void cargar_trainers(FILE * base_trainer, lista_trainers arr_trainers, int discri, int cant_poke);
void mostrar_estado(Trainer * ptr);


//Funciones para los combates

int calcular_damage(Ejemplar * atacante, Ejemplar * defensor, Tabla_Tipos table);
void turno_generico(Ejemplar * atk, Ejemplar * def,  Tabla_Tipos table );
void combate_gen(Ejemplar * oponente1, Ejemplar * oponente2, Tabla_Tipos table);

void sala_trainers(Trainer * combatiente1, Trainer * combatiente2, Tabla_Tipos table, FILE * hist);
//void sala_trainers(Trainer * combatiente1, Trainer * combatiente2, Tabla_Tipos table);
int posee_pok_vivos(Trainer * combatiente);
void curar_equipo(Trainer * ptr);
int se_excluyen_mutua(Ejemplar * pokemon_1, Ejemplar * pokemon_2, Tabla_Tipos table);

//Funciones de torneo
                                                                //CAMBIO 14/11/26 AGREGADO ARCHIVO FILE PARA ESCRIBIR HISTORIAL!
void gran_torneo(tabla_grupos grupos, lista_trainers arr_entrenadores, Tabla_Tipos table, combats_result resultados, podio * posicion_podio, FILE * historial, Cola_torneo * cola);
void fase_grupos(tabla_grupos grupos, Tabla_Tipos table, int f, FILE * historial);
void fase_octavos(arreglo_trancision arr_grupos, Tabla_Tipos table, combats_result resultados, FILE * historial, Cola_torneo * cola);
void fase_cuartos(arreglo_trancision arr_grupos, Tabla_Tipos table, combats_result resultados, FILE * historial, Cola_torneo * cola);
void fase_semis(arreglo_trancision arr_grupos, Tabla_Tipos table, combats_result resultados, FILE * historial, Cola_torneo * cola);
void fase_final(arreglo_trancision arr_grupos, Tabla_Tipos table, podio * posicion_podio, FILE * historial);

//Funciones auxiliares del torneo

void buscar_grupo(int i, arreglo_trancision arr_grupos, combats_result results, Trainer * a, Trainer * b);

void imprimir_grupos(tabla_grupos grupos, string_letras tags);
void asignar_matriz(tabla_grupos grupos, lista_trainers arr_entrenadores);
void asigar_grupos_trancision(tabla_grupos grupos, string_letras tags, arreglo_trancision arr, Cola_torneo * cola);
void copiar_arreglo_grupo(arreglo_trancision destino, arreglo_trancision fuente);

void buscar_grupo_semis(int i, arreglo_trancision arr_restantes, combats_result resultados, Trainer * a, Trainer * b);


//Funciones para la cola de entrenadores

int es_cola_empty(Cola_torneo * cola_torn);
void iniciar_cola_tournam(Cola_torneo * cola_torn);
void encolar_tournam(Cola_torneo * cola_torn, Trainer dato);
Trainer desencolar_tournam(Cola_torneo * cola_torn);

//Funciones de ordenamiento
void quick_sort_trainer(tabla_grupos grupo_fila, int fila, int high, int low);
int quick_partition(tabla_grupos grupo_fila, int fila, int high, int low);
void swap_quick(Trainer * a, Trainer * b);
int comparar_trainer_quick(Trainer * a, Trainer * b);

//Ordenamiento para arreglo simple de trainers
void quick_sort_trainer_list(lista_trainers arr_trainers, int n, int high, int low);
int quick_partition_list(lista_trainers arr_trainers, int n, int high, int low);


//Funciones varias
void debug_resultados(combats_result resultados, arreglo_trancision arr_grupos, int len_grupos, int len_results);
void mostrar_equipo_trainer(Trainer * ptr);
void mostrar_ejemplar_trainer(Ejemplar * sample);
void liberar_equipo_trainer(Trainer * sample);
void liberar_cabeza_trainer(Trainer * sample);
void imprimir_lista_entrenadores(lista_trainers arr_trainers, int particip);

void buscar_entrenador_nombre(lista_trainers mis_train, char name[], int cant);
void buscar_entrenador_mostrar_team(lista_trainers mis_train, char name[], int cant);

int hay_entrenadores_repetidos(lista_trainers trainers, int cantidad);


#endif