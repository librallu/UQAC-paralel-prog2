#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <mpi.h>

// ============= sudoku helper ===============//
// This part contains some heuristics for sudoku solver

/**
 * copy sudoku grid from start to dest
 */
void copy_grid(int* dest, int* start, int n);

/**
 * display a sudoku grid
 */
int display(int* grid, int n);

/**
 * Simplify the grid using heuristics
 */
void simplify_all(int* grid, int n, int** possibles);

/**
 * get all the possible values for the grid with positions i,j and
 * fill *res
 * Return the number of possible values
 */
int get_possible_values(int i, int j, int n, int* grid, int* res);

/**
 * return the number of possibles for a possibles vector (possibles[i*n*n+j])
 */
int compute_nb_possibles(int* v, int n);

/**
 * puts in (i,j) the best case for backtracking. It returns the number
 * of possibilities for the (i,j) position.
 */
int choose_backtracking(int* grid, int** possibles, int n, int* i, int* j);


/**
 * return 1 if error
 * return -1 if terminated
 * else return 0
 */
int to_continue(int* grid, int** possibles, int n);

/**
 * returns 1 if grid is correct, 0 elsewhere
 */
int sudoku_verification(int* grid, int n);


// implementation of above functions


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



// simplify problem in a simple go
// returns the number of values updated
// should be called multiple times until it returns 0.
int simplify(int* grid, int n, int** possibles) {
	simplify_hill(grid, n, possibles);
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


// ===========================================


// BFS definition
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



int main(int argc, char** argv){
	
	// initialisation de MPI
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int *grid;
	int n,i,j;
	FILE* f;
	
	// For master, read the problem
	if ( rank == 0 ) { // if master thread
		
		if (argc < 2) {
			printf("usage: %s filename\n", argv[0]);
			return 1;
		}
		f = fopen(argv[1], "r");
		fscanf(f, "%d", &n);
		grid = malloc(sizeof(int)*n*n*n*n);
		for ( i = 0 ; i < n*n*n*n ; i++ ) fscanf(f, "%d", &(grid[i]));
		fclose(f);
		
		
	}
	
	//broadcast size of grid
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	int nmax = 2*n*n;
	int** grids;
	int nbGrids, start, res, currentNode = 0;
	
	if ( rank == 0 ){ // if master thread
		// generate matrix list
		grids = (int**) malloc(sizeof(int*)*nmax);
		res = bfs(grid, possibles, n, nmax, grids, &nbGrids, &start);
		printf("res:%d\n",res);
		if ( res == 1 ) {
			display(grid, n);
			MPI_Abort(MPI_Comm comm, 0);
		} else if ( res == -1 ) {
			printf("No solution\n");
			MPI_Abort(MPI_Comm comm, 0);
		} else {
			printf("nb noeuds :%d starting at %d\n",nbGrids, start);
			for ( i = 0 ; i < nbGrids ; i++ ) {
				//~ printf(" --- %d --- \n", (start+i)%nmax);
				//~ display(grids[(start+i)%nmax],n);
				
				// send grids to desired nodes
				if ( currentNode != rank ) {
					printf("sending to %d grid %d\n", currentNode, (start+i)%nmax);
				}
				currentNode = (currentNode+1)%size;
				
			}
		}
	}
	
	
}
