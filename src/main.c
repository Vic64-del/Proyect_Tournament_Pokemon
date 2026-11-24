#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stdpoke/stdpoke.h"        //Incluimos stdpoke: pokedex y sus cosas
#include "stdtrainer/stdtrainer.h"      //Incluimos stdtrainer: para entrenadores, combates y torneo


void asignar_trainers_team_main(ptr_pok dex, lista_trainers arr_de_entrenadores, int nivel_poke, int cant_poke);
void mostrar_menu_main(Pokedex mi_pokedex, lista_trainers mis_trainers, FILE * base_results, FILE * base_trainers, combats_result arr_resultados, 
    tabla_grupos matriz_entrenador, Tabla_Tipos table, FILE * historial);

void consultar_dex_main(Pokedex dex);
void consultar_trainer_main(lista_trainers mis_trainers, int cant);
void agregar_trainer_stdin(int indic_arr, lista_trainers mis_trainers, Pokedex dex, int nivel_poke, int cant_poke);
void configu_menu_main(int * nivel_poke, int * cant_equipo);


int main(){
    FILE * base_pokemon;    //Archivo de datos sobre la pokedex
    FILE * base_tipos;        //Archivo de datos sobre los tipos pokemon y sus relaciones elementales
    FILE * base_trainers;   //Archivo con informacion de los 32 entrenadores para el torneo
    FILE * base_results;        //Archivo con resultados para los octavos de final
    FILE * hist_combats;        //Archivo no fuente, sino para escribir los combates realizados del torneo

    Pokedex dex;
    lista_trainers lista_de_entrenador;    //Lista de entrenadores
    tabla_grupos matriz_de_entrenador;      //Matriz para los grupos


    Lista_tipos colec_atk;          //Lista de tipos y la tabla
    Tabla_Tipos table;      
    combats_result arr_resultados;      //Arreglo de resultados que se cargaran de archivo o por teclado

    srand(time(NULL));     //Inicializacion de la semilla
    
    base_pokemon = fopen("data/pokedex.txt","r");
    if(base_pokemon == NULL){
        fprintf(stderr,"Error! Codigo de error: 1 (No se puede abrir el archivo o inexistente)\n");
        return -1;
    }

    base_tipos = fopen("data/tipos.txt","r");           //Comprobacion de la existencia de archivos
    if(base_tipos == NULL){
        fprintf(stderr,"Error! Codigo de error: 1 (No se puede abrir el archivo o inexistente)\n");
        return -1;
    }

    base_trainers = fopen("data/entrenadores.txt","r");
    if(base_trainers == NULL){
        fprintf(stderr,"Error! Codigo de error: 1 (No se puede abrir el archivo o inexistente)\n");
        return -1;
    }

    base_results = fopen("data/resultados.txt","r");
    if(base_results == NULL){
        fprintf(stderr,"Error! Codigo de error: 1 (No se puede abrir el archivo o inexistente)\n");
        return -1;
    }

    hist_combats = fopen("data/historial.txt","w+");
    if(hist_combats == NULL){
        fprintf(stderr,"Error! Codigo de error: 1 (No se puede abrir el archivo o inexistente)\n");
        return -1;   
    }



    iniciar_pokedex(dex);       //Operaciones para crear y cargar la Pokedex
    cargar_pokedex(base_pokemon,dex);       //Cargamos la pokedex desde su archivo fuente

    crear_tablatipos(table);                    //Operaciones para cargar y crear la tabla de tipos
    cargar_tipos_and_table(colec_atk,table,base_tipos);

    int val = hay__tipos_duplicados(colec_atk);

    if(val){            //Verificamos con val 

        fprintf(stderr,"Error! Codigo de error: 4 (Hay tipos duplicados, invalidos o erroneos)\n");
        return -1;
    }


    asociar_tipos_especie(dex,colec_atk);

    //cargar_trainers(base_trainers,lista_de_entrenador);     //Cargamos la informacion de los entrenadores y se asignan sus equipos
    //asignar_trainers_team_main(dex,lista_de_entrenador);    //Nota, se esta pasando el arreglo Pokedex a la funcion, aunque ella deba recibir un apuntador a *Especie

    //cargar_results_file(base_results,arr_resultados);       //Se carga el archivo de resultados

    //Desde aqui, todo esta listo para comenzar el torneo
    system("clear");    //LINEA IMPORTANTE
    mostrar_menu_main(dex,lista_de_entrenador,base_results,                             //Decir que es lo que estamos pasando por parametro
        base_trainers,arr_resultados,matriz_de_entrenador,table,hist_combats);


    //gran_torneo(matriz_de_entrenador,lista_de_entrenador,table,arr_resultados);  //Se realiza el torneo


    fclose(base_trainers);
    fclose(base_tipos);         //Se cierran los archivos
    fclose(base_pokemon);
    fclose(base_results);
    fclose(hist_combats);

    return 0;
}

void mostrar_menu_main(Pokedex mi_pokedex, lista_trainers mis_trainers, FILE * base_results, FILE * base_trainers, combats_result arr_resultados, tabla_grupos matriz_entrenador, Tabla_Tipos table, FILE * historial){           //Mostrara el menu de opciones del programa
    int flag_swicht=1;
    int read;
    int trigger_results = 0;       //Bandera para saber si ya se han cargado los resultados
    int trigger_trainer = 0;        //Badera para saber si los entrenadroes ya han sido cargados
    int trigger_tour = 0;       //Bandera para saber si ya se hizo el torneo

    int ind_arreglo_trainers=0;       //indice que corresponde a los entrenadores cargados manual
    int cant_participantes = 0;     //cantidad de participantes
    podio pod_torneo;
    Cola_torneo cola_registro;      //usaremos esta cola para organizar todos los participantes al finalizar el torneo
    lista_trainers arr_paralelo;
    iniciar_cola_tournam(&cola_registro);

    int nivel_poke = LEVEL_CAP/2;        //Regla de torneo, nivel 50 por defecto
    int cant_poke = LONG_TEAM;                  //6 pokemon por defecto
    int validar;        //para validar ciertas entradas
  
    //Solo podra haber un torneo por    ejecucion
    while(flag_swicht){
        printf("===GRAN TORNEO POKEMON 1.5v===\n");
        printf("1. Consultar Pokedex \n");   printf("============\n");
        printf("2. Registrar entrenador \n");        printf("============\n");
        printf("3. Crear equipo \n");                    printf("============\n");
        printf("4. Cargar entrenadores de archivo \n");      printf("============\n");
        printf("5. Consultar entrenadores \n");          printf("============\n");

        printf("6. Consultar podio \n");           printf("============\n");
        printf("7. Cargar resultados de archivo \n");            printf("============\n");
        printf("8. Realizar Gran Torneo Pokemon \n");                  printf("============\n");
        printf("9. Consultar clasificacion \n");           printf("============\n");
        printf("10. Consultar historial de combates \n");           printf("============\n");
        printf("11. Consultar enfrentamientos \n");              printf("============\n");
        printf("12. Configuracion \n");                 printf("============\n");
        printf("13. Salir\n");                       printf("============\n");
        //printf("14. Sugerencia para git \n");                printf("============\n");    
        //consultar historial y enfrentamientos en opciones


        printf("/-Digite una opcion: ");
        scanf("%i",&read);
        limpiar_stdin();

        switch(read){
            case 1:     //Mostrara toda la pokedex, por ahora
            consultar_dex_main(mi_pokedex);

            break;

            case 2:     //Se registrara un entrenador por pantalla, se restara en 1 la cantidad de leidos del archivo
            
            if(cant_participantes<TRAINERS_TOURNAMENT){
                cant_participantes++;
                agregar_trainer_stdin(ind_arreglo_trainers,mis_trainers,mi_pokedex,nivel_poke,cant_poke);     
                printf("Entrenador cargado \n");
                ind_arreglo_trainers++;  //Aumenta en 1 para la proxima casilla. Enviaremos esto para restar al resto de entrenadores que puedan ser cargados por archivo      
                
                validar = hay_entrenadores_repetidos(mis_trainers,cant_participantes);

                if(validar){    //Si validar recibe 1, hay repetidos, error
                    fprintf(stderr,"Error! Codigo de error: 3 \n");
                    exit(EXIT_FAILURE);
                }    


                trigger_trainer =  1;
            }else{
                printf("Cantidad maxima de entrenadores registrados \n");

            }
            if(cant_participantes == TRAINERS_TOURNAMENT) trigger_trainer = 1;  //Si manualmente se suben 32 entrenadores

            break;

            
            case 4:

            cargar_trainers(base_trainers,mis_trainers,ind_arreglo_trainers,cant_poke);   //pasaremos el indice de los entrnadores cargados por input, para que se carge el resto de archivo    

            validar = hay_entrenadores_repetidos(mis_trainers,TRAINERS_TOURNAMENT);

            if(validar){    //Si validar recibe 1, hay repetidos, error
                fprintf(stderr,"Error! Codigo de error: 3 \n");
                exit(EXIT_FAILURE);
            }   


            asignar_trainers_team_main(mi_pokedex,mis_trainers,nivel_poke,cant_poke);
            trigger_trainer = 1;
            cant_participantes = TRAINERS_TOURNAMENT;   //Ahora si hay 32;
            printf("Entrenadores han sido cargados de archivo, tambien sus equipos han sido asignados \n");               
            break;


            case 5:     //Consultaremos los entrenadores almacenados

            if(cant_participantes <= 0){     //Si no hay participantes
                printf("No se pueden mostrar los entrenadores, aun no han sido cargados\n");
            }else{  
                printf("Lista de entrenadores: \n");   


                if(trigger_tour != 1){      //Si no se ha realizado torneo, el arreglo estara inicialmente vacio
                    consultar_trainer_main(mis_trainers,cant_participantes); 
                }else{
                    //Usaremos el arreglo con datos reales
                    consultar_trainer_main(arr_paralelo,cant_participantes);//cant part debe ser 32

                }

                //imprimir_lista_entrenadores(mis_trainers,cant_participantes);
            }
           
            break;


            case 6:             //Mostraremos el podio, los 4 primeros lugares del torneo
            if(trigger_tour!=1){
                printf("No es posible mostrar el podio: el Torneo Pokemon no ha sido realizado aun\n");

            }else{
                printf("===Podio del Torneo Pokemon===\n");
                printf("|- Campeon: %s \n",pod_torneo.champ.nombre);
                
                printf("|- Subcampeon: %s \n",pod_torneo.subchamp.nombre);
                printf("|- Tercer lugar: %s \n",pod_torneo.third.nombre);
                printf("|- Cuarto lugar: %s \n",pod_torneo.four.nombre);
            }
            break;





            case 7:         //Se cargan los resultados altos del torneo, necesarios para que se empiece
            cargar_results_file(base_results,arr_resultados);

            validar = validar_results(arr_resultados,8,4,2);        //para validar

            if(validar!=0){
                fprintf(stderr,"Error! Codigo de error: 2\n");
            }

            printf("Se han cargado los resultados\n");
            trigger_results=1;
            break;

            case 8:             //Para el torneo 
            if(trigger_results!=1  || trigger_trainer != 1){
                printf("No se puede iniciar el torneo: resultados no han sido cargados o entrenadores sin cargar \n");
            }else{

                if(cant_participantes != TRAINERS_TOURNAMENT){
                    printf("No se puede iniciar el torneo: participantes insuficientes \n");
                }else{

                    if(trigger_tour == 1){
                        printf("Ya se ha realizado el Torneo Pokemon!\n");
                    }else{

                        gran_torneo(matriz_entrenador,mis_trainers,table,arr_resultados,&pod_torneo,historial,&cola_registro);
                        trigger_tour = 1;    //Ya se realizo el torneo

                        int i =0;
                        while(!es_cola_empty(&cola_registro)){  //Vaciaremos la cola en un arreglo para ordenarlo y tener la clasificacion completa al alcance
                        arr_paralelo[i] = desencolar_tournam(&cola_registro);
                        
                        i++;
                        if(i>=TRAINERS_TOURNAMENT) break;   
                        }


                    }

                   
                }
                


            }
            break;

            case 9:                 //Mostraremos la clasificacion ordenada 

            if(trigger_tour != 1){      //No se ha realizado el torneo, no podemos mostrar nada
                printf("No es posible mostrar la clasificacion: Torneo no realizado\n");

            }else{
                printf("Clasificacion general del torneo: \n");

                quick_sort_trainer_list(arr_paralelo,TRAINERS_TOURNAMENT,TRAINERS_TOURNAMENT-1,0);  //Ordenamos
                imprimir_lista_entrenadores(arr_paralelo,TRAINERS_TOURNAMENT);
                

            }
            break;


            case 10:            //Mostraremos el historial de combates escrito en el archivo
            
            if(trigger_tour != 1){
                printf("No se puede mostrar historial: Torneo no realizado\n");
            }else{
                mostrar_contenido(historial);
            }
            break;
            

            case 11:            //Mostraremos los enfrentamientos cargados

            if(trigger_results != 1){
                printf("No se puede mostrar los resultados, no han sido cargados\n");
            }else{
                imprimir_enfrentamientos(arr_resultados);
            }

           
            break;

            case 12:            //Opciones de configuracion para el programa
            configu_menu_main(&nivel_poke,&cant_poke);
            break;


            case 13:        //Comando para salir y romper el while
            flag_swicht=0;
            break;


            default:
            printf("Opcion invalida\n");
        }



        if(!flag_swicht){
            break;
        }
        printf("\nPresione para continuar: ");
        while(getchar() != '\n');         //para limpiar el buffer, cortesia de la IA
        getchar();
        system("clear");    //Limpia la pantalla, en linux sera el comando clear



    }


    printf("Gracias por usar este programa\n");
}


void consultar_trainer_main(lista_trainers mis_trainers, int cant){
    system("clear");
    char buffer[20];
    int flag=1;
    int read;

    while(flag){
        printf("Opciones de entrenador:\n");        printf("===============\n");  
        printf("1. Mostrar todos los entrenadores \n");         printf("===============\n");  
        printf("2. Mostrar informacion de un entrenador \n");       printf("===============\n");  
        printf("3. Mostrar equipo de un entrenador\n");     printf("===============\n");  

        printf("4. Regresar al Menu principal \n");     printf("===============\n");  
        
        printf("/-Digite una opcion: ");
        scanf("%i",&read);
        limpiar_stdin();        //Limpiar entrada despues de cada scan

        
        switch(read){
            case 1:
            printf("Lista completa de entrenadores: \n");             //Mostramos la lista completa 
            imprimir_lista_entrenadores(mis_trainers,cant);
            break;

            case 2:     //Buscaremos un entrenador por nombre y mostraremos su info
            printf("Digite nombre de entrenador a buscar: ");
            scanf("%s",buffer);
            limpiar_stdin();

            corregir_formato(buffer);
            

            printf("===========\n");
            buscar_entrenador_nombre(mis_trainers,buffer,cant);
            limpiar_buffer(buffer,sizeof(buffer));      //Limpiamos para posteriores usadas
            break;


            case 3:             //Buscaremos el entrenador y mostraremos su equiipo
            printf("Digite nombre de entrenador para mostrar su equipo: ");
            scanf("%s",buffer);
            limpiar_stdin();
            corregir_formato(buffer);

            buscar_entrenador_mostrar_team(mis_trainers,buffer,cant);
            limpiar_buffer(buffer,sizeof(buffer));  

            break;  


            case 4:
            flag = 0;
            break;

            default:
            printf("Entrada invalida\n");



        }


        printf("\nPresione para continuar: ");
        while(getchar() != '\n');         //para limpiar el buffer, cortesia de la IA
        getchar();

        system("clear");



    }







}


void configu_menu_main(int * nivel_poke, int * cant_equipo){            //Menu de configuracion con opciones para variar reglas de torneo
    system("clear");
    
    int mod;
    int flag =1;
    int read;

    while(flag){
        printf("==Opciones de Configuracion==\n");        printf("============\n");
        printf("1. Modificar nivel de Pokemon \n");       printf("============\n");
        printf("2. Modificar cantidad de Pokemon de equipos \n");          printf("============\n");       
        printf("3. Opciones para el generador de equipos\n");       printf("============\n");
        printf("4. Mostrar codigos de error \n");             printf("===============\n"); 
        printf("5. Regresar a Menu principal \n");      printf("============\n");

        printf("/-Digite una opcion: ");
        scanf("%i",&read);

        switch(read){
            case 1:     //Mostraremos la totalidad de la pokedex
            printf("Nivel actual de regla del Torneo: %i \n",*nivel_poke);
            printf("Desea modificar el nivel? Digite 1 para si, 2 para no: ");

            scanf("%i",&mod);
            if(mod == 1){
                printf("Digite el nuevo nivel: ");
                scanf("%i",&mod);

                if(mod > LEVEL_CAP || mod <= 0){
                    printf("Nivel invalido\n");
                }else{
                    *nivel_poke = mod;      //Se cambia el nivel    
                    printf("\nNivel actualizado exitosamente\n");
                }


            }

            limpiar_stdin();
            break;

            case 2:
            printf("Cantidad de ejemplares para un equipo de regla del Torneo: %i \n",*cant_equipo);
            printf("Desea modificar la cantidad de ejemplares por equipo? Digite 1 para si, 2 para no: ");

            scanf("%i",&mod);
            if(mod == 1){
                printf("Digite la nueva cantidad: ");
                scanf("%i",&mod);

                if(mod > LONG_TEAM || mod <= 0){
                    printf("Cantidad invalida\n ");
                }else{
                    *cant_equipo = mod;        
                    printf("\nCantidad actualizada exitosamente\n");
                }


            }

            break;

            case 3:
            printf("\nConfiguracion actual del generador de equipos: ");
          
            case 4:             //Codigos basicos de error para identificar
            printf("Listado de codigos de error:\n");
            printf("Codigo: 1 | Error: Algun archivo inexistente o incapaz de abrir\n");
            printf("Codigo: 2 | Error: Algun resultado de combate invalido \n");
            printf("Codigo: 3 | Error: Entrenadores duplicados o invalidos \n");
            printf("Codigo: 4 | Error: Algun tipo invalido o inexistente \n");
            printf("Codigo: 50 | Error: Conflicto al asignar memoria con malloc \n");
        
            break;

            case 5:         //salir del submenu
            flag=0;
            break;

            default:
            printf("Entrada invalida\n");


        }


        printf("\nPresione para continuar: ");
        while(getchar() != '\n');         //para limpiar el buffer, cortesia de la IA
        getchar();

        system("clear");
    }





}






void consultar_dex_main(Pokedex dex){      //Ofrecera funciones para el submenu dedicado a la pokedex
    system("clear");
    char buffer[20];
  
    int flag =1;
    int read;

    while(flag){
        printf("==Opciones de Pokedex==\n");        printf("============\n");
        printf("1. Mostrar Pokedex completa \n");       printf("============\n");
        printf("2. Buscar Pokemon por nombre \n");          printf("============\n");       
        printf("3. Buscar Pokemon por numero\n");       printf("============\n");
        printf("4. Regresar a Menu principal \n");      printf("============\n");

        printf("/-Digite una opcion: ");
        scanf("%i",&read);

        switch(read){
            case 1:     //Mostraremos la totalidad de la pokedex
            mostrar_dex_completa(dex);
            break;

            case 2:
            printf("\nDigite el nombre del Pokemon a buscar: ");
            scanf("%s",buffer);
            limpiar_stdin();            //limpia lo restante en stdin
            corregir_formato(buffer);

            buscar_pk_nombre(dex,buffer);
            limpiar_buffer(buffer,sizeof(buffer));      //Limpiamos lo leido para proximas lecs

            break;

            case 3:
            printf("\nDigite el numero del Pokemon a buscar: ");
            scanf("%s",buffer);
            limpiar_stdin();
            
            corregir_id(buffer);
            buscar_pk_numero(dex,buffer);

            limpiar_buffer(buffer,sizeof(buffer));
            break;

            case 4:         //salir del submenu
            flag=0;
            break;

            default:
            printf("Entrada invalida\n");


        }


        printf("\nPresione para continuar: ");
        while(getchar() != '\n');         //para limpiar el buffer, cortesia de la IA
        getchar();

        system("clear");
    }


}



void agregar_trainer_stdin(int indic_arr, lista_trainers mis_trainers, Pokedex dex, int nivel_poke, int cant_poke){       //Crearemos un nuevo trainer introducido por pantalla
    Trainer new;
    char buffer[20];
    int id_read;
    int min=0;
    int n;
    int max = POKES_CANT;

    printf("Digite el nombre del entrenador: ");
    scanf("%s",buffer);
    printf("Digite id del entrenador: ");
    scanf("%i",&id_read);

    iniciar_trainer(&new,buffer,id_read,cant_poke);

   

    for(int j=0;j<cant_poke;j++){       //A cada entrnador individual se le asigna un equipo de 6
        n = min + rand() % (max-min + 1);
        construir_equipo(dex,&new,n,nivel_poke);
    
    }



    mis_trainers[indic_arr] = new;      //Pasamos el nuevo trainer creado a la posicion correspondiente en su arreglo

    limpiar_stdin();
}



void asignar_trainers_team_main(ptr_pok dex, lista_trainers arr_de_entrenadores, int nivel_poke, int cant_poke){     //A cada entrenador ya leido en el arreglo, le creamos un equipo de 6
    int n;
    int min = 1;
    int max = POKES_CANT;

    
    for(int i=0;i<TRAINERS_TOURNAMENT;i++){ //Ciclo para recorrer todos los 32 entrenadores
        
        //Colocar condicional  
        if(arr_de_entrenadores[i].trainer_team->longitud >= cant_poke){
            
            continue;       //El entrenador, por lo que sea, ya tiene 6 en su equipo, saltamos la iter
            
        }

        //arr_de_entrenadores[i].trainer_team->base_cantidad = cant_poke;         //Cantidad base de pokemon por equipo. Invariable en torneo
       
        for(int j=0;j<cant_poke;j++){       //A cada entrnador individual se le asigna un equipo de 6
            n = min + rand() % (max-min + 1);           //Un id random 
         
            construir_equipo(dex,&arr_de_entrenadores[i],n,nivel_poke);
        }

        
    }

}



/*

  for(int i=0;i<TRAINERS_TOURNAMENT;i++){
        printf("%i %s \n",lista_de_entrenador[i].id,lista_de_entrenador[i].nombre);
        mostrar_equipo(&lista_de_entrenador[i]);
    }

*/
/*
    Trainer paco;
    iniciar_trainer(&paco);
    int n;
    int min=1;
    int max = POKES_CANT;
    for(int i=0;i<LONG_TEAM;i++){
        n = min + rand() % (max-min + 1);
        construir_equipo(pointer,&paco,n);      

    }

    mostrar_equipo(&paco);
    */



/*


 printf("Equipo de %s :[================]\n",juan.nombre);
    mostrar_equipo(&juan);

    printf("Equipo de %s :[================]\n",maria.nombre);
    mostrar_equipo(&maria);

    printf("\n");
    printf("Desea mostrar el combate en tiempo real? ");
    int c = getchar();

    printf("HISTORIAL COMBATE: =======================\n");
    sala_trainers(&juan,&maria,table);











*/

 /*
    Trainer paco;
    Trainer sasha;
    iniciar_trainer(&paco);
    strcpy(paco.nombre,"Paco");

    iniciar_trainer(&sasha);
    strcpy(sasha.nombre,"Sasha");
    int n;
    int min=1;
    int max = POKES_CANT;
    for(int i=0;i<LONG_TEAM;i++){
        n = min + rand() % (max-min + 1);
        construir_equipo(dex,&paco,n);      

    }

    for(int i=0;i<LONG_TEAM;i++){
        n = min + rand() % (max-min + 1);
        construir_equipo(dex,&sasha,n);      

    }

    printf("EQUIPO DE PACO ================\n");
    mostrar_equipo(&paco);
    printf("EQUIPO DE SASHA ================\n");
    mostrar_equipo(&sasha);

    printf("\n");

    sala_trainers(&paco,&sasha,table);
    */

    //Generamos un magnemite y un nidoqueen