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

void inicializaListaPos(int *lista, int tam){
	for (int i=0;i<tam;i++){
		lista[i]=1;
	}
}

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

    int 	message[cantidadDeProcesos];  // Lista que contiene todos los procesos disponibles 1 disponible 0 no existe
    int 	capsula[2]; //primer elemento es el valor del token, el segundo, si es 1 hay que realizar broadcast, el tercero es un contador, cada proceso que reciba la capsula le sumar
    int 	tokenParcial;
    MPI_Comm grid_comm;
	int dim_sizes[1];
	int wrap_around[1];
	int reorder = 1;
	dim_sizes[0] = 1;
	wrap_around[0] = 1; 


    /* Hasta que todos los procesos no lleguen hasta aqui ninguno continua */

    //crea un comunicador lineal periodico a partir de MPI_COMM_WORLD
	MPI_Cart_create(MPI_COMM_WORLD, 2, dim_sizes,wrap_around, reorder, &grid_comm);
	MPI_Barrier(MPI_COMM_WORLD);    
	start = MPI_Wtime();
    inicializaListaPos(message,cantidadDeProcesos);
    if (my_rank==0){
    	tokenParcial=token-numeroAleatorio(token);
 		
 		if (tokenParcial<0){
 			cout <<"proceso "<<my_rank<<" tiene la papa con valor "<<tokenParcial<<" (proceso" <<my_rank<<" sale del juego)" << endl;
 			//le envia el token al vecino
 			//capsula[0]=tokenParcial;
 			//capsula[1]=1;
 			//MPI_Send(capsula,2,MPI_INT, my_rank+1, tag, MPI_COMM_WORLD/*comunicador comun*/);
 			message[my_rank]=0;
 			MPI_Bcast(message, cantidadDeProcesos, MPI_INT, 0, MPI_COMM_WORLD);
 		}

 		else{
 			cout <<"proceso "<<my_rank<<" tiene la papa con valor "<<tokenParcial<< endl;
 			message[my_rank]=0;
 			MPI_Bcast(message, cantidadDeProcesos, MPI_INT, 0, MPI_COMM_WORLD);
 			//MPI_Send(&tokenParcial,1,MPI_INT, my_rank+1, tag, MPI_COMM_WORLD/*comunicador comun*/);
 		}

    //MPI_Bcast(message, cantidadDeProcesos, MPI_INT, 0, MPI_COMM_WORLD);

    //MPI_Bcast(message, cantidadDeProcesos, MPI_INT, 0, MPI_COMM_WORLD);
	}

	else{
	//MPI_Recv(capsula, 2, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
    MPI_Bcast(message, cantidadDeProcesos, MPI_INT, 0, MPI_COMM_WORLD);
	}

    //while(true){
    //}

    //if (my_rank != 0)
	printf("Mi mensaje es '%d' Y yo soy el proceso %d y lo estoy recibiendo.\n",message[0],my_rank);

    MPI_Barrier(MPI_COMM_WORLD);  // Espero a que todos los procesos terminen para calcular el tiempo de finalizacion.
    stop = MPI_Wtime();

    if (my_rank == 0){
       printf("Tiempo empleado: %g\n",stop-start);
    }

    MPI_Finalize();

}

// 	/*Variables globales*/
// 	int myrank, cantidadProcesos, tag=100, rankUtilizado,tokenParcial;
// 	int *mensaje,procesoExcluido[1];
// 	mensaje = (int *) malloc (1* sizeof ( int));

// 	const int largo = 10000;

// 	bool inicio=true;
	
// 	MPI_Status status;

// 	/*------------------*/

// 	//Inicializacion trabajo openMPI
// 	MPI_Init(&argc, &argv);
// 	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
// 	MPI_Comm_size(MPI_COMM_WORLD, &cantidadProcesos);

// 	MPI_Comm  comm_world, comm_worker;
//   	MPI_Group group_world, group_worker;
//   	int ierr;

// 	//int *will_use,num_used;
// 	//MPI_Comm TIMS_COMM_WORLD;
// 	//MPI_Group new_group,old_group;

// 	if (myrank==0 && inicio==true){
// 		inicio=false;
// 		tokenParcial=token-numeroAleatorio(token);
// 		if (tokenParcial<0){
// 			cout <<"proceso "<<myrank<<" tiene la papa con valor "<<tokenParcial<<" (proceso" <<myrank<<" sale del juego)" << endl;
			
// 			comm_world = MPI_COMM_WORLD;
//   			MPI_Comm_group(comm_world, &group_world);
//   			mensaje[0]=myrank;
//   			procesoExcluido[0]=myrank;
//   			//int message=myrank;
//   			MPI_Group_excl(group_world, 1, procesoExcluido, &group_worker);  /* process 0 not member */
//   			MPI_Comm_create(comm_world, group_worker, &comm_worker); //se sobreEscribe el comunicador global por el nuevo menos el de rango actual
// 			//sprintf(message, "%d",myrank);
// 			MPI_Bcast(mensaje, 1, MPI_INT, myrank, comm_worker);
// 			//MPI_Bcast(numeroNoParticipa,1,MPI_INT,tag,TIMS_COMM_WORLD);

// 			//MPI_Send(myrank+1,1,MPI_INT, rank, tag, TIMS_COMM_WORLD/*comunicador comun*/);

// 		}//fin if (tokenParcial<0)

// 		else{
// 			cout <<"proceso "<<myrank<<" tiene la papa con valor "<<tokenParcial<< endl;
// 			mensaje[0]=100;
// 			comm_world = MPI_COMM_WORLD;
// 			MPI_Bcast(mensaje, 1, MPI_INT, myrank, comm_world);
// 			//Se lo envio al siguiente
// 			//MPI_Send(myrank+1,1,MPI_INT, rank, tag, TIMS_COMM_WORLD/*comunicador comun*/);

// 		}

// 	}

// 	MPI_Bcast(mensaje, 1, MPI_INT, 0, MPI_COMM_WORLD);

// 	MPI_Barrier(MPI_COMM_WORLD);  // Espero a que todos los procesos terminen para calcular el tiempo de finalizacion.


// 	cout <<"soy el rank " << myrank << "me llego el mensaje: " << mensaje << endl;

// 		MPI_Finalize();

// 	return 0;

// }

	// if (myrank==turno){
	// 	//pasa algo
	// }

		
	// 	int matriz [size-1][largo], total=0,sumaParcial=0;
		
	// 	for (int i=0; i<size-1;i++){
			
	// 		for (int z=0; z<largo;z++){
	// 			matriz[i][z]=i+1;
	// 		}
	// 	}

	// 	for (int pid=1; pid<size;pid++){
	// 		MPI_Send(&matriz[pid-1],largo,MPI_INT, pid, tag, MPI_COMM_WORLD/*comunicador comun*/);
	// 	}


	// 	//int mensaje=5, destinatario=1;
	// 	//MPI_Send(&mensaje,1,MPI_INT, destinatario, tag, MPI_COMM_WORLD/*comunicador comun*/);

	// 	for (int pid=1; pid<size;pid++){
	// 		MPI_Recv(&sumaParcial, 1, MPI_INT, pid, tag, MPI_COMM_WORLD, &status);
	// 		total+=sumaParcial;
	// 	}
	// 	cout << "total es: "<< total << endl;
	// }

	// else{

	// 	int buzon [largo], padre=0,suma=0;
	// 	//MPI_Status status;

	// 	MPI_Recv(buzon, largo, MPI_INT, padre, tag, MPI_COMM_WORLD, &status);
		
	// 	for (int i=0; i<largo;i++){
	// 		suma+=buzon[i];
	// 	}

	// 	MPI_Send(&suma,1,MPI_INT, padre, tag, MPI_COMM_WORLD/*comunicador comun*/);		

	// 	//cout << "Mensaje recibido: " << buzon << endl;
	
	
/*	MPI_Finalize();

	return 0;

//mpiexec -np 10 ./ejemplo.exe
}*/