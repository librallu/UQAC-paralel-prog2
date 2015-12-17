#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <mpi.h>

int main(int argc, char** argv){
	
	int *mainGrid, *tmpGrid;
	int n,i,j,k,res,over = 0;
	if (argc < 2) {
		printf("usage: %s filename\n", argv[0]);
		return 1;
    }
	
	
	int **grids;
	grids = malloc(sizeof(int*)*(QUEUE_LEN));
	
	// initialisation de MPI
	int size, rank;
	double t;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if(rank == 0){
		// code master
		FILE* f = fopen(argv[1], "r");
		fscanf(f, "%d", &n);
		mainGrid = (int*) malloc(sizeof(int)*n*n*n*n);
		for ( i = 0 ; i < n*n*n*n ; i++ ) fscanf(f, "%d", &(mainGrid[i]));
		fclose(f);
		display(mainGrid, n);
		buildQueue(mainGrid);
		
		
	} 
	
	//broadcast size of grid
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	
	if(rank != 0){
		// code node
		tmpGrid = (int*) malloc(sizeof(int)*n*n*n*n);
		// tant qu'on a pas trouv� de solution
		while(!over){
			// recevoir une grille
			MPI_Recv(tmpGrid,n*n*n*n,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// tester la branche 
			result = bnbLocal();
			// envoyer le r�sultat
			MPI_Send(&result,1,MPI_INT,0,0,MPI_COMM_WORLD);
			// attendre r�ponse pour savoir si on aura une autre tache
			MPI_Recv(&over,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			
			if (over && result){ // si la recherche est termin�e et qu'on est d�tenteur de la solution
				//envoyer la grille qu'on a trouv�
				MPI_Send(tmpGrid,n*n*n*n,MPI_INT,0,0,MPI_COMM_WORLD);
			}
			//sinon recommencer
		}
	} else {
		MPI_Request* requests = malloc (sizeof(MPI_Request)*size);
		int * results = (int*) calloc(size,sizeof(int));
		//envoyer du travail au cluster
		dispatchWork();
		// mettre en oeuvre les callbacks de r�ponse
		for (i = 0; i < size; i++){
			MPI_Irecv(&(results[i]),1,MPI_INT,i,0,MPI_COMM_WORLD,&(requests[i]));
		}
		
		// tant qu'on a pas fini de chercher, bah on cherche
		while(!over){
			//chercher un peu
			if(!bnbLocal()) {
				//si on ne trouve pas de notre cot� �couter les autres
				MPI_Testany();
				// quand on a une r�ponse
				for (j=0;j<size; j++){
					//chercher si la r�ponse re�ue est positive
					over |= results[j];
					if(over){ // on a trouv� chez les voisins
						// r�cup�rer la solution
						MPI_Send(&over,1,MPI_INT,j,0,MPI_COMM_WORLD);
						MPI_Recv(mainGrid,n*n*n*n,MPI_INT,j,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
						// et arr�ter tout le monde
						for (k=0;k<size;k++){
							if (k!=j) {
								MPI_Wait(&(requests[k]),MPI_STATUS_IGNORE);
								MPI_Send(&over,1,MPI_INT,k,0,MPI_COMM_WORLD);
							}
						}
						break;
					}
				}
			}
		}
	}
	
	printf("--------\n");
	display(mainGrid,n);
	
	MPI_Barrier(MPI_COMM_WORLD);
	// on a fini avec MPI
	MPI_Finalize();
	
	return 0;
}
