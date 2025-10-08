#ifndef X_SUDOKU_H
#define X_SUDOKU_H
#include "datatype.h"
#define PERCENT_SUDOKU 1
#define SUDOKU 2

int generate_sudoku_cnf(int sudoku[9][9], int type, char *output_filename);                               // ����������cnf
int read_sudoku_from_file(char *filename, int sudoku[9][9]);                                              // ���ļ��ж�ȡ����
int generate_sudoku(int sudoku[][9], int type);                                                           // ����һ������
int print_sudoku(int sudoku[9][9]);                                                                       // ��ӡ����
int convert_solution_to_sudoku(int *solution, int sudoku[9][9]);                                          // ��cnf�Ľ�ת��Ϊ����
int solve_sudoku(int sudoku[9][9], int branch_select_strategy, int type, double *time, int *select_time); // �������
int output_sudoku_to_file(char *filename, int sudoku[9][9]);                                              // ������������ļ�
 int shuffle_array(int *arr, int n);
#endif
