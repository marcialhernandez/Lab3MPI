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

	int myrank, size, tag=99;

	const int largo = 10000;
	
	MPI_Status status;

	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (myrank==0){
		
		int matriz [size-1][largo], total=0,sumaParcial=0;
		
		for (int i=0; i<size-1;i++){
			
			for (int z=0; z<largo;z++){
				matriz[i][z]=i+1;
			}
		}

		for (int pid=1; pid<size;pid++){
			MPI_Send(&matriz[pid-1],largo,MPI_INT, pid, tag, MPI_COMM_WORLD/*comunicador comun*/);
		}


		//int mensaje=5, destinatario=1;
		//MPI_Send(&mensaje,1,MPI_INT, destinatario, tag, MPI_COMM_WORLD/*comunicador comun*/);

		for (int pid=1; pid<size;pid++){
			MPI_Recv(&sumaParcial, 1, MPI_INT, pid, tag, MPI_COMM_WORLD, &status);
			total+=sumaParcial;
		}
		cout << "total es: "<< total << endl;
	}

	else{

		int buzon [largo], padre=0,suma=0;
		//MPI_Status status;

		MPI_Recv(buzon, largo, MPI_INT, padre, tag, MPI_COMM_WORLD, &status);
		
		for (int i=0; i<largo;i++){
			suma+=buzon[i];
		}

		MPI_Send(&suma,1,MPI_INT, padre, tag, MPI_COMM_WORLD/*comunicador comun*/);		

		//cout << "Mensaje recibido: " << buzon << endl;
	}
	
	
	MPI_Finalize();

	return 0;

//mpiexec -np 10 ./ejemplo.exe
}