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

//Instruccion para compilar en consola
//mpic++ -W -Wall main.cc -o  casa

using namespace std;


//	x = rand() % n  + 1; 

//Bu
/*
int retornaVecinoAnterior(MPI_Group group, rank){
	int cantidadProcesos,usedId;
	MPI_Comm_size(MPI_COMM_WORLD, &cantidadProcesos);
	int i=rank-1;

	while (i!=0){
		MPI_Group_rank(group,&usedId);
		if &

	}



}*/

int dimeElVecinoIzq(int rankActual,int cantidadProcesos){
	if (rankActual==0){
		return cantidadProcesos-1;
	}

	else{
		return rankActual-1;
	}
}

int dimeElVecinoDer(int rankActual,int cantidadProcesos){
	if (rankActual==cantidadProcesos-1){
		return 0;
	}

	else{
		return rankActual+1;
	}
}

//Hay ganador! = true
//No hay ganador! = false
bool checkGanador(int *lista, int tam){
	int contadorParticipantes=0;
	for (int i=0;i<tam;i++){
		if(lista[i]==1){
			contadorParticipantes=contadorParticipantes+1;
		}

		if (contadorParticipantes>1){
			return false;
		}
	}

	return true;
}

void inicializaListaPos(int *lista, int tam){
	for (int i=0;i<tam;i++){
		lista[i]=1;
	}
}

//Deja los resultados en capsula
//capsula[0]=izquierda
//capsula[1]=derecha
void obtieneIzqAndDer(int * listaProcesos,int tamListaProcesos, int * capsula,int rank,int cantidadProcesos){
	//calculaIzquierda
	bool banderaIzquierdaAusente=true;
	bool banderaDerechaAusente=true;
	bool banderaUnico=true;
	for (int i=rank;i>-1;i--){
		if (listaProcesos[i]==1){
			capsula[0]=i;
			banderaIzquierdaAusente=false;
			banderaUnico=false;
			break;
		} 
	}//fin for

	if (banderaIzquierdaAusente==true){
		for (int i=cantidadProcesos-1;i>rank;i--){
			if (listaProcesos[i]==1){
				capsula[0]=i;
				banderaUnico=false;
				break;
			}

		}//fin for

	}//fin if

	if (banderaUnico==true){
		capsula[0]=rank;
		capsula[1]=rank;
	}//fin if

	else{ //Si no es unico, implica que existe una derecha distinta a la izquierda
		//calcula derecha
		for (int i=rank;i<cantidadProcesos;i++){
			if (listaProcesos[i]==1){
				capsula[0]=i;
				banderaDerechaAusente=false;
				break;
			} 

		} //fin for	

		if (banderaDerechaAusente==true){
			//significa que hay que darse la vuelta
			for (int i=0;i<rank;i++){
				if (listaProcesos[i]==1){
					capsula[0]=i;
					break;
				} 

			} //fin for	
		}//fin if (banderaDerechaAusente==true)

	}//fin else
}//fin funcion

void creaSubMundo(){
	//todavia no hace nada
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

	int token=0;

	/* Una cadena que lista las opciones cortas válidas para getOpt
	  Se inicia con : pues si falta algun argumento, enviara un caso tipo ":"" */

	const char* const opciones = "t:";

	if (recibeArgumentosConsola(opciones,argc, argv,&token) ==false){
		exit(1);
	}

	int myrank, cantidadDeProcesos,vecino[2];
  	int mensajeTag=0;
  	MPI_Status recv_status, send_status;
  	MPI_Request send_request;
  
  /*----------------*/
  /* Initialize MPI */
  /*----------------*/

  MPI_Init(&argc, &argv);

  /*--------------------------------------------------------------------*/
  /* Find out my rank and size using global communicator MPI_COMM_WORLD */
  /*--------------------------------------------------------------------*/

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
     
  /*--------------------*/
  /* Find out neighbors */
  /*--------------------*/

  //if ((leftid=(myrank-1)) < 0) leftid = cantidadDeProcesos-1;
  //if ((rightid=(myrank+1)) == cantidadDeProcesos) rightid = 0;

  //////////////////////////////////

  //Declaracion de variables para iniciar el juego

  const int izquierdo=0;
  const int derecho=1;
  if ((vecino[izquierdo]=(myrank-1)) < 0) vecino[izquierdo] = cantidadDeProcesos-1;
  if ((vecino[derecho]=(myrank+1)) == cantidadDeProcesos) vecino[derecho] = 0;
  const int tamListaProcesos=cantidadDeProcesos+3;
  const int valorTokenViajero=cantidadDeProcesos;
  const int valorTurno=cantidadDeProcesos+1;
  const int cantidadProcesosActivos=cantidadDeProcesos+2;
  int listaProcesosDisponibles[valorTurno], vidaProceso; //contador de momento limitara el while true
  if (myrank==0){
  inicializaListaPos(listaProcesosDisponibles,cantidadDeProcesos);
	}
  listaProcesosDisponibles[valorTurno]=0; //se inicializa el listaProcesosDisponibles en 0 para que empiece el proceso 0 el juego
  listaProcesosDisponibles[valorTokenViajero]=token;
  listaProcesosDisponibles[cantidadProcesosActivos]=cantidadDeProcesos;
  vidaProceso=listaProcesosDisponibles[cantidadProcesosActivos];

  //////////////////////////////////
  
  MPI_Barrier(MPI_COMM_WORLD);

  //Antes de entrar al ciclo, se debe asegurar que todos los procesos hayan preasignado los anteriores valores

  while(cantidadDeProcesos!=1){ //Si hay mas de 1 proceso

  	if (listaProcesosDisponibles[valorTurno]==myrank){
		listaProcesosDisponibles[valorTurno]=vecino[derecho];

		if (listaProcesosDisponibles[myrank]==1&&vidaProceso!=1){ //Si es que estoy habilitado para jugar
			if (listaProcesosDisponibles[valorTokenViajero]<0){ //Tengo que empezar el juego de nuevo!
				listaProcesosDisponibles[valorTokenViajero]=token-numeroAleatorio(token);
				cout <<"proceso "<<myrank<<" tiene la papa con valor "<<listaProcesosDisponibles[valorTokenViajero]<< endl;
			}

			else{ //Tengo que restar un numero aleatorio al tokenViajero
				listaProcesosDisponibles[valorTokenViajero]=listaProcesosDisponibles[valorTokenViajero]-numeroAleatorio(token);
				if (listaProcesosDisponibles[valorTokenViajero]<0){ //Si el valor quedo negativo
					//if condicion de escritura
					listaProcesosDisponibles[myrank]=0; //mi proceso sale del juego !!
					listaProcesosDisponibles[cantidadProcesosActivos]=listaProcesosDisponibles[cantidadProcesosActivos]-1;
						//if (listaProcesosDisponibles[cantidadProcesosActivos]!=1){
						cout <<"proceso "<<myrank<<" tiene la papa con valor "<<listaProcesosDisponibles[valorTokenViajero]<< " (proceso "<<myrank<< " sale del juego)" << endl;
						//}
					}
				
				else{
					cout <<"proceso "<<myrank<<" tiene la papa con valor "<<listaProcesosDisponibles[valorTokenViajero]<< endl;
				}

			}
		}

		//if (cantidadDeProcesos!=2&&vidaProceso!=1){
  		MPI_Issend(listaProcesosDisponibles,tamListaProcesos,MPI_INT,vecino[derecho],mensajeTag,MPI_COMM_WORLD,&send_request);
  		//}
  	}

  	if (vidaProceso!=1){ //Mientras sea distinta a la condicion de borde, seguire recibiendo
  		MPI_Recv(listaProcesosDisponibles,tamListaProcesos,MPI_INT,vecino[izquierdo],mensajeTag,MPI_COMM_WORLD,&recv_status);
  	}

  	//MPI_Recv(listaProcesosDisponibles,tamListaProcesos,MPI_INT,vecino[izquierdo],mensajeTag,MPI_COMM_WORLD,&recv_status);

    if (listaProcesosDisponibles[valorTurno]==vecino[izquierdo]){

    	MPI_Wait(&send_request,&send_status);
	}
    //printf("Proc %d recibe variable listaProcesosDisponibles = %d \n", myrank, listaProcesosDisponibles[valorTurno]);

	if (vidaProceso==1&&cantidadDeProcesos==2){
		if (listaProcesosDisponibles[myrank]==1){
			//break;
   	  		for (int i=0;i<cantidadDeProcesos;i++){
   	  			if (listaProcesosDisponibles[i]==1){
   	  				cout << "Proceso " << i <<" es el ganador"<<endl;
   	  		}
   	  	}
  	}
		return 0;
	}

    if (vidaProceso==1){ //Si se cumple la condicion de borde
    	break;
    }



    //vidaProceso--;
    vidaProceso=listaProcesosDisponibles[cantidadProcesosActivos];
  }

  MPI_Barrier(MPI_COMM_WORLD);


  if (myrank==0){
   	  for (int i=0;i<cantidadDeProcesos;i++){
   	  	if (listaProcesosDisponibles[i]==1){
   	  		cout << "Proceso " << i <<" es el ganador"<<endl;
   	  	}
   	  }
  }

	MPI_Finalize();

}
