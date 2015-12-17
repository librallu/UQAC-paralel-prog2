#ifndef __SUDOKU_H__
#define __SUDOKU_H__

#define QUEUE_LEN 100

int to_continue(int* grid, int** possibles, int n);

void copy_grid(int* dest, int* start, int n);

int backtrack(int* grid, int** possibles, int n);


#endif //SUDOKU_H

