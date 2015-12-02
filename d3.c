#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
//~ #include <mpi.h>


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

void multiplyMatrix(int** A, int** B, int** C, int n){
	int i,j,k,s;
	for ( i = 0 ; i < n ; i++ )
		for ( j = 0 ; j < n ; j++ ) {
			s = 0;
			for ( k = 0 ; k < n ; k++ )
				s += A[i][k]*B[k][j];
			C[i][j] = s;
		}
}

int main(int argc, char** argv){
	
	// Booléen valant 0 si l'utilisateur ne veut 
	// pas afficher la matrice, 1 sinon
	int printMatrix = 0, c, n, index;
	
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
	
	// lit les matrices A et B et construit C
	int** A = (int**) malloc(sizeof(int*)*n);
	int** B = (int**) malloc(sizeof(int*)*n);
	int** C = (int**) malloc(sizeof(int*)*n);
	int i;
	for ( i = 0 ; i < n ; i++ )
		C[i] = (int*) malloc(n*sizeof(int));
	readMatrix(A, n);
	readMatrix(B, n);
	
	multiplyMatrix(A, B, C, n);

	if ( printMatrix ) afficheMatrix(C, n);
	
	for ( i = 0 ; i < n ; i++ ){
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}
	free(A);
	free(B);
	free(C);
	
	return 0;
}
