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

	int         my_rank;       // Rank del proceso
    int         cantidadDeProcesos;             // Numero de procesos
    int         source;        // Rank del que envia
    int         dest;          // Rank del que recibe
    int         tag = 0;       // Tag del mensaje
    //int        message[1];  // Mensaje
    MPI_Status  status;

    double   start,stop;

    /* Inicio */
    MPI_Init(&argc, &argv);

    /* Averiguando el Rank del proceso */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Averiguando el numero de procesos que participan */
    MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);


    int tokenViajero=0,position;
    const int tamListaProcesos=cantidadDeProcesos+1;
    int listaProcesosDisponibles[tamListaProcesos]; //El ultimo elemento en la pos cantidadDeProcesos contiene al token viajero
    int turno=0;

    if (my_rank==0){
    	//tokenViajero=token-numeroAleatorio(token);
    	//cout <<"proceso "<<my_rank<<" tiene la papa con valor "<<tokenViajero<< endl;

    	//listaProcesosDisponibles[cantidadDeProcesos]=tokenViajero;
    	inicializaListaPos(listaProcesosDisponibles,cantidadDeProcesos); //se inicializa hasta el tam-1
    	//cout <<"proceso: " <<my_rank<<" envia mensaje a: " << dimeElVecinoDer(my_rank,cantidadDeProcesos) << endl;

    	//MPI_Rsend(listaProcesosDisponibles,tamListaProcesos,MPI_INT, dimeElVecinoDer(my_rank,cantidadDeProcesos), tag, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Espero a que todos los procesos terminen para calcular el tiempo de finalizacion.


    while (true){
    	if (turno==my_rank){
    		//cout << "soy el proceso:"<< my_rank<< " y me llego: "<<turno << endl;
    		turno=dimeElVecinoDer(my_rank,cantidadDeProcesos);
			MPI_Ssend(&turno,1,MPI_INT, dimeElVecinoDer(my_rank,cantidadDeProcesos), tag, MPI_COMM_WORLD);   
			//MPI_Barrier(MPI_COMM_WORLD); 	
		}

    	else{
    		cout << "esperando mensaje de: " << dimeElVecinoIzq(my_rank,cantidadDeProcesos) << endl;
    		MPI_Recv(&turno, 1, MPI_INT, dimeElVecinoIzq(my_rank,cantidadDeProcesos) , tag, MPI_COMM_WORLD, &status);
    	}
    				    	    		    MPI_Barrier(MPI_COMM_WORLD);  // Espero a que todos los procesos terminen para calcular el tiempo de finalizacion.
    				    	    		    fflush(stdout);
    	    		cout << "soy el proceso:"<< turno<< " y me llego: "<<turno << endl;
    	    		    MPI_Barrier(MPI_COMM_WORLD);  // Espero a que todos los procesos terminen para calcular el tiempo de finalizacion.



			/*cout <<"proceso: " <<my_rank<<" envia mensaje a: " << dimeElVecinoDer(my_rank,cantidadDeProcesos) << endl;    		
    		MPI_Rsend(&turno,1,MPI_INT, dimeElVecinoDer(my_rank,cantidadDeProcesos), tag, MPI_COMM_WORLD);*/

    	//}

    	//else{
    		/*cout <<"proceso: " <<my_rank<<" obtiene mensaje de: " << dimeElVecinoIzq(my_rank,cantidadDeProcesos) << endl;
    		MPI_Recv(&turno, 1, MPI_INT, dimeElVecinoIzq(my_rank,cantidadDeProcesos) , tag, MPI_COMM_WORLD, &status);*/
    	//}

	}

	MPI_Finalize();

    //return 0;

}

