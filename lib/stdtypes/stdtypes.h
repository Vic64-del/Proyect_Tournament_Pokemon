#ifndef _STD_DATYPES
#define _STD_DATYPES
#include <stdio.h>    
#include <stdlib.h>
//Esta libreria define la mayoria de tipos de datos mas importantes en este proyecto: pokemon, ejemplares, entrandores y sus equipos

//=================================================//      Aqui comienza la definicion de los tipos Pokemon, antes que nada

#define TYPESPOK_CANT 18        //18 tipos de pokemon
#define MATRIX_CORP (TYPESPOK_CANT*TYPESPOK_CANT)     //La matriz de tipos, tendra el tamano de la multiplicacion de la cantidad de tipos
#define FACTOR_CONV 2   //Factor de conversion para mi el enum de interacciones entre tipos
#define DMG_COMBAT_GEN 10       //Reservado para efectuar un dano generico fijo en caso de que no se puedan danar los combatientes

#define LEN_RESULTS 30          //Macro para la cantidad maxima de resultados de torneo. Se coloca en 30 por si acaso

//////
#define OK                 0
#define ERR_ID_DUP         1
#define ERR_ID_RANGO       2
#define ERR_ID_FUERA_ORDEN 3    //para manejar errroes de resultado
#define ERR_RONDA          4
#define ERR_COMB_IGUALES   5
#define ERR_POS_RANGO      6
#define ERR_GRUPO_INVALIDO 7
//////

typedef char nombre[15];    //Nombre para el tipo
typedef struct Tipo{                //Definimos un tipo pokemon
    int id;          //Id de un tipo
    nombre name;
  
}Tipo;              //Estructura para los tipos Pokemon
                    
typedef Tipo Lista_tipos[TYPESPOK_CANT];        //Definimos la lista de tipos pokemon

typedef enum Interac{       //Enum que contiene las interacciones entre tipos elementales
    SUPER_EFFECT = 4,     //Muy efectivo
    NONE_EFFECT = 0,      //Sin efecto
    LESS_EFFECT = 1,       //Poco efectivo
    NEUTRO = 2  //Efecto neutro

}Interac;

typedef enum{
    AGUA       = 0,         //Enum de 18 Tipos con sus id. Usar -1 para acceder Cortesia de la ia
    PLANTA     = 1,
    FUEGO      = 2,
    ELECTRICO  = 3,
    HIELO      = 4,
    LUCHA      = 5,
    VENENO     = 6,
    TIERRA     = 7,
    VOLADOR    = 8,
    PSIQUICO   = 9,
    BICHO      = 10,
    ROCA       = 11,
    FANTASMA   = 12,
    DRAGON     = 13,
    SINIESTRO  = 14,
    ACERO      = 15,
    HADA       = 16,
    NORMAL     = 17


}Tipos_Dominio;

typedef Interac Tabla_Tipos[TYPESPOK_CANT][TYPESPOK_CANT]; 


//=================================================//   Aqui comienza la definicion de una especie
typedef struct Especie{
    char id[5];         //Id unico de la especie
    int ide;     //entero para el id 
    char nombre_specie[20];
    char lectura_tipo1[15];     //Cadenas que se utilizan para leer los tipos de cada Pokemon
    char lectura_tipo2[15]; 
    Tipo tipo1;                 //Tipos reales del Pokemon, con nombre e identificador            
    Tipo tipo2;
    int hp_base;
    int atk_base;       //Stats base de la especie
    int def_base;
    int vel_base;

}Especie;

typedef struct Ejemplar{
    int nivel;      //Nivel del ejemplar
    int hp;             //puntos de vida
    int atk;
    int def;            //Estadisticas del Pokemon
    int vel;
    int is_alive;       //Booleano para saber si esta debilitado o no
    int hp_real;            //Entero para almacenar la vida real del ejemplar, no de su especie, para restaurarlo
    Especie * fijer;         //Apunta a su especie asociada
    struct Ejemplar * siguiente;        //Apunta al siguiente integrante

}Ejemplar;      //Lista de Ejemplar, que puede estar asociada a otros (equipo)

//=================================================//       Aqui comienza la definicion de un equipo y su entrenador

typedef struct Team{
    int longitud;
    int base_cantidad;          //Cantidad base de ejemplares que tiene, no cambiara durante el torneo
    int cant_vivos;     //Cantidad de pokemon que se mantienen de pie
    Ejemplar * equipo;
}Team;                      //Team, cabecera que tendra cada entrenador y asi su lista de pokemones, o equipo

typedef struct Trainer{
    int id;             //id que debe ser irrepetible
    char nombre[20];
    int cant_vic;   
    int cant_derr;              //Datos de cantidad de victorias, derrotas y empates
    int cant_empat;
    int score;
    int cant_pok_derrot;        //Cantidad de pokemon derrotados por este trainer
    Team * trainer_team;            //Apuntador a equipo

}Trainer;           //Entrenador, que tiene una cabecera a la lista de su equipo

//Las siguientes definiciones de tipo corresponden a la carga de resultados del torneo (octavos)

typedef struct{
    int pos;
    char grupo;

}combatiente;       //Un combatiente con sus datos

typedef struct{
    int id_combat;
    combatiente comb1;
    combatiente comb2;

}indiv_combat;          //Combate individual que es cargado: Tiene id, y datos de los dos combatientes

typedef indiv_combat combats_result[LEN_RESULTS];    //Arreglo de resultados de combates cargados (por file o por input)
    //DEFINIR EL TAMANO DEL ARREGLO DE CARGA DE COMBATES COMO MACRO


//Funciones principalmente auxiliares 

//Funciones de cadenas
void corregir_formato(char input[]);
int comparar_cadena(char s1[], char s2[]);
void corregir_id(char input[]);
void procesar_linea(Tabla_Tipos table, FILE * base_tipos, int fil);

//Funciones para la tabla de tipos
void crear_tablatipos(Tabla_Tipos table);
void cargar_tipos_and_table(Lista_tipos coleccion, Tabla_Tipos table, FILE * base_tipos);
void imprimir_tabla(Tabla_Tipos table);
void mostrar_tipos(Lista_tipos coleccion);
void mostrar_interacciones(int atk, int def, Tabla_Tipos table);
void mostrar_todo_interc(Tabla_Tipos table, int id, Lista_tipos coleccion);
float devolver_multiplicar_tipo(int atk, int def, Tabla_Tipos table);

//funciones auxiliaries
void mostrar_estado(Trainer * ptr);
void cargar_results_file(FILE * base_results, combats_result arr);
void limpiar_buffer(char *buf, size_t tam);
void limpiar_stdin();
void mostrar_contenido(FILE *f);
void imprimir_enfrentamientos(combats_result arr_results);

int hay__tipos_duplicados(const Lista_tipos lista);


int validar_results(const combats_result arr, int n_octavos, int n_cuartos, int n_semis);





#endif