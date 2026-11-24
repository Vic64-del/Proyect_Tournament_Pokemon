#include "stdtypes.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void corregir_formato(char input[]){    //Corregira un formato de entrada con mayusculas y minusculas variable
    size_t len = strlen(input);
    int flag = 0;
 
    if(!(isupper(input[0]))){   //Si no es mayuscula la primera pos, hacerla 
        input[0] =toupper(input[0]);
    }
    int i=1;
    while(i<len){
        if(!(islower(input[i]))){   //Si no es minuscula, la pasa a a minuscula
            input[i] = tolower(input[i]);
        }

        if(isdigit(input[i])){
            flag=1;
            break;
        }

        i++;
    }

    if(flag){
        printf("Entrada invalida\n");
    }

    
}

void corregir_id(char input[]) {           //CORTESIA DE LA IA  
    size_t len = strlen(input);         

    // Solo dígitos, no vacío
    if (len == 0) return;
    for (size_t i = 0; i < len; i++)
        if (!isdigit((unsigned char)input[i])) return;

    // Si ya tiene 3+ dígitos, no tocar
    if (len >= 3) return;

    char buffer[4];
    buffer[0] = '0';
    buffer[1] = '0';
    buffer[2] = '0';
    buffer[3] = '\0';

    // Copiar los dígitos al final
    if (len == 1) {
        buffer[2] = input[0];
    } else { // len == 2
        buffer[1] = input[0];
        buffer[2] = input[1];
    }

    strcpy(input, buffer);
}



void crear_tablatipos(Tabla_Tipos table){   //Inicializa todas las casillas de la tabla en neutro
    for(int i=0;i<TYPESPOK_CANT;i++){
        for(int j=0;j<TYPESPOK_CANT;j++){
            table[i][j] = NEUTRO;
        }

    }

}

void cargar_tipos_and_table(Lista_tipos coleccion, Tabla_Tipos table, FILE * base_tipos){       //Lee los tipos de un archivo e inmediatamente se cargan en la matriz de tabla de tipos
    int i=0;

    while(i<TYPESPOK_CANT){
        fscanf(base_tipos,"%i",&coleccion[i].id);                                   //Se lee id y nombre
        coleccion[i].id-= 1;          //Resta en 1 para coincidir con la casilla de la matriz!
        fscanf(base_tipos,"%s",coleccion[i].name);

        procesar_linea(table,base_tipos,i);     //Procesa una linea entera

        i++;
    }


}

void procesar_linea(Tabla_Tipos table, FILE * base_tipos, int fil){      //Procesa una linea, mandando a la tabla las interacciones
    char buffer[4];         //El caracter 'Z' sera mi centinela para indiciar el fin de una linea
    int conv_id;
    char interaccion = 'A';
    int cont =0;
    while(interaccion != 'Z'){

        fscanf(base_tipos,"%s",buffer);
        conv_id = buffer[0] - '0';      //Convierte el caracter numerico, cual representa una id, a valor entero
       
        if(isdigit(buffer[1])){                         //Si hay dos digitos
            conv_id = (conv_id * 10) + buffer[1] - '0';
            interaccion = buffer[2];
        }else{
            interaccion = buffer[1];
        }
        
        if(interaccion == 'Z'){
            break;
        }

        switch(interaccion){
            case 'F':
            table[fil][conv_id-1] = SUPER_EFFECT;       //Muy efecvivo
            break;

            case 'L':
            table[fil][conv_id-1] = LESS_EFFECT;        //Poco efectivo
            break;

            case 'N':
            table[fil][conv_id-1] = NONE_EFFECT;        //sin efecto
            break;

            default:
            fprintf(stderr,"Error! Codigo de error: 3\n");  //Si llegamos aca, la interaccion no es natural, hay error
            exit(EXIT_FAILURE); //salimos

        }

        cont++;

        if(cont >= 100){      //Si se han realizado muchas iteraciones y no se ha salido, la bandera no se encuentra. Pasa algo y mejor cerramos
            fprintf(stderr,"Error! Codigo de error: 3\n");
            exit(EXIT_FAILURE);
        }

    }



}


void imprimir_tabla(Tabla_Tipos table){
    for(int i=0;i<TYPESPOK_CANT;i++){
        for(int j=0;j<TYPESPOK_CANT;j++){
            printf("%i ",table[i][j]);

        }
        printf("\n");
    }



}

void mostrar_tipos(Lista_tipos coleccion){      //Imprime la los tipos y sus Ids respectivos
    for(int i=0;i<TYPESPOK_CANT;i++){
        printf("Nombre: %s, Id: %i\n",coleccion[i].name,coleccion[i].id);
    }


}

void mostrar_interacciones(int atk, int def, Tabla_Tipos table){    //Recibe el id del tipo atacante y del tipo defensor, y muestra la relacion entre ambos
    switch(table[atk][def]){
        case SUPER_EFFECT:
        printf("Super efectivo\n");
        break;

        case LESS_EFFECT:
        printf("Poco efectivo\n");
        break;

        case NONE_EFFECT:
        printf("Sin efecto\n");
        break;

        case NEUTRO:
        printf("Efecto neutro\n");
        break;

        default:
        printf("no se");


    }



}

void mostrar_todo_interc(Tabla_Tipos table, int id, Lista_tipos coleccion){ 
    for(int j=0;j<TYPESPOK_CANT;j++){
        printf("%s > %s :: ",coleccion[id].name,coleccion[j].name);
        mostrar_interacciones(id,j,table);

    }


}

int comparar_cadena(char s1[], char s2[]){  //Esta funcion usa strcmp para comparar dos cadenas, usada para legibilidad. Devuelve 1 si son iguales
    if(strcmp(s1,s2)==0){
        return 1;

    }

    return 0;

}

float devolver_multiplicar_tipo(int atk, int def, Tabla_Tipos table){   //Con el indice de la matriz, compara cual sera el caso y lo retorna. Acceso inmediato
                   
    if(atk == -1 || def == -1){ //En caso de id invalido, o de pokemon que tenga segundo tipo
        return 0;
    }
    
    switch(table[atk][def]){
        case SUPER_EFFECT:

        return SUPER_EFFECT / FACTOR_CONV;      
    

        case LESS_EFFECT:
        
        return (float)LESS_EFFECT / FACTOR_CONV;        //Se castea a float para que retorne 0.5 o 1/2
   

        case NONE_EFFECT:
        return NONE_EFFECT;

    
        case NEUTRO:
        return NEUTRO / FACTOR_CONV;
    }

    return 0;       
}

//Las siguientes funciones seran para procesar entradas de resultados


void cargar_results_file(FILE * base_results, combats_result arr){  //Esta funcion lee los resultados de un archivo de resultados, con id de combate y sus participantes
    int n;
    char buffer[4];
    int temp;  

    fscanf(base_results,"%i",&n);       //Leemos el numero correspondiente a octavos de final
    int i;

    for(i=0;i<n;i++){
        fscanf(base_results,"%i",&arr[i].id_combat);    //Numero del combate

        fscanf(base_results,"%s",buffer);
        arr[i].comb1.pos = buffer[0] - '0';
        arr[i].comb1.grupo = buffer[1];             //Lectura del buffer a un combatiente

        fseek(base_results,3,SEEK_CUR);     //Nos movemos unas posiciones

        fscanf(base_results,"%s",buffer);
        arr[i].comb2.pos = buffer[0] - '0';
        arr[i].comb2.grupo = buffer[1]; 

   
    }

    fscanf(base_results,"%i",&n);       //Leemos el numero de combates que corresponden a cuartos de final
    int j= 0;

    for(j=0;j<n;j++){
        fscanf(base_results,"%i",&arr[i].id_combat);  
        fscanf(base_results,"%s",buffer);

        arr[i].comb1.grupo = buffer[0];

        temp = ((buffer[1] - '0') * 10) + (buffer[2] - '0');    
        arr[i].comb1.pos = temp;

        fseek(base_results,3,SEEK_CUR);

        fscanf(base_results,"%s",buffer);
        arr[i].comb2.grupo = buffer[0];

        temp = ((buffer[1] - '0') * 10) + (buffer[2] - '0');    //para conocer el numero de cada trainer
        arr[i].comb2.pos = temp;




        i++;    //Aumentamos 


    }


    //Leeremos los resultados de las semifinales
    fscanf(base_results,"%i",&n);       //Leemos el numero de combates que corresponden a cuartos de final
    //printf("%i \n",i);

    for(j=0;j<n;j++){
        fscanf(base_results,"%i",&arr[i].id_combat);  
        fscanf(base_results,"%s",buffer);

        arr[i].comb1.grupo = buffer[0];

        temp = ((buffer[1] - '0') * 10) + (buffer[2] - '0');    
        arr[i].comb1.pos = temp;

        fseek(base_results,3,SEEK_CUR);

        fscanf(base_results,"%s",buffer);
        arr[i].comb2.grupo = buffer[0];

        temp = ((buffer[1] - '0') * 10) + (buffer[2] - '0');    //para conocer el numero de cada trainer
        arr[i].comb2.pos = temp;




        i++;    //Aumentamos, el iterador i no se reinicia, ira aumentando a medida que se leen los resultados
    }


    rewind(base_results);       //Regresamos al inicio por si esta funcion es llamada nuevamente
}


//Funciones para procesamiento de cadena y buffer

void limpiar_stdin(){          //CORTESIA DE LA IA
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void limpiar_buffer(char *buf, size_t tam){            //LIMPIA EL BUFFER, CORTESIA DE LA IA
    memset(buf, 0, tam);
    // o alternativamente: buf[0] = '\0';   
}


void mostrar_contenido(FILE *f)                 //MOSTRAR ARCHIVO, CORTESIA DE LA IA 
{
    if (!f) {
        fprintf(stderr, "mostrar_contenido: puntero FILE nulo\n");
        return;
    }

    /* 1) Vaciar el buffer de escritura al archivo */
    if (fflush(f) != 0) {           //Manda todo lo de buffer, vaciandolo, a su debido destino durante la ejecucion
        perror("fflush");
        return;
    }

    /* 2) Ir al principio del archivo */
    if (fseek(f, 0, SEEK_SET) != 0) {
        perror("fseek");
        return;
    }

    /* 3) Leer y mostrar hasta EOF */
    int c;
    while ((c = fgetc(f)) != EOF) {
        putchar(c);
    }
    /* Si el archivo no terminaba en '\n', añadimos uno para que la consola quede limpia */
    putchar('\n');

    if (ferror(f)) {
        perror("fgetc");
        clearerr(f);
        return;
    }

    /* 4) Dejar el cursor al final para poder seguir escribiendo */
    fseek(f, 0, SEEK_END);
}


void imprimir_enfrentamientos(combats_result arr_results){      //Imprime los resultados cargados
    //i==14
    printf("Enfrentamientos de octavos: \n");
    
    for(int i=0;i<8;i++){
        printf("%i %i%c  -- %i%c\n",arr_results[i].id_combat,arr_results[i].comb1.pos,arr_results[i].comb1.grupo,
        arr_results[i].comb2.pos,arr_results[i].comb2.grupo);

    }
    printf("\n");
    printf("Enfrentamientos de cuartos: \n");
    for(int i=8;i<12;i++){

        printf("%i %c%i -- %c%i \n",arr_results[i].id_combat,arr_results[i].comb1.grupo,arr_results[i].comb1.pos,
        arr_results[i].comb2.grupo,arr_results[i].comb2.pos);


    }

    printf("\n");
    printf("Enfrentamientos de semifinales:  \n");
    for(int i=12;i<14;i++){

        printf("%i %c%i -- %c%i \n",arr_results[i].id_combat,arr_results[i].comb1.grupo,arr_results[i].comb1.pos,
        arr_results[i].comb2.grupo,arr_results[i].comb2.pos);


    }

}

int hay__tipos_duplicados(const Lista_tipos lista){        //Verifica que no hayan tipos duplicados. CORTESIA DE LA IA
    for (int i = 0; i < TYPESPOK_CANT; i++) {
        for (int j = i + 1; j < TYPESPOK_CANT; j++) {
            if (lista[i].id == lista[j].id) {
                return 1;   // id repetido
            }
            if (strcmp(lista[i].name, lista[j].name) == 0) {
                return 1;   // nombre repetido
            }
        }
    }
    return 0;
}


int validar_results(const combats_result arr,           //Valida los results cargados, cortesia de la IA
                    int n_octavos, int n_cuartos, int n_semis)          
{
    int total = n_octavos + n_cuartos + n_semis;

    /* ---- 1) IDs únicos y en orden creciente ---- */
    for (int i = 0; i < total; i++) {
        if (arr[i].id_combat < 49 || arr[i].id_combat >= 49 + total)
            return ERR_ID_RANGO;
        for (int j = i + 1; j < total; j++)
            if (arr[i].id_combat == arr[j].id_combat)
                return ERR_ID_DUP;
        if (i > 0 && arr[i].id_combat <= arr[i-1].id_combat)
            return ERR_ID_FUERA_ORDEN;
    }

    /* ---- 2) Validar cada combate según su ronda ---- */
    for (int i = 0; i < total; i++) {
        combatiente a = arr[i].comb1;
        combatiente b = arr[i].comb2;

        if (a.grupo == b.grupo && a.pos == b.pos)
            return ERR_COMB_IGUALES;

        if (i < n_octavos) {
            /* Octavos: token "1A".."2H"
               grupo: 'A'..'H'   pos: 1..2 */
            if (a.grupo < 'A' || a.grupo > 'H') return ERR_GRUPO_INVALIDO;
            if (b.grupo < 'A' || b.grupo > 'H') return ERR_GRUPO_INVALIDO;
            if (a.pos < 1 || a.pos > 2)         return ERR_POS_RANGO;
            if (b.pos < 1 || b.pos > 2)         return ERR_POS_RANGO;
            /* en octavos los grupos deben ser distintos */
            if (a.grupo == b.grupo)             return ERR_RONDA;
        }
        else {
            /* Cuartos y semis: token "G49".."G62"
               grupo literal 'G'   pos: 49..62 (rango global de ids de octavos) */
            if (a.grupo != 'G')                 return ERR_GRUPO_INVALIDO;
            if (b.grupo != 'G')                 return ERR_GRUPO_INVALIDO;
            if (a.pos < 49 || a.pos > 62)       return ERR_POS_RANGO;
            if (b.pos < 49 || b.pos > 62)       return ERR_POS_RANGO;
        }
    }

    return OK;
}










