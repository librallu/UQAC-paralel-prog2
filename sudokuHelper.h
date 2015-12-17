#ifndef __SUDOKUHELPER__
#define __SUDOKUHELPER__

#include <stdio.h>
#include <stdlib.h>

int display(int* grid, int n);
void simplify_all(int* grid, int n, int** possibles);

/**
 * 
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
 * returns 1 if grid is correct, 0 elsewhere
 */
int sudoku_verification(int* grid, int n); // pas encore testée

#endif
