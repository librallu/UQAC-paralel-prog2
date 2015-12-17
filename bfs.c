#include <stdio.h>
#include <stdlib.h>
#include "sudokuHelper.h"


int backtrack(int* grid, int** possibles, int n){
	int a,b;
	
	// starting by simplifying the grid
	simplify_all(grid, n, possibles);
	//~ display(grid, n);
	//~ printf("----\n");
	
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
			//~ printf("chosen position: %d,%d\n",a,b);
			//~ printf("we choose %d \n", k);
			//~ display(grid, n);
			//~ printf("----\n");
			
			// call the backtrack operation
			// if we found the solution
			if ( backtrack(grid, possibles, n) ) {
				free(grid_backup);
				return 1;
			}
			
			// if not correct, restore the grid
			copy_grid(grid, grid_backup, n); // grid <- grid_backup
			//~ printf("restoring grid\n");
			//~ display(grid,n);
			//~ printf("====\n");
		}
	}
		
	// if arrived here, none of the solutions works
	free(grid_backup);
	return 0;

}





int main(int argc, char* argv[]){
	
	int *grid;
	int n,i,j;
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
	
	bfs(grid, possibles, n, );
	
	if ( !sudoku_verification(grid, n) ) 
		printf("incorrect\n");
	else
		display(grid, n);
	
	
	//~ printf("------\n");
	
	
	for ( i = 0 ; i < n*n ; i++ )
		for ( j = 0 ; j < n*n ; j++ )
			free(possibles[n*n*i+j]);
	free(possibles);
	free(grid);
}
