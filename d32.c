#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <mpi.h>

// Fonction qui affiche comment se servir du programme
void printHelp(char* name){
	printf("usage: %s [-p] n\n", name);
}


// Afiche la matrice M sur la sortie standard
void afficheMatrix(int** M, int n) {
	int i,j;
	for ( i = 0 ; i < n ; i++ ){
		for ( j = 0 ; j < n ; j++ )
			printf("%d ", M[i][j]);
		printf("\n");
	}
}

// Lit la matrice M sur l'entrée standard
void readMatrix(int** M, int n) {
	int i,j;
	for ( i = 0 ; i < n ; i++ ){
		for ( j = 0 ; j < n ; j++ )
			scanf("%d", &(M[i][j]));
	}
}

// Multiplication avec MPI de la matrice
// on rajoute le paramètre rank et size pour
// diviser la charge de travail sur chacun des noeuds
void multiplyMatrix(int** A, int** B, int** C, int n, int rank, int size){
	int i,j,k,s;
	MPI_Status status;
	
	// Partie calcul à proprement parler
	for ( i = rank ; i < n ; i+=size ) {
		for ( j = 0 ; j < n ; j++ ) {
			s = 0;
			for ( k = 0 ; k < n ; k++ )
				s += A[i][k]*B[k][j];
			C[i][j] = s;
		}
	}
	
	// partie envoi/réception de messages
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
	// Tout le monde se synchronise pour terminer la fonction
	MPI_Barrier(MPI_COMM_WORLD);
}

// Initialise la structure des matrices A, B, C
void initializeMatrix(int** A, int** B, int** C, int n){
	int i; // initialize all the matrices
	for ( i = 0 ; i < n ; i++ ) {
		A[i] = (int*) malloc(n*sizeof(int));
		B[i] = (int*) malloc(n*sizeof(int));
		C[i] = (int*) malloc(n*sizeof(int));
	}
}

int main(int argc, char** argv){
	
	// initialisation de MPI
	int size, rank, i;
	double t;
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// Booléen valant 0 si l'utilisateur ne veut 
	// pas afficher la matrice, 1 sinon
	int printMatrix = 0;
	
	int c, n, index;
	
	int** A;
	int** B;
	int** C;

	// Le maitre lit sur l'entrée standard les différentes informations
	// nécessaires (n et matrices en entrée)
	if ( rank == 0 ) {
	
		// détermine la valeur de p sur la ligne de commande
		while ((c=getopt(argc,argv, "ph")) != -1 ){
			switch(c) {
				case 'p': // Si l'utilisateur veut afficher la matrice
					printMatrix = 1;
					break;
				case 'h': // si l'utilisateur veut afficher l'aide
					printHelp(argv[0]);
					exit(0);
					break;
				default:
					break;
			}
		}
		
		// lit n
		scanf("%d", &n);
		
		// lit les matrices A et B et construit C
		A = (int**) malloc(sizeof(int*)*n);
		B = (int**) malloc(sizeof(int*)*n);
		C = (int**) malloc(sizeof(int*)*n);
		initializeMatrix(A,B,C,n);
		readMatrix(A, n);
		readMatrix(B, n);
		
	}
	
	// Échange d'information pour n
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if ( rank != 0 ){
		// Pour les esclaves, les matrices ne sont pas initialisées
		// et la valeur de n était requise, c'est pourquoi
		// on initialise les matrices ici
		A = (int**) malloc(sizeof(int*)*n);
		B = (int**) malloc(sizeof(int*)*n);
		C = (int**) malloc(sizeof(int*)*n);
		initializeMatrix(A,B,C,n);
	}
	
	// Échange d'information sur les matrices
	for ( i = 0 ; i < n ; i++ ) {
		MPI_Bcast(A[i], n, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(B[i], n, MPI_INT, 0, MPI_COMM_WORLD);
	}
	
	// Multiplication de la matrice
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0) t=MPI_Wtime();
	multiplyMatrix(A, B, C, n, rank, size);
	
	// on a fini avec MPI
	MPI_Finalize();
	
	// Affichage du temps d'exécution
	if(rank == 0) {
		t=MPI_Wtime()-t;
		printf("Execution time : %lf\n", t);
	}

	// affichage si demandé
	if ( printMatrix ) afficheMatrix(C, n);
	
	// Libération de la mémoire
	for ( i = 0 ; i < n ; i++ ){
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}
	free(A);
	free(B);
	free(C);
		
	// Fin du programme
	return 0;
}
