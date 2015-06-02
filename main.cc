#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {

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