#include "sudokuHelper.h"

void copy_grid(int* dest, int* start, int n) {
	int i,j;
	for ( i = 0 ; i < n*n*n*n ; i++ ) dest[i] = start[i];
}

/**
 * get possible values for a given case in grid.
 * res needs to be initialized with size n*n+1
 * returns the number of possible values
 */
int get_possible_values(int i, int j, int n, int* grid, int* res){
	int nb = n*n;
	int k,l;
	if ( grid[i*n*n+j] != 0 ) { // If the value is already fixed
		fprintf(stderr, "get_possible_values: (%d,%d) have value %d\n", i,j, grid[i*n*n+j]);
	}
	for ( k = 1 ; k <= n*n ; k++ ) res[k] = 1;
	
	// line check
	for ( k = 0 ; k < n*n ; k++ )
		if (grid[i*n*n+k] != 0 && res[grid[i*n*n+k]]) {
			nb--;
			res[grid[i*n*n+k]] = 0;
		}
	
	// column check
	for ( k = 0 ; k < n*n ; k++ )
		if (grid[k*n*n+j] != 0 && res[grid[k*n*n+j]]) {
			nb--;
			res[grid[k*n*n+j]] = 0;
		}
	
	// subsquare check
	int start_i, start_j;
	for ( k = 0 ; k < n ; k++ )
		for ( l = 0 ; l < n ; l++ ) {
			start_i = i-i%n;
			start_j = j-j%n;
			if (grid[(start_i+k)*n*n+(start_j+l)] != 0 && res[grid[(start_i+k)*n*n+(start_j+l)]]) {
				nb--;
				res[grid[(start_i+k)*n*n+(start_j+l)]] = 0;
			}
		}
	//~ if ( nb == 1 ){
		//~ for ( k = 1 ; k <= n*n ; k++ )
			//~ if ( res[k] ) printf("%d ", k);
		//~ printf("\n");
	//~ }
	return nb;
}




/**
 * display the grid for n*n
 */
int display(int* grid, int n){
	int i,j;
	for ( i = 0 ; i < n*n ; i ++ ) {
		for ( j = 0 ; j < n*n ; j++ )
			if ( grid[i*n*n+j] == 0 )
				printf("  _ ");
			else
				printf("%3d ", grid[i*n*n+j]);
		printf("\n");
	}
}



// Instance simplification for line i
int line_single_possibility(int* grid, int** possibles, int n, int i) {
	int nb_modified = 0;
	// list existing values in the set
	int *neighbourhood_possible = (int*) malloc(sizeof(int)*n*n+1);
	int j,k;
	for ( k = 1 ; k <= n*n ; k++ ) neighbourhood_possible[k] = 0;
	
	for ( j = 0 ; j < n*n ; j++ )
		for ( k = 1 ; k <= n*n ; k++ )  
			neighbourhood_possible[k] += possibles[i*n*n+j][k];
	
	// Assign the value if possible
	for ( j = 0 ; j < n*n ; j++ )
		if ( !grid[i*n*n+j] )
			for ( k = 1 ; k <= n*n ; k++ )
				if ( possibles[i*n*n+j][k] && neighbourhood_possible[k] == 1 ) {
					grid[i*n*n+j] = k;
					nb_modified++;
				}
	free(neighbourhood_possible);
	return nb_modified;
}

// Instance simplification for column j
int column_single_possibility(int* grid, int** possibles, int n, int j) {
	int nb_modified = 0;
	// list existing values in the set
	int i,k;
	int *neighbourhood_possible = (int*) malloc(sizeof(int)*n*n+1);
	for ( k = 1 ; k <= n*n ; k++ ) neighbourhood_possible[k] = 0;
	
	for ( i = 0 ; i < n*n ; i++ )
		for ( k = 1 ; k <= n*n ; k++ )
			neighbourhood_possible[k] += possibles[i*n*n+j][k];
	// Assign the value if possible
	for ( i = 0 ; i < n*n ; i++ )
		if ( !grid[i*n*n+j] )
			for ( k = 1 ; k <= n*n ; k++ )
				if ( possibles[i*n*n+j][k] && neighbourhood_possible[k] == 1 ) {
					grid[i*n*n+j] = k;
					nb_modified++;
				}
	free(neighbourhood_possible);
	return nb_modified;
}

// Instance simplification for sub cube (a,b)
int cube_single_possibility(int* grid, int** possibles, int n, int a, int b) {
	int nb_modified = 0;
	// list existing values in the set
	int *neighbourhood_possible = (int*) malloc(sizeof(int)*n*n+1);
	int i,j,k;
	for ( k = 1 ; k <= n*n ; k++ ) neighbourhood_possible[k] = 0;
	for ( i = 0 ; i < n ; i++ )
		for ( j = 0 ; j < n ; j++ )
			for ( k = 1 ; k <= n*n ; k++ )  
				neighbourhood_possible[k] += possibles[(a*n+i)*n*n+b*n+j][k];
	
	// Assign the value if possible
	for ( i = 0 ; i < n ; i++ )
		for ( j = 0 ; j < n ; j++ )
			if ( !grid[(a*n+i)*n*n+b*n+j] )
				for ( k = 1 ; k <= n*n ; k++ )
					if ( possibles[(a*n+i)*n*n+b*n+j][k] && neighbourhood_possible[k] == 1 ) {
						grid[(a*n+i)*n*n+b*n+j] = k;
						nb_modified++;
					}
	free(neighbourhood_possible);
	return nb_modified;
}



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



int simplify_hill(int* grid, int n, int** possibles) {
	int i,j,k, changed = 0;
	// check if there is a single value for a single position
	for ( i = 0 ; i < n*n ; i++ )
		for ( j = 0 ; j < n*n ; j++ )
			if ( grid[i*n*n+j] == 0 )
				if ( get_possible_values(i,j,n,grid,possibles[i*n*n+j]) == 1 ) {
					for ( k = 1 ; k <= n*n ; k++ )
						if ( possibles[i*n*n+j][k] ) {
							grid[i*n*n+j] = k;
							changed++;
							break;
						}
				}
	
	// Apply heuristics of second kind
	for ( i = 0 ; i < n*n ; i++ ) {
		changed += line_single_possibility(grid, possibles, n, i);
		changed += column_single_possibility(grid, possibles, n, i);
		changed += cube_single_possibility(grid, possibles, n, i/n, i%n);
	}
	
	return changed;
}

/**
 * use a hill climbing heuristic for sudoku.
 * May return the sudoku completed very fast, but
 * can also non modify the grid.
 * Return 1 if the grid is complete, 0 elsewhere
 */
//~ int hillClimbing(int* grid, int n, int** possibles){
	//~ int a,b,k=1,cont=1, cont2 ,i,j, nbPossibles;
	
	//~ cont = to_continue(grid, possibles, n);
	//~ if ( cont ) return 0; // problem already solved
	
	//~ // create a backup for the grid
	//~ int* grid_backup = (int*) malloc(sizeof(int)*n*n*n*n);
	//~ copy_grid(grid_backup, grid, n); // grid_backup <- grid
	
	//~ printf("in\n");
	//~ while (!cont) {
		//~ // affect a value
		//~ nbPossibles = choose_backtracking(grid, possibles, n, &a, &b);
		//~ cont2 = 1;
		//~ while(cont2) {
			//~ k = (k+1)%(n*n+1);
			//~ printf("%d\t%d\t%d\n",k, nbPossibles, rand()%9);
			//~ if ( k > 0 && possibles[a*n*n+b][k] && rand()%9 < nbPossibles ) {
				//~ printf("(%d,%d) %d\n",a,b,k);
				//~ grid[a*n*n+b] = k;
				//~ cont2 = 0;
			//~ }
		//~ }
		
		//~ // simplify grid
		//~ while (simplify_hill(grid, n, possibles));
		//~ simplify_all(grid, n, possibles);
		//~ display(grid,n);
		
		//~ // check condition for next iteration
		//~ cont = cont=to_continue(grid, possibles, n);
	
	//~ }
	//~ printf("out\n");
		
	//~ if ( cont != -1 ) { // no solution found
		//~ // restore original problem
		//~ copy_grid(grid, grid_backup, n);
		//~ free(grid_backup);
		//~ // restore possibles grid
		//~ for ( i = 0 ; i < n*n ; i++ )
			//~ for ( j = 0 ; j < n*n ; j++ )
				//~ if ( grid[i*n*n+j] == 0 )
					//~ get_possible_values(i,j,n,grid,possibles[i*n*n+j]);
		//~ return 0;
	//~ } else { // solution found
		//~ printf("Yeah\n");
		//~ free(grid_backup);
		//~ return 1;
	//~ }
//~ }


// simplify problem in a simple go
// returns the number of values updated
// should be called multiple times until it returns 0.
int simplify(int* grid, int n, int** possibles) {
	simplify_hill(grid, n, possibles);
	//~ hillClimbing(grid, n, possibles);
}

// takes a grid and a possible values table to be completed and simplify it as
// possible.
void simplify_all(int* grid, int n, int** possibles) {
	while (simplify(grid, n, possibles));
}

int compute_nb_possibles(int* v, int n) {
	int res = 0, i;
	for ( i = 1 ; i <= n*n ; i++ ) res+=v[i];
	return res;
}

/**
 * choose the best position for backtracking by choosing 
 * the one who has the less possibilities.
 */
int choose_backtracking(int* grid, int** possibles, int n, int* i, int* j) {
	int a,b, tmp, mini = n*n+1;
	for ( a = 0 ; a < n*n ; a++ )
		for ( b = 0 ; b < n*n ; b++ )
			if ( !grid[a*n*n+b] ) {
				tmp = compute_nb_possibles(possibles[a*n*n+b], n);
				if ( tmp < mini ) {
					mini = tmp;
					*i = a;
					*j = b;
				}
				
			}
	return mini;
}


// return 1 if sudoku is correct, 0 elsewhere
// TODO test de cette fonction
// peut être utile si le besoin s'en fait sentir
int sudoku_verification(int* grid, int n) {
	int i, j, k, a, b;
	int* tab = malloc(sizeof(int)*n*n+1);
	
	// column verification
	for ( i = 0 ; i < n*n ; i++ ) {
		for ( j = 1 ; j <= n*n ; j++ ) tab[j] = 0;
		for ( j = 0 ; j < n*n ; j++ ) {
			if ( ++tab[grid[i*n*n+j]] == 2) return 0;
		}
	}

	// line verification
	for ( j = 0 ; j < n*n ; j++ ) {
		for ( i = 1 ; i <= n*n ; i++ ) tab[i] = 0;
		for ( i = 0 ; i < n*n ; i++ ) {
			if ( ++tab[grid[i*n*n+j]] == 2) return 0;
		}
	}

	// block verification
	for ( a = 0 ; a < n ; a++ ) {
		for ( b = 0 ; b < n ; b++ ) {
			for ( i = 1 ; i <= n*n ; i++ ) tab[i] = 0;
			if ( ++tab[grid[(a*n+i/n)*n+(b*n+i%n)]] == 2 ) return 0;
		}
	}
	
	free(tab);
	return 1;
}
