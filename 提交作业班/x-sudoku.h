#ifndef X_SUDOKU_H
#define X_SUDOKU_H
#include "datatype.h"
#define PERCENT_SUDOKU 1
#define SUDOKU 2

int generate_sudoku_cnf(int sudoku[9][9], int type, char *output_filename);                               // 生成数独的cnf
int read_sudoku_from_file(char *filename, int sudoku[9][9]);                                              // 从文件中读取数独
int generate_sudoku(int sudoku[][9], int type);                                                           // 生成一个数独
int print_sudoku(int sudoku[9][9]);                                                                       // 打印数独
int convert_solution_to_sudoku(int *solution, int sudoku[9][9]);                                          // 将cnf的解转化为数独
int solve_sudoku(int sudoku[9][9], int branch_select_strategy, int type, double *time, int *select_time); // 求解数独
int output_sudoku_to_file(char *filename, int sudoku[9][9]);                                              // 将数独输出到文件
 int shuffle_array(int *arr, int n);
#endif
