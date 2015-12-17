#ifndef __SUDOKU_H__
#define __SUDOKU_H__

int to_continue(int* grid, int** possibles, int n);

void copy_grid(int* dest, int* start, int n);

int backtrack(int* grid, int** possibles, int n);


#endif //SUDOKU_H

