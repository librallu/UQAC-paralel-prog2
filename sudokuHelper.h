#ifndef __SUDOKUHELPER__
#define __SUDOKUHELPER__

#include <stdio.h>
#include <stdlib.h>
//~ #include <time.h>

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
int sudoku_verification(int* grid, int n); // pas encore utilisée

#endif
