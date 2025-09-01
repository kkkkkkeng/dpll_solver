#ifndef X_SUDOKU_H
#define X_SUDOKU_H
#include "datatype.h"
#define PERCENT_SUDOKU 1
#define SUDOKU 2

// int convert_sudoku_to_formula(int sudoku[9][9], Formula *formula, int type);//将数独限制转换为formula
// int convert_sudoku_to_clause_list(clause_list *list, int sudoku[9][9],int type);//将数独限制转换为子句列表
// int convert_clause_list_to_formula(clause_list *list, Formula *formula);//将子句列表转换为formula
int generate_sudoku_cnf(int sudoku[9][9], int type, char *output_filename); // 生成数独的cnf
int read_sudoku_from_file(char *filename, int sudoku[9][9]);                // 从文件中读取数独
int generate_sudoku(int sudoku[9][9], int type);                            // 生成一个数独
int print_sudoku(int sudoku[9][9]);                                         // 打印数独
int convert_solution_to_sudoku(int *solution, int sudoku[9][9]);            // 将cnf的解转化为数独

#endif
