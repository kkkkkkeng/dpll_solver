#ifndef X_SUDOKU_H
#define X_SUDOKU_H
#include "datatype.h"
#define PERCENT_SUDOKU 1
#define SUDOKU 2

// int convert_sudoku_to_formula(int sudoku[9][9], Formula *formula, int type);//����������ת��Ϊformula
// int convert_sudoku_to_clause_list(clause_list *list, int sudoku[9][9],int type);//����������ת��Ϊ�Ӿ��б�
// int convert_clause_list_to_formula(clause_list *list, Formula *formula);//���Ӿ��б�ת��Ϊformula
int generate_sudoku_cnf(int sudoku[9][9], int type, char *output_filename); // ����������cnf
int read_sudoku_from_file(char *filename, int sudoku[9][9]);                // ���ļ��ж�ȡ����
int generate_sudoku(int sudoku[9][9], int type);                            // ����һ������
int print_sudoku(int sudoku[9][9]);                                         // ��ӡ����
int convert_solution_to_sudoku(int *solution, int sudoku[9][9]);            // ��cnf�Ľ�ת��Ϊ����

#endif
