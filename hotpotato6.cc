#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int my_rank, ncpus;
  int left_neighbor, right_neighbor;
  int data_received=-1;
  int send_tag = 101, recv_tag=101;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &ncpus);

  left_neighbor = (my_rank-1 + ncpus)%ncpus;
  right_neighbor = (my_rank+1)%ncpus;

  while(true){
  MPI_Sendrecv(&my_rank, 1, MPI_INT, left_neighbor, send_tag,
               &data_received, 1, MPI_INT, right_neighbor, recv_tag,
	      MPI_COMM_WORLD, &status);
  
  printf("Among %d processes, process %d received from right neighbor: %d\n",
	 ncpus, my_rank, data_received);
}

  // clean up
  MPI_Finalize();
  return 0;
}  