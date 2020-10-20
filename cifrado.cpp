/**
 * UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Programacion de microprocesadores
 * Seccion 21
 * Proyecto #2
 * 
 * Walter Saldaña #19897
 * Javier Cotto #19342
 * José Gutierrez #19111
 * 
 * Programa para cifrar y descifrar un mensaje
 * utilizando un algoritmo basado en el algoritmo
 * DES
 **/

//Importar librerias --------------------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <stdio.h>		//printf
#include <cstdlib>		//rand
#include <cmath>		//pow
#include <ctime>		//time_t, clock, difftime
#include <fstream>      //file processing
#include <pthread.h>	

using namespace std;

//Variables globales --------------------------------------------------------------------------------------------------------------------------------------------------
int* PLAIN;
char KEY[16];
int SIZE = 16;
const int ROUNDS = 4;
char ALLKEYS[16*ROUNDS];
int MODULE;



void rotateKeyCipher();

//Funcion para pthreads -----------------------------------------------------------------------------------------------------------------------------------------------
void *cifrar(void *thread_rank){
    //Obtener ID del hilo
    int tID;
	tID = (int)thread_rank;
    int cont;

    //Encriptar bloque asignado
    for(int i=0; i<ROUNDS; i++){
        cont = 0;
        for(int j=tID; j<=(tID+SIZE); j++){
            PLAIN[j] = char(int(PLAIN[j])+int(ALLKEYS[cont+(i*16)]));
            cont++;
        }
    }

    
    //Terminar el proceso
    pthread_exit(NULL);
}

void *descifrar(void *thread_rank){
    //Obtener ID del hilo
    int tID;
	tID = (int)thread_rank;
    int cont;

    //Encriptar bloque asignado
    for(int i=0; i<ROUNDS; i++){
        cont = 0;
        for(int j=tID; j<=(tID+SIZE); j++){
            PLAIN[j] = char(int(PLAIN[j])-int(ALLKEYS[cont+(i*16)]));
            cont++;
        }
    }

    //Terminar el proceso
    pthread_exit(NULL);
}

//Funciones generales -------------------------------------------------------------------------------------------------------------------------------------------------
void rotateKeyCipher(){
    int shift = (int)(SIZE/ROUNDS);
    char temp[SIZE];
    for(int i=0; i<SIZE; i++){
        temp[i]=KEY[i];
    }
    for(int i=0; i<SIZE-4; i++){
        KEY[i+4]=temp[i];
    }
    for(int i=0; i<4; i++){
        KEY[i]=temp[i+12];
    }
}

void escribir(char chars[]){
	ofstream archivo1;
	archivo1.open("llave.txt",ios_base::app);//Abriendo archivo en modo escritura
	if(archivo1.fail()){
		cout<<"No se pudo abrir el archivo";
		exit(1);
	}
	for(int i=0; i<SIZE; i++){
		archivo1<<chars[i];
	}
	archivo1.close();
}

void generateKey(){
    string my_key="";
    srand (time(NULL));
    for(int i=0; i<SIZE; i++){
        //Unicamente seleccionar asciis imprimibles
        KEY[i] = (char)((rand()%(127-33)) +33);
        //KEY[i] = (char)(rand()%128);
    }
    //escribir(KEY);
}

void encriptar(){
    //Generar llave aleatoria
    generateKey();
    cout<<"Llave: ";
    for(int i=0; i<SIZE; i++){
        cout<<KEY[i];
    }cout<<endl;

    for(int i=0; i<ROUNDS; i++){
        for(int j=0; j<SIZE; j++){
            ALLKEYS[j+(i*16)] = KEY[j];
        }
        rotateKeyCipher();
    }
    cout<<"\n"<<endl;
    for(int i=0; i<(SIZE*4); i++){
        cout<<ALLKEYS[i];
    }cout<<endl;

    //Crear objeto de lectura para el archivo
    ifstream lectura("plain.txt",ios::in);
    if(!lectura){
        cerr<<"Fail to read plain.txt"<<endl;
        exit(EXIT_FAILURE);
    }

    //Determinar numero de caracteres
    lectura.seekg(0, ios_base::end);
    int nChars = lectura.tellg();
    PLAIN = new int[nChars];
    lectura.seekg(0, ios_base::beg);

    //Almacenar mensaje del archivo en arreglo
    char letra;
    int i=0;
    while(lectura.get(letra)){
        PLAIN[i]<<letra;
        i++;
    }

    //Determinar cantidad de hilos a usar
    int n = nChars/16;
    MODULE = nChars%16;

    //Distribuir tareas en pthreads
    pthread_t threads;
    int ptEstatus;
    for(int i=0; i<=(n*SIZE); i+=SIZE){
        //Crear hilos
        ptEstatus = pthread_create(&threads, NULL, cifrar,(void *)i);
        if (ptEstatus){
            printf("ERROR; return code from pthread_create() is %d\n", ptEstatus);
            exit(-1);
        //Esperar a que finalicen
        }ptEstatus = pthread_join(threads, NULL);
        if (ptEstatus) {
            printf("ERROR; return code from pthread_join() is %d\n", ptEstatus);
            exit(-1);
        }
    }
}

void desencriptar(){}

// Main ---------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(){
    //Desplegar opciones
    int opc = 0;
    cout<<"Ingrese una opcion: \n\t(1) Encriptar plain.txt  \n\t(2) Desencriptar cipher.txt "<<endl;
    cin>>opc;

    //Verificar la opcion seleccionada
    if(opc == 1){
        encriptar();
    }else if(opc == 2){
        desencriptar();
    }else{
        cout<<"Ingrese una opcion valida"<<endl;
    }
    
    //Fin del programa
    return 0;
}
