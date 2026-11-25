#include "stdtrainer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdtypes/stdtypes.h"      //Incluimos la libreria de types tambien. Poke tambien la tiene


void iniciar_trainer(Trainer * sample, char nombre[], int id, int cant_poke){ //Inicia el tipo trainer con sus stats en 0, sin nombre y con el equipo vacio
    sample->cant_derr = 0;
    sample->cant_empat = 0;
    sample->cant_vic = 0;
    sample->score= 0;
    sample->cant_pok_derrot = 0;

    sample->id = id;   //identificador tomado
    strcpy(sample->nombre,nombre);      //copiamos el nombre leido

    sample->trainer_team = malloc(sizeof(Team));    //ARREGLADO 15/11/26 CORTESIA DE LA IA Se le asigno al sizeof el tamano del apuntador a * Team, en vez del struct Team

    if(sample->trainer_team == NULL){
        fprintf(stderr,"Error! Codigo de error: 50 (No hay memoria suficiente para el apuntador a equipo)\n");      //COLOCAR CODIGO DE ERROR
        
    }
  
    iniciar_team(sample->trainer_team,cant_poke);
    
}

void iniciar_team(Team * user, int cant_poke){  //Inicializa el equipo de un entrenador, el apuntador ya tiene una dir valida creada
    user->equipo = NULL;
    user->longitud = 0;
    user->cant_vivos = 0;       //Cantidad de pokemon en pie
    user->base_cantidad = cant_poke;
}

int es_equipo_vacio(Team * sample){ //Devuelve 1 si el equipo esta vacio, 0 en caso contrario
    if(sample->longitud ==0){
        return 1;
    }

    return 0;
}

void cargar_trainers(FILE * base_trainer, lista_trainers arr_trainers, int discri, int cant_poke){     //Carga 32 entrenadores con sus nombres y los inicializa, como fuente usando archivo de entrenadores
    int arr_ids[TRAINERS_TOURNAMENT];
    string_arr nombres;
    
    
    for(int i=0+discri;i<TRAINERS_TOURNAMENT;i++){     //pasamos toda esta informacion a arreglos temp
        fscanf(base_trainer,"%i",&arr_ids[i]);
        fscanf(base_trainer,"%s",nombres[i]);
    }

    //printf("En cargar trainers: %i \n",cant_poke);
    for(int i=0+discri;i<TRAINERS_TOURNAMENT;i++){    

        iniciar_trainer(&arr_trainers[i],nombres[i],arr_ids[i],cant_poke);      //inicializa los 32 trainres por fuera del flujo de archivo
    }   


    rewind(base_trainer);   //Volvemos al inicio del archivo para dejar el apuntador alli
}


void mostrar_trainer(Trainer * user){   //Muestra los datos de un entrenador, mas no muestra su equipo
    printf("Datos de Entrenador: \n");
   
    printf("Nombre: %s\n",user->nombre);
    printf("Id: %i\n",user->id);
    printf("Victorias: %i, Derrotas: %i, Empates: %i\n",user->cant_vic,user->cant_derr,user->cant_empat);
    printf("Puntuacion: %i\n",user->score);
    printf("Pokemon derrotados: %i \n",user->cant_pok_derrot);

}


int calcular_damage(Ejemplar * atacante, Ejemplar * defensor, Tabla_Tipos table){  //Calcula del dano de un ataque, recibiendo los ejemplares atacante y defensor y usando sus estadisticas para el calculo 
    //Variables que la formula utiliza
    float damage;
    int ataque = atacante->atk;
    int nivel = atacante->nivel;
    int def = defensor->def;    
    float efectividad;

    //Ids de los tipos pokemon para ser consultados
    int id_atk = atacante->fijer->tipo1.id;
    int id_def = defensor->fijer->tipo1.id;
    efectividad = devolver_multiplicar_tipo(id_atk,id_def,table);   //Busca la efectividad entre los tipos del atacante y del defensor, operacion de orden O(1)
    
    //Factor de aleatoriedad
    float aleatorio = 0.85f + (rand() % 16) / 100.0f;   //Grado de aleatoriedad del ataque

    //Calculo del dano final y que sera retornado como entero

    damage = (ataque * nivel / (def + 10)) * efectividad;

    damage*=aleatorio;      //Se le multiplica por el factor aleatorio. Este es el dano final con los tipos 1 obligatorios del atacante y del defensor

    if(defensor->fijer->tipo2.id != -1){    //Si el defensor tiene segundo tipo
        id_def = defensor->fijer->tipo2.id;
        efectividad = devolver_multiplicar_tipo(id_atk,id_def,table);

        damage*=efectividad;        //Se multiplica el valor damage por la efectividad que pueda tener el segundo tipo del defensor respecto al primer tipo del atacante

    }

    if(atacante->fijer->tipo2.id != -1){    //Si el atacante posee segundo tipo
        id_atk = atacante->fijer->tipo2.id;        
        id_def = defensor->fijer->tipo1.id;     //Agarramos el id del primer tipo

        efectividad = devolver_multiplicar_tipo(id_atk,id_def,table);

        damage*=efectividad;        //Nuevo multiplicador operado

        if(defensor->fijer->tipo2.id != -1){    //Si el defensor tiene segundo tipo
            id_def = defensor->fijer->tipo2.id;
            efectividad = devolver_multiplicar_tipo(id_atk,id_def,table);

            damage*=efectividad;        //Se multiplica el valor damage por la efectividad que pueda tener el segundo tipo del defensor respecto al primer tipo del atacante
        }

    }


    int resultado = (int)damage;     //Casteo a entero para eliminar decimales
    return resultado;
}

void turno_generico(Ejemplar * atk, Ejemplar * def,  Tabla_Tipos table){      //Pasa un turno individual, en donde el ejemplar A ataca al ejemplar B y se disminuye unicamente la vida. EL ejemplar A sera el mas rapido
    int dmg=0;

    if(atk->is_alive){
        int flag = se_excluyen_mutua(atk,def,table);
        if(flag){
            def->hp-=DMG_COMBAT_GEN;        //En caso de que se excluyan mutuamente, se haran un dano de combate fijo de 10

        }else{
            dmg = calcular_damage(atk,def,table);
            dmg = (dmg < 0) ? 1 : dmg;            //Si el damage recibido es menor que 0, se le pone le valor mini
            def->hp-=dmg;  

            //printf("%s le hace %i de damage a %s \n",atk->fijer->nombre_specie,dmg,def->fijer->nombre_specie);
            //_sleep(5000);   //Espera 5 segundos

        }

        //printf("Dmg efectuado: %i\n",dmg);
    }
   
    //Le restamos el dano a los puntos de salud del defensor   

}

int se_excluyen_mutua(Ejemplar * pokemon_1, Ejemplar * pokemon_2, Tabla_Tipos table){ //Esta funcion comprueba si dos Pokemon no pueden atacarse debido a la nulidad de sus tipos
    int flag =0;
    int id_tipo = pokemon_1->fijer->tipo1.id;   //Primer tipo del primer pokemon
    int id_comp = pokemon_2->fijer->tipo1.id;   //Compararemos con el primer tipo del segundo pok
    
    if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
        flag++;
    }

    id_comp = pokemon_2->fijer->tipo2.id;
    if(!(id_comp == -1)){           //Si tiene segundo tipo

        if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
            flag++;
        }
    }

    id_tipo = pokemon_2->fijer->tipo1.id;          //id tipo contiene el primer tipo de pok 2
    id_comp = pokemon_1->fijer->tipo1.id;           //Contiene el tipo 1 del pokemon 1 

    if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
        flag++;

    }

    id_comp = pokemon_1->fijer->tipo2.id;
    if(!(id_comp == -1)){           //Si tiene segundo tipo
        if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
            flag++;
        }
    }
    
    id_tipo = pokemon_2->fijer->tipo2.id;
    id_comp = pokemon_1->fijer->tipo1.id;

    if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
        flag++;

    }

    id_comp = pokemon_1->fijer->tipo2.id;
    if(!(id_comp == -1)){           //Si tiene segundo tipo
        if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
            flag++;
        }
    }

    id_tipo = pokemon_1->fijer->tipo2.id;
    id_comp = pokemon_2->fijer->tipo1.id;

    if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){                                //Cambiar codigo mierdero
        flag++; 

    }

    id_comp = pokemon_2->fijer->tipo2.id;
    if(!(id_comp == -1)){           //Si tiene segundo tipo
        if(devolver_multiplicar_tipo(id_tipo,id_comp,table)==0){
            flag++;
        }
    }


    if(flag >= 2){      //Hubo exclusion mutua si se llego a sumar dos o mas flags
        return 1;   
    }


    return 0;
}



void combate_gen(Ejemplar * oponente1, Ejemplar * oponente2, Tabla_Tipos table){   //Combate entre dos ejemplares en donde suceden los turnos
    Ejemplar * vaprimero = NULL;
    Ejemplar * vasegundo = NULL;
  
    if(oponente1->vel == oponente2->vel){   //Si las velocidades son iguales, hay que hacer desempate
        int rnd = 1 + rand() % (2 - 1 + 1);
        if(rnd == 1){
            vaprimero = oponente1;          //Si salio 1, ira primero oponente 1, caso contrario ira oponente 2 de primero
            vasegundo = oponente2;
        }else{
            vaprimero = oponente2;
            vasegundo = oponente1;

        }

    }else if(oponente1->vel > oponente2->vel){  //Si la velocidad de op1 es mayor que la de op2, ira primero op1 y despues op2
        vaprimero = oponente1;
        vasegundo = oponente2;
    }else{
        vaprimero = oponente2;
        vasegundo = oponente1;

    } 

    //printf("%s ataca primer turno \n",vaprimero->fijer->nombre_specie);
    //_sleep(3000);
    turno_generico(vaprimero,vasegundo,table);      //Llamada a los turnos, donde ataca el que va primero.    

    if(vasegundo->hp <= 0){      //Despues del primer turno evaluamos la vida del pokemon atacado
        vasegundo->is_alive = 0;
       // printf("%s ha sido debilitado! \n",vasegundo->fijer->nombre_specie);


    }else{
        //printf("%s ataca segundo turno \n",vasegundo->fijer->nombre_specie);
        //_sleep(3000);
        turno_generico(vasegundo,vaprimero,table); 

    }

    if(vaprimero->hp < 0){  //Evaluamos la vida del pokemon atacado despues del turno
        vaprimero->is_alive = 0;            //Pokemon debilitadop
        //printf("%s ha sido debilitado! \n",vaprimero->fijer->nombre_specie);
    }

        
}


void sala_trainers(Trainer * combatiente1, Trainer * combatiente2, Tabla_Tipos table, FILE * hist){     //Sala donde se produce un combate entre dos entrenadores
    Ejemplar * pass_comb1=  NULL;   //Para recorrer la lista de ejemplares
    Ejemplar * pass_comb2 = NULL;      
    pass_comb1 = combatiente1->trainer_team->equipo;        //Cada pass apunta al primer pokemon de ambos entrenadores
    pass_comb2 = combatiente2->trainer_team->equipo; 

    //printf("Entrenadores a enfrentarse: %s, %s \n",combatiente1->nombre,combatiente2->nombre);

    fprintf(hist,"| Combate! Entrenadores a enfrentarse: %s , %s |  ",combatiente1->nombre,combatiente2->nombre);
    
    while((posee_pok_vivos(combatiente1)==1) && (posee_pok_vivos(combatiente2)==1)   ){    //Verifica si ambos entrenadores tienen pokemon en pie

        //printf("\n"); 
        //printf("[Pokemon enviado por %s: %s ]\n",combatiente1->nombre,pass_comb1->fijer->nombre_specie);
        //printf("[Pokemon enviado por %s: %s ]\n",combatiente2->nombre,pass_comb2->fijer->nombre_specie);
        //_sleep(5000);   //Espera 5 segundos
        combate_gen(pass_comb1,pass_comb2,table);

        if(pass_comb1->hp <=0){
            combatiente1->trainer_team->cant_vivos--;
            combatiente2->cant_pok_derrot++;
            pass_comb1 = pass_comb1->siguiente;     //Se pasa al siguiente pokemon


        }else if(pass_comb2->hp <=0){
            combatiente2->trainer_team->cant_vivos--;   //Se disminuye la cantidad de vivos
            combatiente1->cant_pok_derrot++;            //Aumenta la cantidad de pokemon que derroto
            pass_comb2 = pass_comb2->siguiente;
            
        }



    }
 
    if(combatiente1->trainer_team->cant_vivos > 0){
        //printf("Ganador del combate: %s\n",combatiente1->nombre);
        combatiente1->cant_vic++;
        combatiente1->score+=POINT_BY_WIN;         //Una victoria son 3 puntos

        combatiente2->cant_derr++;      //Aumenta cantidad de derrotas de combatiente 2 si gana el comb 1
        fprintf(hist,"[Ganador de este combate: %s ]\n",combatiente1->nombre);
    }else{
        combatiente1->cant_derr++;
        combatiente1->score+=POINT_BY_LOSE;         //Una derrota, 0 puntos

        combatiente2->score+=POINT_BY_WIN;         //Aumenta cantidad de cosas de combatiente 2 porque gano
        combatiente2->cant_vic++;

        fprintf(hist,"[Ganador de este combate: %s ]\n",combatiente2->nombre);

        //printf("Ganador del combate: %s\n",combatiente2->nombre);
    }

    
    //printf("Pokemon vivos de comb1 : %i\n",combatiente1->trainer_team->cant_vivos);
    //printf("Pokemon vivos de comb2 : %i\n",combatiente2->trainer_team->cant_vivos);
    //fprintf(hist,"Se realizo un combate xd \n");

    fprintf(hist,"\n");

    curar_equipo(combatiente1);
    curar_equipo(combatiente2);
    //Curamos a los combatientes terminado el combate para el proximo
    
}


int posee_pok_vivos(Trainer * combatiente){   //Retorna 1 si el entrenador tiene pokemones en pie todavia
    if(combatiente->trainer_team->cant_vivos > 0){
        return 1;
    }

    return 0;
}

void curar_equipo(Trainer * ptr){       //Esta funcion recibe a un entrenador y recupera a su equipo
    Ejemplar * pass = NULL;      

    pass = ptr->trainer_team->equipo;
    ptr->trainer_team->cant_vivos = ptr->trainer_team->base_cantidad;  //Regresa a tener la cantidad de vivos de su equipo

    while(pass != NULL){
        pass->is_alive = 1;
        pass->hp = pass->hp_real;   
        pass = pass->siguiente;   
    }

}

//Desde aqui comienza el torneo 

void asignar_matriz(tabla_grupos grupos, lista_trainers arr_entrenadores){       //Cada fila sera un grupo en la matriz, 8 filas, 8 grupos
    int f = 0;          //Controla las filas
    int c = 0;          //Controla mis columnas
    int i = 0;          //Indice para el arreglo de trainers

    for(f=0;f<FIL_GROUP;f++){
        for(c=0;c<COL_GROUP;c++){
            grupos[f][c] = arr_entrenadores[i];
            i++;

        }
       
    }

}


void gran_torneo(tabla_grupos grupos, lista_trainers arr_entrenadores, Tabla_Tipos table, combats_result resultados, podio * posicion_podio, FILE * historial, Cola_torneo * cola){     //Funcion arbol en donde se desarrollan las fases del torneo
    //Preparativos
    string_letras tags = "ABCDEFGH";
    arreglo_trancision arreglo_grupos;      //Este es un arreglo de trancision para recoger los primer y segundo lguar de cada grupo

    /*
    printf("entrenadores antes de matriz \n");
    for(int i=0;i<TRAINERS_TOURNAMENT;i++){
        printf("%s  %i \n",arr_entrnadores[i].nombre,arr_entrnadores[i].score);
    }
    */
    asignar_matriz(grupos,arr_entrenadores);         

    //Mostraremos lo que tiene la matriz
    /*
    int f = 0;          //Controla las filas
    int c = 0;          //Controla mis columnas
    for(f=0;f<FIL_GROUP;f++){
        for(c=0;c<COL_GROUP;c++){       //col group es 4 (entrenadores por grupo)
            printf("%s  ",grupos[f][c].nombre);

    
        }   
        printf("\n");
    }
    printf("\n\n");
    */


    
    fprintf(historial,"|===COMBATES DE LA FASE DE GRUPOS===|\n");
    //ciclo para fase grupos
    for(int i=0;i<FIL_GROUP;i++){
        fase_grupos(grupos,table,i,historial);        //Ciclo para la fase de grupos
    }
    
    asigar_grupos_trancision(grupos,tags,arreglo_grupos,cola);       //Se asignan los ganadores de cada grupo a un vector de trancision para ser procesados
    
    /*
    for(int i=0;i<FIL_GROUP;i++){       //Imprime los resultados cargados
        printf("%i ",resultados[i].id_combat);
        printf("%i%c - %i%c\n",resultados[i].comb1.pos,resultados[i].comb1.grupo
            ,resultados[i].comb2.pos,resultados[i].comb2.grupo);

    }
    printf("\n\n");

    for(int i=0;i<FIL_GROUP;i++){       //Imprime el arreglo de grupos 
        printf("Grupo: %c. ",arreglo_grupos[i].grupo);
        printf("%i %s ||| ",arreglo_grupos[i].i_first,arreglo_grupos[i].trainer_first.nombre);

        printf("%i %s\n",arreglo_grupos[i].j_second,arreglo_grupos[i].trainer_second.nombre);

    }
    */

    fprintf(historial,"|===COMBATES DE LA FASE DE OCTAVOS DE FINAL===|\n");
    fase_octavos(arreglo_grupos,table,resultados,historial,cola);    //Se realizan los octavos de final 

    /*
    for(int i=0;i<FIL_GROUP;i++){       //Imprime lo que ira a los cuartos de final
        printf("Tag: %c , Id: %i\n",arreglo_grupos[i].tag_winner,arreglo_grupos[i].id_combat);
        printf("Ganador de esa sala: %s\n",arreglo_grupos[i].trainer_first.nombre);

    }
    */

   
    fprintf(historial,"|===COMBATES DE LA FASE DE CUARTOS DE FINAL===|\n");
    fase_cuartos(arreglo_grupos,table,resultados,historial,cola);

    /*
    printf("Entrenadoes que iran a semifinal: \n");
    for(int i=0;i<4;i++){      //Se imprimen los que van a seminfinales
        printf("%i , %s \n",arreglo_grupos[i].id_combat,arreglo_grupos[i].trainer_first.nombre);
  
    }   
    printf("\n\n");
    */
    fprintf(historial,"|===COMBATES DE LA FASE DE SEMIFINALES===|\n");
    fase_semis(arreglo_grupos,table,resultados,historial,cola);

    /*
    printf("Finalistas: \n");

    for(int a=0;a<2;a++){      
        printf("%i , %s  ",arreglo_grupos[a].id_combat,arreglo_grupos[a].trainer_first.nombre);
        printf("Tag: %c \n",arreglo_grupos[a].tag_winner);


    }   

    printf("Perdedores: \n");

    for(int a=2;a<4;a++){      
        printf("%i , %s  ",arreglo_grupos[a].id_combat,arreglo_grupos[a].trainer_first.nombre);
        printf("Tag: %c \n",arreglo_grupos[a].tag_winner);
    }

    printf("\n");
    */
    fprintf(historial,"|===COMBATES DE LA FASE DE FINALISTAS===|\n");
    fase_final(arreglo_grupos,table,posicion_podio,historial);   //Se produce la fase final donde se disputa el campeon, el segundo lugar y sucesivos

    encolar_tournam(cola,posicion_podio->champ);        //Encolamos a los 4 restantes. Todos sus datos deben estar actualizados
    encolar_tournam(cola,posicion_podio->subchamp);
    encolar_tournam(cola,posicion_podio->third);
    encolar_tournam(cola,posicion_podio->four);

    printf("Torneo realizado exitosamente! Puede revisar el podio para ver ganadores, o el historial de combates \n");

}  


void fase_grupos(tabla_grupos grupos, Tabla_Tipos table, int f, FILE * historial){ //Se realizan 6 enfrentamientos en un grupo. Se hace hardcode por motivos de comodidad y legibilidad. Recibe el indice de la fila
    sala_trainers(&grupos[f][0],&grupos[f][1],table,historial);   //Combate 1                   
    sala_trainers(&grupos[f][0],&grupos[f][2],table,historial);   //Combate 2
    sala_trainers(&grupos[f][0],&grupos[f][3],table,historial);   //Combate 3

    sala_trainers(&grupos[f][1],&grupos[f][2],table,historial);       //Combate 4
    sala_trainers(&grupos[f][1],&grupos[f][3],table,historial);       //Combate 5
    sala_trainers(&grupos[f][2],&grupos[f][3],table,historial);       //Combate 6

    quick_sort_trainer(grupos,f,COL_GROUP-1,0);        //Ordenamos el grupo segun su score.   
    //Los resagados seran encolados con su informacion para ser tratados despues
    
}

void fase_octavos(arreglo_trancision arr_grupos, Tabla_Tipos table, combats_result resultados, FILE * historial, Cola_torneo * cola){  //Pasamos la cola del torneo y el arreglo que contiene los ganadores de cada grupo 
    Trainer sample1;
    Trainer sample2;
    arreglo_trancision arr_grupo_cuartos;       //Un arreglo paralelo para almacenar los entrenadores que ganaorn octavos

    //debug_resultados(resultados,arr_grupos,8,8);     

    for(int i=0;i<FIL_GROUP;i++){
        buscar_grupo(i,arr_grupos,resultados,&sample1,&sample2);
        int vics_a = sample1.cant_vic;   //Variables para almacenar la cant idad de victorias
        int vics_b = sample2.cant_vic; 
        sala_trainers(&sample1,&sample2,table,historial); 

        if(sample1.cant_vic > vics_a){   //Gano quien esta en sample_1, //Ahora guardaremos en el mismo vector de trancision, el ganador
            arr_grupo_cuartos[i].tag_winner = 'G';          //G de ganador
            arr_grupo_cuartos[i].id_combat = resultados[i].id_combat; //Id del combate que se gano
            arr_grupo_cuartos[i].trainer_first = sample1;//Ahora trainer first contendra el ganador de ese combate, el otro no importa

            encolar_tournam(cola,sample2);  //Encolamos a los perdedores

        }else if(sample2.cant_vic > vics_b){        //Gano el otro trainer
            arr_grupo_cuartos[i].tag_winner = 'G';     
            arr_grupo_cuartos[i].id_combat = resultados[i].id_combat;    
            arr_grupo_cuartos[i].trainer_first = sample2;  
    
            encolar_tournam(cola,sample1);      //Encolamos a los perdedores

        }
        
    }

    //Pasaremmos la informacion recolectada del arreglo paralelo al arreglo de trancision para el futuro   
    //Se pasaran 8 entrenadores de los 16 combates disputados
    copiar_arreglo_grupo(arr_grupos,arr_grupo_cuartos);

    /*
    for(int i=0;i<FIL_GROUP;i++){
        liberar_equipo_trainer(&arr_grupos[i].trainer_second);      //Liberamos a los entrenadores desclasificados: los trainer second ya no importan (sus equipos)

    }
    */

}

void fase_cuartos(arreglo_trancision arr_grupos, Tabla_Tipos table, combats_result resultados, FILE * historial, Cola_torneo * cola){ //Parecido a la fase de octavos, esta funcion busca los trainers relacionados a los resultados y se hacen los combates
    Trainer sample1;
    Trainer sample2;
    arreglo_trancision arr_grupo_semis;       //Un arreglo paralelo para almacenar los entrenadores que ganaron los cuartos
    int i;      //Empieza en 8
    int j =0;
    
    for(i=8;i<12;i++){
        buscar_grupo_semis(i,arr_grupos,resultados,&sample1,&sample2);
        int vics_a = sample1.cant_vic;   //Variables para almacenar la cant idad de victorias
        int vics_b = sample2.cant_vic; 

        sala_trainers(&sample1,&sample2,table,historial); 

        if(sample1.cant_vic > vics_a){   //Gano quien esta en sample_1, //Ahora guardaremos en el mismo vector de trancision, el ganador
            arr_grupo_semis[j].tag_winner = 'G';          //G de ganador
            arr_grupo_semis[j].id_combat = resultados[i].id_combat; //Id del combate que se gano
            arr_grupo_semis[j].trainer_first = sample1;//Ahora trainer first contendra el ganador de ese combate, el otro no importa


            encolar_tournam(cola,sample2);  //Encolamos a los perdedores


        }else if(sample2.cant_vic > vics_b){        //Gano el otro trainer
            arr_grupo_semis[j].tag_winner = 'G';     
            arr_grupo_semis[j].id_combat = resultados[i].id_combat;    
            arr_grupo_semis[j].trainer_first = sample2;  

            encolar_tournam(cola,sample1);  //Encolamos a los perdedores

        }
        j++;        //Aumentamos el indice de j para que se almacenen correctamente los ganadores
    }

    //copiaremo el arreglo de semis al de grupos para el futuro
    copiar_arreglo_grupo(arr_grupos,arr_grupo_semis);
    
}

void fase_semis(arreglo_trancision arr_grupos, Tabla_Tipos table, combats_result resultados, FILE * historial, Cola_torneo * cola){      //Similar a la anterior, esta funcion buscara los trainers ganadores de los resultados cargados, para efectuar las semis y dejar al 3ero y al 4to al final, para realizar la final del 1ero y el 2do
    Trainer sample1;
    Trainer sample2;
    arreglo_trancision arr_grupo_final;       //Un arreglo paralelo para almacenar los entrenadores que ganaron los cuartos
    int i;      //Empieza en 12 
    int j =0;
    int indice_desplz = 2;  //Se usara para guardar el tercer y el cuarto lugar


    for(i=12;i<14;i++){                 //Se realizan los ultimos dos combates
        buscar_grupo_semis(i,arr_grupos,resultados,&sample1,&sample2);              //usamos la misma funcion pues todavia sirve
        int vics_a = sample1.cant_vic;   //Variables para almacenar la cant idad de victorias
        int vics_b = sample2.cant_vic; 

        //printf("Combatiran: %s y %s \n",sample1.nombre,sample2.nombre);

        sala_trainers(&sample1,&sample2,table,historial); 
       

        if(sample1.cant_vic > vics_a){   //Gano quien esta en sample_1, //Ahora guardaremos en el mismo vector de trancision, el ganador
            arr_grupo_final[j].tag_winner = 'G';          //G de ganador
            arr_grupo_final[j].id_combat = resultados[i].id_combat; //Id del combate que se gano
            arr_grupo_final[j].trainer_first = sample1;//Ahora trainer first contendra el ganador de ese combate, el otro no importa

            arr_grupo_final[indice_desplz].trainer_first = sample2;     //Se coloca al perdedor en la tercera posicion
            arr_grupo_final[indice_desplz].tag_winner = 'P';        //Perdedor 
            arr_grupo_final[indice_desplz].id_combat = resultados[i].id_combat;

            encolar_tournam(cola,sample2);


        }else if(sample2.cant_vic > vics_b){        //Gano el otro trainer
            arr_grupo_final[j].tag_winner = 'G';     
            arr_grupo_final[j].id_combat = resultados[i].id_combat;    
            arr_grupo_final[j].trainer_first = sample2;  

            
            arr_grupo_final[indice_desplz].trainer_first = sample1;     //Se coloca al perdedor en la tercera posicion
            arr_grupo_final[indice_desplz].tag_winner = 'P';        //Perdedor 
            arr_grupo_final[indice_desplz].id_combat = resultados[i].id_combat;
        
            encolar_tournam(cola,sample1);
        }
        j++;        //Aumentamos el indice de j para que se almacenen correctamente los ganadores
        indice_desplz++;

    }
 

    copiar_arreglo_grupo(arr_grupos,arr_grupo_final);       //Copiamos los resultados en el arreglo de grupos. 1 y 2 posiciones son los finalistas

}

void fase_final(arreglo_trancision arr_grupos, Tabla_Tipos table, podio * posicion_podio, FILE * historial){      //Funcion final del torneo. Se disputan el combate para determinar el campeon y el resto de posiciones 
    //El arreglo de trancision contiene: 1 y 2 ganadores de la semifinales al principio, y 3 y 4 perdedores de las semis
    arreglo_trancision resultados_final;    
    Trainer sample1;
    Trainer sample2;
    int comb_ciclo=0;   
    int j;
    int cant_vics_1;    //Para conocer la cantidad de victorias de cada trainer
    int cant_vics_2;    


    for(int i=0;i<2;i++){
        if(comb_ciclo==0){  //Esto simplemente alterna entre los indices 1 y 2 del arreglo, o los 3 y 4, para tomar los entrenadores que van a enfrentarse
            j = 2;
        }else{
            j = 0;
        }

        sample1 = arr_grupos[j].trainer_first;      //Perdedores de las seminfinales se enfrentaran
        sample2 = arr_grupos[j+1].trainer_first;

        cant_vics_1 = sample1.cant_vic;
        cant_vics_2 = sample2.cant_vic;
        sala_trainers(&sample1,&sample2,table,historial);

        if(sample1.cant_vic > cant_vics_1){ //Gano quien estaba en sample1, sera el tercer lugar
            resultados_final[j].trainer_first = sample1;
            resultados_final[j+1].trainer_first = sample2;    //Perdedor en 4to lugar

        }else if(sample2.cant_vic > cant_vics_2){
            resultados_final[j].trainer_first = sample2;
            resultados_final[j+1].trainer_first = sample1;

        }



        comb_ciclo++;
    }   


    //Pasaremos los 4 ganadores del torneo al podio para mostrarlo despues
    posicion_podio->champ =resultados_final[0].trainer_first;
    posicion_podio->subchamp =resultados_final[1].trainer_first;
    posicion_podio->third = resultados_final[2].trainer_first;
    posicion_podio->four = resultados_final[3].trainer_first;

}


void buscar_grupo_semis(int i, arreglo_trancision arr_restantes, combats_result resultados, Trainer * a, Trainer * b){      //Busca los entrenadores para las semifinales, tambien puede ser reutilizado para otras fases
    int id_a = resultados[i].comb1.pos;     //La posicion en este caso representa el id del combate que ganaron
    int id_b = resultados[i].comb2.pos;

    int flag1=0;
    int flag2=0;
    int j=0;

    while(!(flag1) || !(flag2)){        //mientras no se hayan encontrado los dos

        if(id_a == arr_restantes[j].id_combat){
            *a = arr_restantes[j].trainer_first;
            flag1=1;
        }

        if(id_b == arr_restantes[j].id_combat){
            *b = arr_restantes[j].trainer_first;
            flag2=1;
        }
        
        if(j>LEN_RESULTS){       //12 es la barrera perteneciente a los resultados de las semis 
            break;
        }

        j++;
       
    }

    //printf("Valor de j: %i\n",j);

}

void copiar_arreglo_grupo(arreglo_trancision destino, arreglo_trancision fuente){ //Copia la informacion de un arreglo de entrenadores ganadores de octavos a otro arreglo

    for(int i=0;i<FIL_GROUP;i++){
        destino[i].tag_winner = fuente[i].tag_winner;   //Caracter de ganador
        destino[i].id_combat = fuente[i].id_combat;     //Id del combate
        destino[i].trainer_first = fuente[i].trainer_first; //Entrenador ganador. El resto de campos no importan
    
    }


}

void buscar_grupo(int i, arreglo_trancision arr_grupos, combats_result results, Trainer * a, Trainer * b){    //Busca en el arreglo de trancision los entrenadores a combatir
    char grupo_1 = results[i].comb1.grupo;
    char grupo_2 = results[i].comb2.grupo;
    int id_pos1 = results[i].comb1.pos;     //Datos para buscar a los dos trainers
    int id_pos2 = results[i].comb2.pos;

    int flag1 = 0;
    int flag2 = 0;
    int j=0;

    while(!(flag1) || !(flag2)){        //mientras no se hayan encontrado los dos

        if(grupo_1 == arr_grupos[j].grupo){         //Si el tag coincide con el grupo presente en la posicion grupos[i]
            if(id_pos1 == 1){
                *a = arr_grupos[j].trainer_first;       //Si el id del resultado es 1 , mandamos al trainer_first


            }else{                  //Caso contrario. Se manda la direccion de trainer a donde apunta b
                *a = arr_grupos[j].trainer_second;

            }


            flag1 = 1;
        }

        if(grupo_2 == arr_grupos[j].grupo){         //Si el tag coincide con el grupo presente en la posicion grupos[i]
            if(id_pos2 == 1){
                *b = arr_grupos[j].trainer_first;       //Si el id del resultado es 1 , mandamos al trainer_first


            }else{                  //Caso contrario. Se manda la direccion de trainer a donde apunta b
                *b = arr_grupos[j].trainer_second;

            }


            flag2 = 1;
        }


        j++;

        if(j>FIL_GROUP){        //Si el indice llega a acceder a posiicones inexistentes del arreglo    
            break;
        }

    }


}



void asigar_grupos_trancision(tabla_grupos grupos, string_letras tags, arreglo_trancision arr, Cola_torneo * cola){    //Asigna los primer y segundo lugar de cada grupo a un arreglo de estos 16 entrenadores.
    int j =0;
    int primer_por_grupo = 4-1;
    int segundo_por_grupo = 3-1;

    int tercer = 2-1;  
    int cuarto = 1-1;

    for(int i=0;i<FIL_GROUP;i++){
        arr[j].trainer_first = grupos[i][primer_por_grupo];        
        arr[j].i_first = 1;         //Primer lugar del grupo actual

        arr[j].trainer_second = grupos[i][segundo_por_grupo];
        arr[j].j_second = 2;            //Segundo lugar del grupo actual

        arr[j].grupo = tags[i];

        encolar_tournam(cola,grupos[i][tercer]);
        encolar_tournam(cola,grupos[i][cuarto]);
        j++;
    }  

    /*
    //Comprobamos que los perdedores se encolen correctamente, junto a sus datos
    while(!es_cola_empty(cola)){
        Trainer new = desencolar_tournam(cola);
        printf("Nombre: %s %i %i \n",new.nombre,new.cant_vic,new.cant_derr);

    }
    */

}









//La siguiente funcion sera para ordenar un arrelgo pequeno de 4 entrenadores segun sus puntos

void quick_sort_trainer(tabla_grupos grupo_fila, int fila, int high, int low){          //Quicksort recursivo para ordenar un arreglo pequeno de 4 entrenadores segun su score
    if(low < high){
        int indic = quick_partition(grupo_fila,fila,high,low);
        
        quick_sort_trainer(grupo_fila,fila,indic-1,low);
        quick_sort_trainer(grupo_fila,fila,high,indic+1);
    }

}

int quick_partition(tabla_grupos grupo_fila, int fila, int high, int low){      //FUncion de particion para Quicksort
    Trainer pivot = grupo_fila[fila][high];     
    int i = low -1;

    for(int j= low; j<=high-1;j++){
        if(comparar_trainer_quick(&grupo_fila[fila][j],&pivot)){
            i++;
            swap_quick(&grupo_fila[fila][i],&grupo_fila[fila][j]);
        }

    }

    swap_quick(&grupo_fila[fila][i+1],&grupo_fila[fila][high]);

    return (i+1);
}


void swap_quick(Trainer * a, Trainer * b){          //Hace un swap entre dos entrenadores
    Trainer temp = *a;
    *a = *b;
    *b = temp;

}

int comparar_trainer_quick(Trainer * a, Trainer * b){       //En caso de que exista un empate por score, se desempata mediante la cantidad de pok derrotados
    if(a->score != b->score)
        return a->score < b->score;

    // Desempate: más pokémon derrotados primero            //Cortesia de la IA
    return a->cant_pok_derrot < b->cant_pok_derrot;
    
}

void quick_sort_trainer_list(lista_trainers arr_trainers, int n, int high, int low){          //Quicksort recursivo para ordenar un arreglo pequeno de 4 entrenadores segun su score
    if(low < high){
        int indic = quick_partition_list(arr_trainers,n,high,low);
        
        quick_sort_trainer_list(arr_trainers,n,indic-1,low);
        quick_sort_trainer_list(arr_trainers,n,high,indic+1);
    }

}


int quick_partition_list(lista_trainers arr_trainers, int n, int high, int low){      //FUncion de particion para Quicksort
    Trainer pivot  = arr_trainers[high];

    int i = low -1;

    for(int j= low; j<=high-1;j++){

        

        if(comparar_trainer_quick(&arr_trainers[j],&pivot)){
            i++;
            swap_quick(&arr_trainers[i],&arr_trainers[j]);
            
        }

    }
    swap_quick(&arr_trainers[i+1],&arr_trainers[high]);
    return (i+1);
}











//Funciones de utilidad

void imprimir_grupos(tabla_grupos grupos, string_letras tags){     //Recibe la matriz de los grupos e imprime victorias, score, etc de cada entrenador por grupo

    for(int i=0;i<FIL_GROUP;i++){
        printf("Grupo: %c\n",tags[i]);
        for(int j=0;j<COL_GROUP;j++){
            printf("Nombre: %s ",grupos[i][j].nombre);
            printf("Score: %i || ",grupos[i][j].score);
            printf("Pok derrotados: %i  || ",grupos[i][j].cant_pok_derrot);
            printf("Vic: %i Derr: %i\n",grupos[i][j].cant_vic,grupos[i][j].cant_derr);
    
        }
          printf("\n");

    }



}


void mostrar_estado(Trainer * ptr){      //Muestra el estado de un entrenador
    Ejemplar * pass = NULL;
    if(ptr->trainer_team != NULL){
        pass = ptr->trainer_team->equipo;
    }
    printf("Entrenador: %s =========\n",ptr->nombre);
    printf("Pokemon en pie: %i\n",ptr->trainer_team->cant_vivos);
    printf("Longitud del equipo: %i\n",ptr->trainer_team->longitud);
    while(pass != NULL){
        printf("Vida de Pok: %i, Vida real del ejemplar: %i",pass->hp,pass->hp_real);
        printf("\n");
        pass = pass->siguiente;
    }


}

void debug_resultados(combats_result resultados, arreglo_trancision arr_grupos, int len_grupos, int len_results){
    for(int i=0;i<len_results;i++){       //Imprime los resultados cargados
        printf("%i ",resultados[i].id_combat);
        printf("%i%c - %i%c\n",resultados[i].comb1.pos,resultados[i].comb1.grupo
            ,resultados[i].comb2.pos,resultados[i].comb2.grupo);

    }
    printf("\n\n");

    for(int i=0;i<len_grupos;i++){       //Imprime el arreglo de grupos 
        printf("Grupo: %c. ",arr_grupos[i].grupo);
        printf("%i %s ||| ",arr_grupos[i].i_first,arr_grupos[i].trainer_first.nombre);

        printf("%i %s\n",arr_grupos[i].j_second,arr_grupos[i].trainer_second.nombre);

    }
    printf("\n\n");

    
}





//Siguientes funciones para la cola de entrenadores

int es_cola_empty(Cola_torneo * cola_torn){ //Devuelve 1 o 0 si la cola esta vacia
    if(cola_torn->frente ==NULL){
        return 1;
    }

    return 0;

}

void iniciar_cola_tournam(Cola_torneo * cola_torn){  //Esta funcion inicializa la cola vacia
    cola_torn->frente = NULL;
    cola_torn->final = NULL;
}

void encolar_tournam(Cola_torneo * cola_torn, Trainer dato){
    nodo_cola * new;
    new = malloc(sizeof(nodo_cola));

    new->user_train = dato;     //Se le asigna a la cola un entrenador
    new->siguiente = NULL;

    if(es_cola_empty(cola_torn)){
        cola_torn->frente = new;
        cola_torn->final = new;

    }else{
        cola_torn->final->siguiente = new;
        cola_torn->final = new;

    }


}

Trainer desencolar_tournam(Cola_torneo * cola_torn){       //Desencola la cola y devuelve un entrenador
    Trainer dato = cola_torn->frente->user_train;       //Colocamos el entrenador 
    nodo_cola * aux = cola_torn->frente;

    cola_torn->frente = cola_torn->frente->siguiente;       

    if(cola_torn->frente == NULL){      //Si es que la cola se desencola completamente
        cola_torn->final = NULL;
    }


    free(aux);      //Liberamos el nodo y retornamos al entrenador
    return dato;
}


void mostrar_equipo_trainer(Trainer * ptr){     //Imprime por pantalla el equipo de un entrenador, especie po respecie
    Ejemplar * pass;
    pass = ptr->trainer_team->equipo;       //Apunta a lo que apunta la cabecera del equipo del entrenador
    int i =0;

    if(pass == NULL){
        printf("Entrenador no tiene equipo, %i \n",ptr->trainer_team->longitud);
    }

    printf("Cantida que tiene: %i \n",ptr->trainer_team->longitud);
    while(pass != NULL){
        printf("Ejemplar: %i: \n",i+1);
        mostrar_ejemplar_trainer(pass);
        printf("================\n");

        pass= pass->siguiente;
        i++;

        if(i>5){
            break;
        }


    }


}

void mostrar_ejemplar_trainer(Ejemplar * sample){       //Muestra un ejemplar pokemon 
    printf("Especie: %s\n",sample->fijer->nombre_specie);
    printf("Nivel: %i\n",sample->nivel);
    printf("Stats: %i %i %i %i\n",sample->hp,sample->atk,sample->def,sample->vel);
    printf("ID de Especie: %i\n",sample->fijer->ide);

}


void liberar_equipo_trainer(Trainer * sample){      //Libera la lista enlazada (equipo) de un entrenador que ya no va a participar  
    Ejemplar * aux;
    Ejemplar * pass = sample->trainer_team->equipo;

    while(pass != NULL){
        aux = pass;
        aux->fijer = NULL;      //El apuntador a su especie tambien va a nulll
        pass = pass->siguiente;


        free(aux);      //Liberamos el nodo
    }

    sample->trainer_team->cant_vivos = 0;
    sample->trainer_team->equipo = NULL;        //Ahora el entrenador no tieene equipo, lo mandamos a null para no tener nada colgando
    sample->trainer_team->longitud = 0; 


}

void liberar_cabeza_trainer(Trainer * sample){      //Se libera el apuntador a *Team que cada entrenador posee
    Team * aux;
    aux = sample->trainer_team;
    
    if(aux!= NULL){
        free(aux);          //liberamos
        
    } 
    
    sample->trainer_team = NULL;        //Ponemos a null para no tener apuntadores colgnates

    aux = sample->trainer_team;
}


void imprimir_lista_entrenadores(lista_trainers arr_trainers, int cant_particip){     //Imprime en pantalla todos los entrenadores leidos del archivo
    for(int i=0;i<cant_particip;i++){
        printf("============\n");
        mostrar_trainer(&arr_trainers[i]);
        printf("\n");
    }




}


void buscar_entrenador_nombre(lista_trainers mis_train, char name[], int cant){        //Busca un entrenador con su nombre
    int i =0;
    while(i < cant){

        if(strcmp(mis_train[i].nombre,name) == 0){
            printf("Entrenador encontrado \n");
            mostrar_trainer(&mis_train[i]);
            break;
        }
        i++;
    }

    if(i>=cant){
        printf("Entrenador no encontrado\n");
    }



}

void buscar_entrenador_mostrar_team(lista_trainers mis_train, char name[], int cant){       //Busca un entrenador y muestra su equipo
     int i =0;
    while(i < cant){

        if(strcmp(mis_train[i].nombre,name) == 0){
            printf("Entrenador encontrado \n");
            printf("Equipo del entrenador %s: \n",mis_train[i].nombre);
            //printf("Cantidad de Pokemon: %i \n",mis_train[i].trainer_team->longitud);
            printf("========\n");
            mostrar_equipo_trainer(&mis_train[i]);
            break;
        }
        i++;
    }

    if(i>=cant){
        printf("Entrenador no encontrado\n");
    }


}


int hay_entrenadores_repetidos(lista_trainers trainers, int cantidad){     //verifica si hay entrenadores repetidos: cortesia de la IA
    int i, j;

    for (i = 0; i < cantidad - 1; i++) {
        for (j = i + 1; j < cantidad; j++) {
            // Comparar por ID
            if (trainers[i].id == trainers[j].id) {
                return 1;
            }
            // Comparar por nombre
            if (strcmp(trainers[i].nombre, trainers[j].nombre) == 0) {
                return 1;
            }
        }
    }

    return 0;
}






//Comentarios



/*

printf("Cantidad vic: %i, cant derr: %i, Score: %i ",grupos[f][0].cant_vic,grupos[f][0].cant_derr,grupos[f][0].score);
    printf("Pok eliminados: %i \n",grupos[f][0].cant_pok_derrot);
    printf("Cantidad vic: %i, cant derr: %i, Score: %i ",grupos[f][1].cant_vic,grupos[f][1].cant_derr,grupos[f][1].score);
    printf("Pok eliminados: %i \n",grupos[f][1].cant_pok_derrot);


*/





/*

printf("Turno: \n");
    printf("Pokemon atk y vida: %s : %i\n",atk->fijer->nombre_specie,atk->hp);
    printf("Pokemon def y vida: %s : %i\n",def->fijer->nombre_specie,def->hp);



printf("Dmg efectuado: %i\n",dmg);


 printf("Combate terminado: ===================\n ");
    printf("Pokemon op1 y vida: %s : %i\n",oponente1->fijer->nombre_specie,oponente1->hp);
    printf("Pokemon op2 y vida: %s : %i\n",oponente2->fijer->nombre_specie,oponente2->hp);

    if(oponente1->is_alive){
        printf("Pokemon ganador: %s\n",oponente1->fijer->nombre_specie);
    }else{
        printf("Pokemon ganador: %s\n",oponente2->fijer->nombre_specie);
    }


*/
/*

for(int i=0;i<4;i++){
        printf("Nombre: %s ",grupos[f][i].nombre);
        printf("Score: %i ",grupos[f][i].score);
        printf("Vic: %i Derr: %i\n",grupos[f][i].cant_vic,grupos[f][i].cant_derr);
    }



*/




    /*
    
    printf("Grupo: %c , Nombres: %s %s\n",
    arr_grupos[0].grupo,arr_grupos[0].trainer_first.nombre,arr_grupos[0].trainer_second.nombre);
    
    printf("Grupo: %c , Nombres: %s %s\n",arr_grupos[1].grupo,
    arr_grupos[1].trainer_first.nombre,arr_grupos[1].trainer_second.nombre);

    mostrar_equipo_trainer(&arr_grupos[0].trainer_first);
    printf("===================\n");
    mostrar_equipo_trainer(&arr_grupos[1].trainer_second);
    */ 

/*
for(int i=0;i<FIL_GROUP;i++){       //Imprime los resultados cargados
        printf("%i ",resultados[i].id_combat);
        printf("%i%c - %i%c\n",resultados[i].comb1.pos,resultados[i].comb1.grupo
            ,resultados[i].comb2.pos,resultados[i].comb2.grupo);

    }
    printf("\n\n");

    for(int i=0;i<FIL_GROUP;i++){       //Imprime el arreglo de grupos 
        printf("Grupo: %c. ",arr_grupos[i].grupo);
        printf("%i %s ||| ",arr_grupos[i].i_first,arr_grupos[i].trainer_first.nombre);
        printf("%i %s\n",arr_grupos[i].j_second,arr_grupos[i].trainer_second.nombre);

    }




printf("\n\n");
    
    for(int i=0;i<FIL_GROUP;i++){       //Imprime lo que ira a los cuartos de final
        printf("Tag: %c , Id: %i\n",arr_grupo_cuartos[i].tag_winner,arr_grupo_cuartos[i].id_combat);
        printf("Ganador de esa sala: %s\n",arr_grupo_cuartos[i].trainer_first.nombre);

    }






*/



/*


  for(int i=0;i<TRAINERS_TOURNAMENT;i++){
    
        fscanf(base_trainer,"%i",&id);
        fscanf(base_trainer,"%s",buffer);

    }

    for(int i=0;i<TRAINERS_TOURNAMENT;i++){
        iniciar_trainer(&arr_trainers[i]);      //inicializa los 32 trainres por fuera del flujo de archivo
    }


    rewind(base_trainer);   //Volvemos al inicio del archivo para dejar el apuntador alli



*/







