/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (C) Marcial Hernandez Sanchez, 2015
 * University of Santiago, Chile (Usach) 
 */

//MPI
#include <mpi.h>
//C
#include <stdio.h>
#include <unistd.h>
//File descriptors from sys - C
#include <sys/types.h>
#include <sys/stat.h>
//System tools to open and write - C
#include <fcntl.h>
//C++
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <vector>

using namespace std;

void inicializaListaPos(int *lista, int tam){
	for (int i=0;i<tam;i++){
		lista[i]=1;
	}
}


int numeroAleatorio(int tope){
	return rand()%tope+1;
}

//Valida que la entrada sea numerica
//retorna 0 y guarda la entrada en entradaPrograma si la entrada es numero
//retorna 1 si hay falla
int isNumber(const string entradaConsola, int * entradaPrograma ){
	int largoEntrada=entradaConsola.size();
	for (int contador=0;contador<largoEntrada;contador++){
		if (!isdigit(entradaConsola.at(contador))){
			return 1;
		}
	}

	*entradaPrograma=stoi(entradaConsola);
	return 0;
}

bool recibeArgumentosConsola(const char * opciones,int argc, char **argv, int *token){

	/* Declaracion de las banderas */

	//La bandera _t: para asegurar que solo haya un argumento
	//Por ejemplo; podria escribir por consola -i entrada1 -i entrada2
	//si pasa esto, se retornara un mensaje de error y se terminara la ejecucion

	int banderaErrorParametros=0, banderaErrorBanderas=0, bandera_t=0, argumentoConsola;

	while (((argumentoConsola = getopt (argc, argv, opciones)) != -1) &&  banderaErrorParametros==0 && banderaErrorBanderas==0){
		//No tiene caso seguir con el while, si se ha detectado una falla en el camino
		//Los casos estan declarados explicitamente, y no analizando debidamente tal la entrada * opciones
		switch (argumentoConsola){  

			case 't': if (bandera_t==0) { //archivo

					  bandera_t++; 

					  banderaErrorParametros = banderaErrorParametros + isNumber(optarg, token);

					  if (*token==0){ //Necesariamente el token tiene que ser mayor que 0
					  	banderaErrorParametros++;
					  }
					  
				  }
				  else{
					  banderaErrorBanderas++;						
				  }
				  break;


			case ':': banderaErrorParametros++; break;
			case '?': 
				  if ((optopt=='t')){
					  banderaErrorParametros++;
				  }
				  else{
					  banderaErrorBanderas++;
				  }
				  break;
			default: banderaErrorBanderas++; break;
		}
	}

	/////////////////////////////////////////////////////////////////

	//////////// Analisis de los argumentos obtenidos de getOpt ///////////

	if (banderaErrorBanderas>0){
		cout << "Error: una o más opciones estan duplicadas o no estan disponibles" << endl;
		return false;
	}

	if (bandera_t==0){
		cout << "Error: no se ha especificado el token de entrada" << endl;
		return false;
	}

	if (banderaErrorParametros>0){

		cout << "Error: uno o más argumentos de alguna de las opciones no son validos" << endl;
		return false;
	}

	return true;
}// Fin funcion recibeArgumentosConsola()

int main(int argc, char **argv) {

	int token=0,cantidadDeProcesos,idProceso,vecino[2],mensajeTag=0;

	/* Una cadena que lista las opciones cortas válidas para getOpt
	  Se inicia con : pues si falta algun argumento, enviara un caso tipo ":"" */

	const char* const opciones = "t:";

	if (recibeArgumentosConsola(opciones,argc, argv,&token) ==false){
		return 0;
	}

  	MPI_Status recv_status, send_status;
  	MPI_Request send_request;

  //Inicializando MPI

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &idProceso);
  MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);

  //////////////////////////////////

  //Declaracion de variables para iniciar el juego

  const int izquierdo=0;
  const int derecho=1;
  //Asignado vecinos
  if ((vecino[izquierdo]=(idProceso-1)) < 0) vecino[izquierdo] = cantidadDeProcesos-1;
  if ((vecino[derecho]=(idProceso+1)) == cantidadDeProcesos) vecino[derecho] = 0;
  const int tamListaProcesos=cantidadDeProcesos+3;
  const int valorTokenViajero=cantidadDeProcesos;
  const int valorTurno=cantidadDeProcesos+1;
  const int cantidadProcesosActivos=cantidadDeProcesos+2;
  int listaProcesosDisponibles[valorTurno], vidaProceso;
  //Se aprovecha la variable listaProcesosDisponibles
  //La pos de 0 a cantidadDeProcesos-1, contendran 1, que indica si el proceso puede seguir jugando o no
  //La pos = cantidadDeProcesos indica el valor del tokenViajero
  //La pos = cantidadDeProcesos+1 indica el valor del turno
  //La pos = cantidadDeProcesos+2 indica la cantidad de procesos activos
  if (idProceso==0){
  inicializaListaPos(listaProcesosDisponibles,cantidadDeProcesos);
	}
  listaProcesosDisponibles[valorTurno]=0; //se inicializa el listaProcesosDisponibles en 0 para que empiece el proceso 0 el juego
  listaProcesosDisponibles[valorTokenViajero]=token;
  listaProcesosDisponibles[cantidadProcesosActivos]=cantidadDeProcesos;
  vidaProceso=listaProcesosDisponibles[cantidadProcesosActivos];

  //////////////////////////////////

//Antes de entrar al ciclo, se debe asegurar que todos los procesos hayan preasignado los anteriores valores
  
  MPI_Barrier(MPI_COMM_WORLD);

  while(cantidadDeProcesos!=1){ //Parecido a un while true, pero si solo hay un proceso de entrada, no es necesario entrar al ciclo y se declara el ganador de forma automatica

  	if (listaProcesosDisponibles[valorTurno]==idProceso){
		listaProcesosDisponibles[valorTurno]=vecino[derecho];

		if (listaProcesosDisponibles[idProceso]==1&&vidaProceso!=1){ //Si es que estoy habilitado para jugar
			if (listaProcesosDisponibles[valorTokenViajero]<0){ //Tengo que empezar el juego de nuevo!
				listaProcesosDisponibles[valorTokenViajero]=token-numeroAleatorio(token);
				cout <<"proceso "<<idProceso<<" tiene la papa con valor "<<listaProcesosDisponibles[valorTokenViajero]<< endl;
			}

			else{ //Tengo que restar un numero aleatorio al tokenViajero
				listaProcesosDisponibles[valorTokenViajero]=listaProcesosDisponibles[valorTokenViajero]-numeroAleatorio(token);
				if (listaProcesosDisponibles[valorTokenViajero]<0){ //Si el valor quedo negativo
					//if condicion de escritura
					listaProcesosDisponibles[idProceso]=0; //mi proceso sale del juego !!
					listaProcesosDisponibles[cantidadProcesosActivos]=listaProcesosDisponibles[cantidadProcesosActivos]-1;
						//if (listaProcesosDisponibles[cantidadProcesosActivos]!=1){
						cout <<"proceso "<<idProceso<<" tiene la papa con valor "<<listaProcesosDisponibles[valorTokenViajero]<< " (proceso "<<idProceso<< " sale del juego)" << endl;
						//}
					}
				
				else{
					cout <<"proceso "<<idProceso<<" tiene la papa con valor "<<listaProcesosDisponibles[valorTokenViajero]<< endl;
				}

			}
		}

  		MPI_Issend(listaProcesosDisponibles,tamListaProcesos,MPI_INT,vecino[derecho],mensajeTag,MPI_COMM_WORLD,&send_request);
  	}

  	if (vidaProceso!=1){ //Mientras sea distinta a la condicion de borde, seguire recibiendo
  		MPI_Recv(listaProcesosDisponibles,tamListaProcesos,MPI_INT,vecino[izquierdo],mensajeTag,MPI_COMM_WORLD,&recv_status);
  	}


    if (listaProcesosDisponibles[valorTurno]==vecino[izquierdo]){

    	MPI_Wait(&send_request,&send_status);
	}

	//Caso excepcional, para evitar Deadlock cuando solo juegan 2 procesos //
	if (vidaProceso==1&&cantidadDeProcesos==2){
		if (listaProcesosDisponibles[idProceso]==1){
			//break;
   	  		for (int i=0;i<cantidadDeProcesos;i++){
   	  			if (listaProcesosDisponibles[i]==1){
   	  				cout << "Proceso " << i <<" es el ganador"<<endl;
   	  		}
   	  	}
  	}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////

    if (vidaProceso==1){ //Si se cumple la condicion de borde
    	break;
    }
    vidaProceso=listaProcesosDisponibles[cantidadProcesosActivos];

  }//Fin While (cantidadDeProcesos!=1)

  MPI_Barrier(MPI_COMM_WORLD);

  if (idProceso==0){
   	  for (int i=0;i<cantidadDeProcesos;i++){
   	  	if (listaProcesosDisponibles[i]==1){
   	  		cout << "Proceso " << i <<" es el ganador"<<endl;
   	  	}
   	  }
  }

	MPI_Finalize();
	return 0;
}
