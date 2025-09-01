#include "datatype.h"
#define PERCENT_SUDOKU 1
#define SUDOKU 2
typedef struct clause_list{
    Clause *clause;
    int count;
    int capacity;
}clause_list;

int add_clause_to_list(clause_list *list, Clause *clause);//����Ӿ䵽�Ӿ��б���
int init_clause_list(clause_list *list, int capacity);//��ʼ���Ӿ��б�
void free_clause_list(clause_list *list);//�ͷ��Ӿ��б�

int convert_sudoku_to_formula(int sudoku[9][9], Formula *formula, int type);//����������ת��Ϊformula
int convert_sudoku_to_clause_list(clause_list *list, int sudoku[9][9],int type);//����������ת��Ϊ�Ӿ��б�
int convert_clause_list_to_formula(clause_list *list, Formula *formula);//���Ӿ��б�ת��Ϊformula
int generate_sudoku_cnf(int sudoku[9][9],int type,char* output_filename);//����������cnf
int read_sudoku_from_file(char* filename,int sudoku[9][9]);//���ļ��ж�ȡ����

int add_cell_constraint_to_list(clause_list *list);//��ӵ�Ԫ�����Ƶ��Ӿ��б���
int add_row_constraint_to_list(clause_list *list);//��������Ƶ��Ӿ��б���
int add_col_constraint_to_list(clause_list *list);//��������Ƶ��Ӿ��б���
int add_block_constraint_to_list(clause_list *list,int row_start,int col_start);//��ӿ����Ƶ��Ӿ��б���
int add_diagonal_constraint_to_list(clause_list *list);//��ӶԽ������Ƶ��Ӿ��б���
int add_number_constraint_to_list(clause_list *list, int sudoku[9][9]);//����������Ѵ��ڵ��������Ƶ��Ӿ��б���
int generate_sudoku(int sudoku[9][9],int type);//����һ������
int print_sudoku(int sudoku[9][9]);//��ӡ����
int to_natural(int row, int col, int val);//�������е�λ��ת��Ϊ��Ȼ��
int to_sudoku(int natural,int *row,int *col,int *val);//����Ȼ��ת��Ϊ�����е�λ�ú�ֵ
int convert_solution_to_sudoku(int* solution,int sudoku[9][9]);//��cnf��ת��Ϊ����