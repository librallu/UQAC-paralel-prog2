#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <mpi.h>


void printHelp(char* name){
	printf("usage: %s [-p] n\n", name);
}

void afficheMatrix(int** M, int n) {
	int i,j;
	for ( i = 0 ; i < n ; i++ ){
		for ( j = 0 ; j < n ; j++ )
			printf("%d ", M[i][j]);
		printf("\n");
	}
}

void readMatrix(int** M, int n) {
	int i,j;
	for ( i = 0 ; i < n ; i++ ){
		M[i] = (int*) malloc(n*sizeof(int));
		for ( j = 0 ; j < n ; j++ )
			scanf("%d", &(M[i][j]));
	}
}

//~ void multiplyMatrix(int** A, int** B, int** C, int n){
	//~ int i,j,k,s;
	//~ for ( i = 0 ; i < n ; i++ )
		//~ for ( j = 0 ; j < n ; j++ ) {
			//~ s = 0;
			//~ for ( k = 0 ; k < n ; k++ )
				//~ s += A[i][k]*B[k][j];
			//~ C[i][j] = s;
		//~ }
//~ }

void multiplyMatrix(int** A, int** B, int** C, int n, int rank, int size){
	int i,j,k,s;
	MPI_Status status;
	for ( i = rank ; i < n ; i+=size ) {
		for ( j = 0 ; j < n ; j++ ) {
			s = 0;
			for ( k = 0 ; k < n ; k++ )
				s += A[i][k]*B[k][j];
			C[i][j] = s;
		}
	}
	if ( rank != 0 ) { // si un esclave finit le travail
		for ( i = rank ; i < n ; i+=size )
			// envoi de message au maitre
			MPI_Send(C[i], n, MPI_INT, 0, i, MPI_COMM_WORLD);
	} else { // Le maitre reçoit le travail des esclaves
		for ( j = 1 ; j < size ; j++ ) {
			for ( i = j ; i < n ; i+=size )
				MPI_Recv(C[i], n, MPI_INT, j, i, MPI_COMM_WORLD, &status);
		}
	}
	// Tout le monde se synchronise
	MPI_Barrier(MPI_COMM_WORLD);
}

void send_n(int* n) {
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void send_matrix(int **A, int **B, int n) {
	int* data = (int*) malloc(sizeof(int)*2*n*n);
	
	free(data);
}


int main(int argc, char** argv){
	
	// initialisation de MPI
	int size, rank;
	double t;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// Booléen valant 0 si l'utilisateur ne veut 
	// pas afficher la matrice, 1 sinon
	int printMatrix = 0, c, n, index;
	
	int** A = (int**) malloc(sizeof(int*)*n);
	int** B = (int**) malloc(sizeof(int*)*n);
	int** C = (int**) malloc(sizeof(int*)*n);
	
	if ( rank == 0 ) {
	
		// détermine la valeur de p sur la ligne de commande
		while ((c=getopt(argc,argv, "ph")) != -1 ){
			switch(c) {
				case 'p':
					printMatrix = 1;
					break;
				case 'h':
					printHelp(argv[0]);
					exit(0);
					break;
				default:
					break;
			}
		}
		
		scanf("%d", &n);
		
		//~ // lit les matrices A et B et construit C
		//~ int i;
		//~ for ( i = 0 ; i < n ; i++ )
			//~ C[i] = (int*) malloc(n*sizeof(int));
		//~ readMatrix(A, n);
		//~ readMatrix(B, n);
		
		

		
		//~ // Multiplication de la matrice
		//~ MPI_Barrier(MPI_COMM_WORLD);
		//~ if(rank == 0) t=MPI_Wtime();
		//~ multiplyMatrix(A, B, C, n, rank, size);
		
		//~ // on a fini avec MPI
		//~ MPI_Finalize();
		
		//~ // Affichage du temps d'exécution
		//~ if(rank == 0) {
			//~ t=MPI_Wtime()-t;
			//~ printf("Execution time : %lf\n", t);
		//~ }

		//~ // affichage si demandé
		//~ if ( printMatrix ) afficheMatrix(C, n);
		
	} else { // Si il s'agit d'un noeud différent de 0
		
	}
	
	send_n(&n);
	MPI_Barrier(MPI_COMM_WORLD);
	printf("rank:%d, n:%d\n", rank, n);
	
	// Libération de la mémoire
	//~ for ( i = 0 ; i < n ; i++ ){
		//~ free(A[i]);
		//~ free(B[i]);
		//~ free(C[i]);
	//~ }
	//~ free(A);
	//~ free(B);
	//~ free(C);
	
	return 0;
}
