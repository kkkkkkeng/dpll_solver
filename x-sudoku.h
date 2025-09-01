#include "datatype.h"
#define PERCENT_SUDOKU 1
#define SUDOKU 2
typedef struct clause_list{
    Clause *clause;
    int count;
    int capacity;
}clause_list;

int add_clause_to_list(clause_list *list, Clause *clause);//添加子句到子句列表中
int init_clause_list(clause_list *list, int capacity);//初始化子句列表
void free_clause_list(clause_list *list);//释放子句列表

int convert_sudoku_to_formula(int sudoku[9][9], Formula *formula, int type);//将数独限制转换为formula
int convert_sudoku_to_clause_list(clause_list *list, int sudoku[9][9],int type);//将数独限制转换为子句列表
int convert_clause_list_to_formula(clause_list *list, Formula *formula);//将子句列表转换为formula
int generate_sudoku_cnf(int sudoku[9][9],int type,char* output_filename);//生成数独的cnf
int read_sudoku_from_file(char* filename,int sudoku[9][9]);//从文件中读取数独

int add_cell_constraint_to_list(clause_list *list);//添加单元格限制到子句列表中
int add_row_constraint_to_list(clause_list *list);//添加行限制到子句列表中
int add_col_constraint_to_list(clause_list *list);//添加列限制到子句列表中
int add_block_constraint_to_list(clause_list *list,int row_start,int col_start);//添加块限制到子句列表中
int add_diagonal_constraint_to_list(clause_list *list);//添加对角线限制到子句列表中
int add_number_constraint_to_list(clause_list *list, int sudoku[9][9]);//添加棋盘中已存在的数字限制到子句列表中
int generate_sudoku(int sudoku[9][9],int type);//生成一个数独
int print_sudoku(int sudoku[9][9]);//打印数独
int to_natural(int row, int col, int val);//将数独中的位置转换为自然数
int to_sudoku(int natural,int *row,int *col,int *val);//将自然数转换为数独中的位置和值
int convert_solution_to_sudoku(int* solution,int sudoku[9][9]);//将cnf解转化为数独