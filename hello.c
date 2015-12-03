#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>



int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	printf("Hello from %d, size: %d\n", rank, size);
	
	// Tout le monde se synchronise
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Finalize();
	return 0;
}
