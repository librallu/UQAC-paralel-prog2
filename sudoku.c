#include <stdio.h>
#include <stdlib.h>
#include "sudokuHelper.h"




// return 1 if error
// return -1 if terminated
// else return 0
int to_continue(int* grid, int** possibles, int n) {
	int a,b,end=-1, tmp;
	for ( a = 0 ; a < n*n ; a++ )
		for ( b = 0 ; b < n*n ; b++ )
			if ( !grid[a*n*n+b] ) {
				end = 0;
				tmp = compute_nb_possibles(possibles[a*n*n+b],n);
				if ( !tmp ) {
					//~ printf("error position: %d, %d\n",a,b);
					return 1;
				}
			}
	return end;
}

void copy_grid(int* dest, int* start, int n) {
	int i,j;
	for ( i = 0 ; i < n*n*n*n ; i++ ) dest[i] = start[i];
}

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

        // --- EXTREME ---
        //~ int grid[] = {
            //~ 8,5,0,0,0,2,4,0,0,
            //~ 7,2,0,0,0,0,0,0,9,
            //~ 0,0,4,0,0,0,0,0,0,
            //~ 0,0,0,1,0,7,0,0,2,
            //~ 3,0,5,0,0,0,9,0,0,
            //~ 0,4,0,0,0,0,0,0,0,
            //~ 0,0,0,0,8,0,0,7,0,
            //~ 0,1,7,0,0,0,0,0,0,
            //~ 0,0,0,0,3,6,0,4,0
        //~ };
        //~ int grid[] = {
            //~ 8,5,0,0,0,2,4,0,0,
            //~ 7,2,3,0,0,0,0,0,9,
            //~ 1,6,4,0,0,0,0,0,0,
            //~ 0,0,0,1,0,7,0,0,2,
            //~ 3,0,5,0,0,0,9,0,0,
            //~ 0,4,0,0,0,0,0,0,0,
            //~ 0,0,0,0,8,0,0,7,0,
            //~ 0,1,7,0,0,0,0,0,0,
            //~ 0,0,0,0,3,6,0,4,0
        //~ };

        //~ // --- HARD ---
        //~ int grid[] = {
            //~ 0,0,3,0,2,0,6,0,0,
            //~ 9,0,0,3,0,5,0,0,1,
            //~ 0,0,1,8,0,6,4,0,0,
            //~ 0,0,8,1,0,2,9,0,0,
            //~ 7,0,0,0,0,0,0,0,8,
            //~ 0,0,6,7,0,8,2,0,0,
            //~ 0,0,2,6,0,9,5,0,0,
            //~ 8,0,0,2,0,3,0,0,9,
            //~ 0,0,5,0,1,0,3,0,0
        //~ };

        // --- EASY ---
        //~ int grid[] = {
            //~ 0,1,3,5,2,4,9,0,8,
            //~ 5,2,4,9,8,7,0,1,3,
            //~ 0,7,8,6,0,0,5,0,4,
            //~ 3,4,0,1,9,8,2,6,7,
            //~ 1,0,0,4,6,2,3,8,5,
            //~ 2,0,6,0,7,5,1,0,9,
            //~ 8,6,9,7,0,1,4,0,2,
            //~ 0,0,2,0,3,6,7,0,1,
            //~ 0,3,1,0,4,9,8,5,6
        //~ };

        // --- MED ---
        //~ int grid[] = {
            //~ 0,8,3,2,9,0,4,5,7,
            //~ 2,9,0,0,0,0,1,0,8,
            //~ 0,0,0,1,0,8,0,6,9,
            //~ 3,7,0,8,2,4,5,0,6,
            //~ 0,0,0,6,5,9,3,0,0,
            //~ 0,6,0,3,7,0,0,2,0,
            //~ 6,0,0,0,0,0,9,0,0,
            //~ 0,4,0,0,0,2,0,8,0,
            //~ 9,0,8,0,6,0,7,1,0
        //~ };

        //~ int n = 3;


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

        int** possibles = (int**) malloc(sizeof(int*)*n*n*n*n);
        for ( i = 0 ; i < n*n ; i++ )
            for ( j = 0 ; j < n*n ; j++ )
                possibles[n*n*i+j] = (int*) malloc(sizeof(int)*n*n);

        //~ printf("Number of possible values: %d\n", get_possible_values(0,0,n,grid,res));
        display(grid, n);
        printf("------\n");

        backtrack(grid, possibles, n);

        display(grid, n);
        printf("------\n");


        for ( i = 0 ; i < n*n ; i++ )
            for ( j = 0 ; j < n*n ; j++ )
                free(possibles[n*n*i+j]);
        free(possibles);
        free(grid);
    }
}





