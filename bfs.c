#include <stdio.h>
#include <stdlib.h>
#include "sudokuHelper.h"


int backtrack(int* grid, int** possibles, int n){
	int a,b;
	
	// starting by simplifying the grid
	simplify_all(grid, n, possibles);
	
	int cont = to_continue(grid, possibles, n);
	if ( cont == 1 ) { // we have got an error
		return 0;
	} else if ( cont == -1 ) { // we solved it
		return 1;
	}
	
	// creating a backup for the grid
	int* grid_backup = (int*) malloc(sizeof(int)*n*n*n*n);
	copy_grid(grid_backup, grid, n); // grid_backup <- grid
	
	// choose a position for backtracking
	choose_backtracking(grid, possibles, n, &a, &b);
	// listing all possibles values for the position
	int k;
	for ( k = 1 ; k <= n*n ; k++ ) {
		if ( possibles[a*n*n+b][k] ) { // if k is possible
			
			// affect k to the position
			grid[a*n*n+b] = k;

			// call the backtrack operation
			// if we found the solution
			if ( backtrack(grid, possibles, n) ) {
				free(grid_backup);
				return 1;
			}
			
			// if not correct, restore the grid
			copy_grid(grid, grid_backup, n); // grid <- grid_backup

		}
	}
		
	// if arrived here, none of the solutions works
	free(grid_backup);
	return 0;

}

/**
 * 
 * return 1 if solved, -1 if error, 0 elsewhere
 */
int bfs(int* grid, int** possibles, int n, int nmax, int** grids, int* nbGrids, int* start){
	
	// grids initialization
	int i,a,b,k,nbPossibles;
	for ( i = 0 ; i < nmax ; i++ )
		grids[i] = (int*) malloc(sizeof(int)*n*n*n*n);
	
	// enqueue the grid
	copy_grid(grids[0], grid, n);
	int queueStart = 0;
	int queueSize = 1;
	
	int* currentGrid;
	int stop = 0;
	while ( queueSize != 0 && !stop ) {
		// dequeue the grid
		currentGrid = grids[queueStart];
		
		//~ display(currentGrid,n);
		//~ printf(" - - - \n");
		
		// starting by simplifying the grid
		simplify_all(currentGrid, n, possibles);
		
		//~ printf("--- %d --- \n", queueStart);
		//~ display(currentGrid,n);
		//~ printf("---\n");
		
		int cont = to_continue(currentGrid, possibles, n);
		if ( cont == -1 ) { // we solved it
			copy_grid(grid, currentGrid, n);
			return 1;
		} else if ( cont == 1 ) { // invalid grid
			queueSize = queueSize-1;
			queueStart = (queueStart+1)%nmax;
		} else if ( cont == 0 ) {
			// choose a position for backtracking
			nbPossibles = choose_backtracking(currentGrid, possibles, n, &a, &b);
			//~ printf("nbPossibles : %d (%d,%d)\n", nbPossibles,a,b);
			if ( nbPossibles + queueSize > nmax ) {
				stop = 1;
				*nbGrids = queueSize;
				*start = queueStart;
			} else {
				for ( k = 1 ; k <= n*n ; k++ ) {
					if ( possibles[a*n*n+b][k] ) { // if k is possible
						//~ printf("using k %d\n",k);
						// copy a new grid
						copy_grid(grids[(queueStart+queueSize)%nmax], currentGrid, n);
						grids[(queueStart+queueSize)%nmax][a*n*n+b] = k;
						queueSize++;
					}
				}
				queueSize--;
				queueStart = (queueStart+1)%nmax;
			}
		}
		//~ printf("====\n");
	}
	
	if ( queueSize == 0 ) return -1;
	return 0;
	
}


int main(int argc, char* argv[]){
	
	int *grid;
		
	if (argc < 2) {
		printf("usage: %s filename\n", argv[0]);
		return 1;
	}
	FILE* f = fopen(argv[1], "r");
	fscanf(f, "%d", &n);
	grid = malloc(sizeof(int)*n*n*n*n);
	for ( i = 0 ; i < n*n*n*n ; i++ ) fscanf(f, "%d", &(grid[i]));
	fclose(f);
	
	int** possibles = (int**) malloc(sizeof(int*)*n*n*n*n);
	for ( i = 0 ; i < n*n ; i++ )
		for ( j = 0 ; j < n*n ; j++ )
			possibles[n*n*i+j] = (int*) malloc(sizeof(int)*n*n);
	
	//~ printf("Number of possible values: %d\n", get_possible_values(0,0,n,grid,res));
	//~ display(grid, n);
	//~ printf("------\n");
		
	int nmax = 2*n*n;
	int ** grids = (int**) malloc(sizeof(int*)*nmax);
	int nbGrids, start=0;
	int res = bfs(grid, possibles, n, nmax, grids, &nbGrids, &start);
	printf("res:%d\n",res);
	if ( res == 1 ) {
		display(grid, n);
	} else if ( res == -1 ) {
		printf("no solution\n");
	} else {
		printf("nb noeuds :%d starting at %d\n",nbGrids, start);
		for ( i = 0 ; i < nbGrids ; i++ ) {
			printf(" --- %d --- \n", (start+i)%nmax);
			display(grids[(start+i)%nmax],n);
		}
	}
	for ( i = 0 ; i < nmax ; i++ ) {
		free(grids[i]);
	}
	free(grids);
	
	
	
	for ( i = 0 ; i < n*n ; i++ )
		for ( j = 0 ; j < n*n ; j++ )
			free(possibles[n*n*i+j]);
	free(possibles);
	free(grid);
}
