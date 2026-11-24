#include "stdtypes/stdtypes.h"      //Incluimos STDTPES!
#include "stdpoke.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void iniciar_pokedex(Pokedex mi_pokedex){ //Para una pokedex vacia
    mi_pokedex[0].atk_base = 0;
    mi_pokedex[0].def_base = 0;
    mi_pokedex[0].vel_base = 0;
    mi_pokedex[0].hp_base = 0;
    
}

void cargar_pokedex(FILE * base_datos, Pokedex mi_pokedex){//Lee la pokedex de la base 
    for(int i=0;i<POKES_CANT;i++){
        fscanf(base_datos,"%s",mi_pokedex[i].id);
        fscanf(base_datos,"%s",mi_pokedex[i].nombre_specie);
        fscanf(base_datos,"%s",mi_pokedex[i].lectura_tipo1);
        fscanf(base_datos,"%s",mi_pokedex[i].lectura_tipo2);
        fscanf(base_datos,"%i",&mi_pokedex[i].hp_base);
        fscanf(base_datos,"%i",&mi_pokedex[i].atk_base);
        fscanf(base_datos,"%i",&mi_pokedex[i].def_base);
        fscanf(base_datos,"%i",&mi_pokedex[i].vel_base);
        mi_pokedex[i].ide = i+1;
        
    }
    
}   

void mostrar_especie(Pokedex mi_pokedex, int i ){ //Muestra una especie de la pokedex
    printf("==========\n");
    
    printf("Id: %s\n",mi_pokedex[i].id);
    printf("Nombre: %s | Tipos: %s %s\n",mi_pokedex[i].nombre_specie,mi_pokedex[i].lectura_tipo1,mi_pokedex[i].lectura_tipo2);
    printf("Estadisticas: %i %i %i %i\n",mi_pokedex[i].hp_base,mi_pokedex[i].atk_base,mi_pokedex[i].def_base,mi_pokedex[i].vel_base);
    printf("==========\n");

}


void mostrar_ejemplar(Ejemplar * sample){       //Muestra un ejemplar pokemon 
    printf("Especie: %s\n",sample->fijer->nombre_specie);
    printf("Nivel: %i\n",sample->nivel);
    printf("Stats: %i %i %i %i\n",sample->hp,sample->atk,sample->def,sample->vel);
    printf("ID de Especie: %i\n",sample->fijer->ide);

}


void mostrar_dex_completa(Pokedex mi_pokedex){  //Imprime toda la pokedex
    for(int i=0;i<POKES_CANT;i++){
        printf("%s %s %s %s : ",mi_pokedex[i].id,mi_pokedex[i].nombre_specie,mi_pokedex[i].lectura_tipo1,mi_pokedex[i].lectura_tipo2);
        printf("%i %i %i %i\n",mi_pokedex[i].hp_base,mi_pokedex[i].atk_base,mi_pokedex[i].def_base,mi_pokedex[i].vel_base);
        
    }


}

void buscar_pk_nombre(Pokedex mi_pokedex, char nombre[]){   //Busca un pokemon en la dex usando un *char
    int i=0;
    int flag = 0;

    while(i<POKES_CANT){
        if(strcmp(nombre,mi_pokedex[i].nombre_specie)==0){
           printf("Pokemon encontrado\n");
           mostrar_especie(mi_pokedex,i);
           flag=1; 
           break;
        }

        i++;
    }

    if(!flag){
        printf("No se encontro al Pokemon\n");
    }


}

void buscar_pk_numero(Pokedex dex, char numero[]){  //Busca un pokemon por su ID
    int flag = 0;
    int i=0;

    while(i<POKES_CANT){
        if(strcmp(dex[i].id,numero)==0){
            mostrar_especie(dex,i);
            flag=1;
            break;
        }


        i++;
    }

    if(!flag){
        printf("Id no encontrado\n");
    }
}

void construir_ejemplar(ptr_pok dex, Ejemplar * animal, int id, int nivel){  //Construye un ejemplar con estadisticas y su apuntador a especie
    animal->fijer = &dex[id];            //El apuntador de animal tiene la direccion de dex[id]
    //printf("Sizeof de ptr: %zu\n",sizeof(dex));
    animal->atk = calcular_stat(nivel,animal->fijer->atk_base,0) + 5;
    animal->def = calcular_stat(nivel,animal->fijer->def_base,0) + 5;   //Stats calculadas con variabilidad en base
    animal->vel = calcular_stat(nivel,animal->fijer->vel_base,0) + 5;   
    animal->hp = calcular_stat(nivel,animal->fijer->hp_base,nivel) + 10;
    animal->hp_real = animal->hp;           //Hp real nunca va a cambiar. El hp normal si cambia durante combates, y debe regresar al valor de hp real

    animal->is_alive = 1;   //Esta disponible para combate, 0 para debilitados
    animal->nivel = nivel;       //asignado un nivel
}

int calcular_stat(int nivel, int stat_base, int cons){  //Devuelve una estadistica individual calculada en base a sus stats base y su nivel. Mandar 0 en cons para las stats que no sean Hp, y mandar nivel en cons para hp
    int variacion = 1 + rand() % (GRADO_VARIATION-1 + 1);
    int stat_calc = ((stat_base * nivel)/50) + cons;
    
    return (stat_calc + variacion);
}


void construir_equipo(ptr_pok dex, Trainer * ptr, int n, int nivel){  //Construye un equipo mediante numeros aleatorios, usando la cons de ejemplares
    Ejemplar * new = malloc(sizeof(Ejemplar));

    if(new==NULL){
        fprintf(stderr,"Error! Codigo de error: 50 (No hay suficiente memoria reservada al apuntador)\n");       //Poner codigo de error
    }

    new->siguiente = NULL;
    construir_ejemplar(dex,new,n-1,nivel);    //Se construye un nodo para el nuevo ejemplar

    if(ptr->trainer_team->longitud == 0){   //Si el equipo esta vacio, se le agrega directamente a la lista
        ptr->trainer_team->equipo = new;
        
    }else{
        Ejemplar * pass;
        pass = ptr->trainer_team->equipo;
        while(pass->siguiente != NULL){     //Se busca el ultimo elemento de la lista para asignarle el nuevo ejemplar
            pass = pass->siguiente;
        }
        pass->siguiente = new;
        
    }
    
   
    ptr->trainer_team->longitud+=1;
    ptr->trainer_team->cant_vivos+=1;
}

void mostrar_equipo(Trainer * ptr){    //Muestra el equipo de un entrenador
    Ejemplar * pass;
    pass = ptr->trainer_team->equipo;       //Apunta a lo que apunta la cabecera del equipo del entrenador
    int i =0;
    while(pass != NULL){
        printf("Especie: %i : \n",i+1);
        mostrar_ejemplar(pass);
        printf("================\n");

        pass= pass->siguiente;
        i++;
    }
}


void asociar_tipos_especie(Pokedex dex, Lista_tipos coleccion_tipos){       //Esta funcion asocia los campos tipo1 y tipo2 a los tipos pokemon del dominio
    int i =0;
    
    while(i<POKES_CANT){
        asigar_tipo(dex,coleccion_tipos,i);
        i++;
    }

}

void asigar_tipo(Pokedex dex, Lista_tipos coleccion, int id){   //A una especie en concreto, se le busca su tipo correspondiente y se le asigna
    int c =0;       //Indice para la coleccion
    int flag2 =0;

    if(comparar_cadena(dex[id].lectura_tipo2,"-")){        //Si el segundo tipo esta vacio
        flag2 = 1;
        dex[id].tipo2.id = -1;
        strcpy(dex[id].tipo2.name,"-");
    }

    while(c<TYPESPOK_CANT){
        if(!flag2){                 //El segundo tipo esta vacio, entonces no hay que buscar        
            if(comparar_cadena(dex[id].lectura_tipo2,coleccion[c].name)){
                dex[id].tipo2 = coleccion[c];
                


            }

        }

        if(comparar_cadena(dex[id].lectura_tipo1,coleccion[c].name)){
            dex[id].tipo1 = coleccion[c];

        }

        c++;   
    }


}


