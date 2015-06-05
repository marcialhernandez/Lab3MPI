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
//capsula[1]=izquierda
//capsula[2]=derecha
void analizaListaProcesos(int * listaProcesos,int tamListaProcesos, int * capsula,int rank,int cantidadProcesos){
	//calculaIzquierda
	bool banderaIzquierdaAusente=true;
	bool banderaDerechaAusente=true;
	bool banderaUnico=true;
	for (int i=rank;i>-1;i--){
		if (listaProcesos[i]==1){
			capsula[1]=i;
			banderaIzquierdaAusente=false;
			banderaUnico=false;
			break;
		} 
	}//fin for

	if (banderaIzquierdaAusente==true){
		for (int i=cantidadProcesos-1;i>rank;i--){
			if (listaProcesos[i]==1){
				capsula[1]=i;
				banderaUnico=false;
				break;
			}

		}//fin for

	}//fin if

	if (banderaUnico==true){
		capsula[1]=rank;
		capsula[2]=rank;
	}//fin if

	else{ //Si no es unico, implica que existe una derecha distinta a la izquierda
		//calcula derecha
		for (int i=rank;i<cantidadProcesos;i++){
			if (listaProcesos[i]==1){
				capsula[1]=i;
				banderaDerechaAusente=false;
				break;
			} 

		} //fin for	

		if (banderaDerechaAusente==true){
			//significa que hay que darse la vuelta
			for (int i=0;i<rank;i++){
				if (listaProcesos[i]==1){
					capsula[1]=i;
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

int myrank, nprocs, leftid, rightid;
  int /*val, sum, tmp*/turno[1],mensajeTag=0,contador=0; //contador de momento limitara el while true
  MPI_Status wait_status;
  MPI_Request recv_request;
  turno[0]=0; //se inicializa el turno en 0 para que empiece el proceso 0 el juego
  
  /*----------------*/
  /* Initialize MPI */
  /*----------------*/

  MPI_Init(&argc, &argv);

  /*--------------------------------------------------------------------*/
  /* Find out my rank and size using global communicator MPI_COMM_WORLD */
  /*--------------------------------------------------------------------*/

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
     
  /*--------------------*/
  /* Find out neighbors */
  /*--------------------*/

  if ((leftid=(myrank-1)) < 0) leftid = nprocs-1;
  if ((rightid=(myrank+1)) == nprocs) rightid = 0;
  
  /*---------------------------------------------------------------------*/
  /* Send the process rank stored as val to the process on my right and  */
  /* receive a process rank from the process on my left and store as tmp */
  /*---------------------------------------------------------------------*/

  //val = myrank;
  //sum = 0;
  while(contador!=10){

  	MPI_Irecv(turno,1, MPI_INT, leftid, mensajeTag, MPI_COMM_WORLD, &recv_request);
  	if (turno[0]==myrank){
  		printf("Proc %d recibe variable turno = %d \n", myrank, turno[0]);
  		turno[0]=rightid;
  		//Acondicionamiento para salir del while
  		if (contador==9){
  			MPI_Wait(&recv_request,&wait_status);
  			break;
  		}
  		//Acondicionamiento para ultimo eslabon
  		MPI_Ssend(turno,1,MPI_INT,rightid,mensajeTag, MPI_COMM_WORLD);
  		//    MPI_Issend(&val,1,MPI_INT,rightid,99,MPI_COMM_WORLD,&send_request);
  		
  	}
    //MPI_Irecv(&tmp,1, MPI_INT, leftid, mensajeTag, MPI_COMM_WORLD, &recv_request);
    //MPI_Ssend(&val,1,MPI_INT,rightid,mensajeTag, MPI_COMM_WORLD);
    MPI_Wait(&recv_request,&wait_status);
	//MPI_Wait(&recv_request,&wait_status);
    //val = tmp;
    //sum += val;
    contador++;
  } //while (val != myrank);

  //El proceso 0 intercepta al ultimo eslabon
  if (myrank==0){
  	MPI_Irecv(turno,1, MPI_INT, leftid, mensajeTag, MPI_COMM_WORLD, &recv_request);
  	MPI_Wait(&recv_request,&wait_status);
  }

  //MPI_Irecv(turno,1, MPI_INT, leftid, mensajeTag, MPI_COMM_WORLD, &recv_request);
  //MPI_Wait(&recv_request,&wait_status);
  /*--------------------------*/
  /* Print the output message */
  /*--------------------------*/

  /*-----------------------*/
  /* Exit and finalize MPI */
  /*-----------------------*/

  MPI_Finalize();

}
