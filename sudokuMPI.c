#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <mpi.h>

int main(int argc, char** argv){
	
	int *grid;
        int n,i,j;
        if (argc < 2) {
            printf("usage: %s filename\n", argv[0]);
            return 1;
    }
	
	FILE* f = fopen(argv[1], "r");
	fscanf(f, "%d", &n);
	grid = (int*) malloc(sizeof(int)*n*n*n*n);
	for ( i = 0 ; i < n*n*n*n ; i++ ) fscanf(f, "%d", &(grid[i]));
	fclose(f);
	
	
	// initialisation de MPI
	int size, rank;
	double t;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Barrier(MPI_COMM_WORLD); //attente de la connection de tous les noeuds du communicator
	
	if(rank == 0){
		display(grid, n);
		// code master
	} else {
		// code node
	}
	
	// on a fini avec MPI
	MPI_Finalize();
	
	return 0;
}
