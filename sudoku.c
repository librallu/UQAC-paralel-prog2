#include <stdio.h>
#include <stdlib.h>

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

// display the grid
int display(int* grid, int n){
	int i,j;
	for ( i = 0 ; i < n*n ; i ++ ) {
		for ( j = 0 ; j < n*n ; j++ )
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



// simplify problem in a simple go
// returns the number of values updated
// should be called multiple times until it returns 0.
int simplify(int* grid, int n, int** possibles) {
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

int choose_backtracking(int* grid, int** possibles, int n, int* i, int* j) {
	int a,b, tmp, mini = n*n;
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
			if ( ++tab[grid[(a*n+i/n)*n*n+(b*n+i%n)]] == 2 ) return 0;
		}
	}
	
	free(tab);
	return 1;
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
	grid = malloc(sizeof(int)*n*n*n*n);
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
