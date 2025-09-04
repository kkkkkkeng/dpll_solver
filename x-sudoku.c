#include "x-sudoku.h"
#include "datatype.h"
#include "solver.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct clause_list
{
    Clause *clause;
    int count;
    int capacity;
} clause_list;
static int add_clause_to_list(clause_list *list, Clause *clause);                         // 添加子句到子句列表中
static int init_clause_list(clause_list *list, int capacity);                             // 初始化子句列表
static void free_clause_list(clause_list *list);                                          // 释放子句列表
static int add_cell_constraint_to_list(clause_list *list);                                // 添加单元格限制到子句列表中
static int add_row_constraint_to_list(clause_list *list);                                 // 添加行限制到子句列表中
static int add_col_constraint_to_list(clause_list *list);                                 // 添加列限制到子句列表中
static int add_block_constraint_to_list(clause_list *list, int row_start, int col_start); // 添加块限制到子句列表中
static int add_diagonal_constraint_to_list(clause_list *list);                            // 添加对角线限制到子句列表中
static int add_number_constraint_to_list(clause_list *list, int sudoku[9][9]);            // 添加棋盘中已存在的数字限制到子句列表中
static int to_natural(int row, int col, int val);                                         // 将数独中的位置转换为自然数
static int to_sudoku(int natural, int *row, int *col, int *val);                          // 将自然数转换为数独中的位置和值

static int shuffle_array(int *arr, int n);                                                     // 打乱数组
static int check_col(int sudoku[9][9], int x, int y, int val);                                 // 检查列是否有冲突
static int check_row(int sudoku[9][9], int x, int y, int val);                                 // 检查行是否有冲突
static int check_block(int sudoku[9][9], int x, int y, int val, int row_start, int col_start); // 检查块是否有冲突
static int check_diagonal(int sudoku[9][9], int x, int y, int val);                            // 检查对角线是否有冲突
static int check_sudoku(int sudoku[9][9], int type, int x, int y);                             // 检查数独是否满足要求
static int fill_sudoku(int sudoku[9][9], int type, int x, int y, int *arr);                    // 随机生成一个满足要求的数独
static int find_empty_cell(int sudoku[9][9], int *row, int *col);                              // 寻找数独中的空格
static void count_sudoku_solutions(int sudoku[9][9], int type, int *count);
static void init_possibilities(int possibilities[9][9][10]);
static int find_empty_cell_smart(int sudoku[9][9], int *row, int *col, int possibilities[9][9][10]);
static int propagate_constraints(int sudoku[9][9], int possibilities[9][9][10], int type, int row, int col, int val);
static int propagate_constraints_on_row(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val);
static int propagate_constraints_on_col(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val);
static int propagate_constraints_on_diagonal(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val);
static int propagate_constraints_on_block(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val, int row_start, int col_start);
static int fill_sudoku_smart(int sudoku[9][9], int type, int *arr, int possibilities[9][9][10]);

static int find_empty_cell(int sudoku[9][9], int *row, int *col)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] == 0)
            {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }
    *row = -1;
    *col = -1;
    return -1;
}

static void count_sudoku_solutions(int sudoku[9][9], int type, int *count)
{
    int row = -1, col = -1;
    if (find_empty_cell(sudoku, &row, &col) == -1)
    {
        (*count)++;
        return;
    }
    for (int num = 1; num <= 9; num++)
    {
        sudoku[row][col] = num;
        if (check_sudoku(sudoku, type, row, col) == 1)
        {
            count_sudoku_solutions(sudoku, type, count);
            if (*count > 1)
            {
                sudoku[row][col] = 0;
                return;
            }
        }
        sudoku[row][col] = 0;
    }
    return;
}

static int x_sudoku_check(int sudoku[9][9]); // 检查数独是否满足%数独要求
static int convert_sudoku_to_formula(int sudoku[9][9], Formula *formula, int type)
{
    if (formula == NULL)
    {
        return -1;
    }
    clause_list list;
    init_clause_list(&list, 1024);
    add_cell_constraint_to_list(&list);
    add_row_constraint_to_list(&list);
    add_col_constraint_to_list(&list);
    for (int i = 1; i <= 9; i += 3)
    {
        for (int j = 1; j <= 9; j += 3)
        {
            add_block_constraint_to_list(&list, i, j);
        }
    }
    if (type == PERCENT_SUDOKU)
    {
        add_block_constraint_to_list(&list, 2, 2);
        add_block_constraint_to_list(&list, 6, 6);
        add_diagonal_constraint_to_list(&list);
    }
    add_number_constraint_to_list(&list, sudoku);
    formula->clause_num = list.count;
    formula->variable_num = 9 * 9 * 9;
    formula->clause_array = list.clause;
    list.clause = NULL;
    list.count = 0;
    list.capacity = 0;
    return 1;
}
static int x_sudoku_check(int sudoku[9][9])
{
    int record[10] = {0};
    for (int i = 0; i < 9; i++)
    {
        if (record[sudoku[i][8 - i]] == 1)
        {
            return -1;
        }
        else
        {
            record[sudoku[i][8 - i]] = 1;
        }
    }
    memset(record, 0, sizeof(record));
    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 3; j++)
        {
            if (record[sudoku[i][j]] == 1)
            {
                return -1;
            }
            else
            {
                record[sudoku[i][j]] = 1;
            }
        }
    }
    memset(record, 0, sizeof(record));
    for (int i = 5; i <= 7; i++)
    {
        for (int j = 5; j <= 7; j++)
        {
            if (record[sudoku[i][j]] == 1)
            {
                return -1;
            }
            else
            {
                record[sudoku[i][j]] = 1;
            }
        }
    }
    return 1;
}

static void init_possibilities(int possibilities[9][9][10])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            for (int k = 1; k <= 9; k++)
            {
                possibilities[i][j][k] = 1;
            }
            possibilities[i][j][0] = 9;
        }
    }
    return;
}
static int find_empty_cell_smart(int sudoku[9][9], int *row, int *col, int possibilities[9][9][10])
{
    int min_possibility = 10;
    int found = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] == 0)
            {
                found = 1;
                if (possibilities[i][j][0] < min_possibility)
                {
                    min_possibility = possibilities[i][j][0];
                    *row = i;
                    *col = j;
                }
            }
        }
    }
    if (found == 0)
    {
        *row = -1;
        *col = -1;
        return -1;
    }
    return 1;
}

static int propagate_constraints(int sudoku[9][9], int possibilities[9][9][10], int type, int row, int col, int val)
{
    if (propagate_constraints_on_row(sudoku, possibilities, row, col, val) == -1)
        return -1;
    if (propagate_constraints_on_col(sudoku, possibilities, row, col, val) == -1)
        return -1;
    if (propagate_constraints_on_block(sudoku, possibilities, row, col, val, row / 3 * 3, col / 3 * 3) == -1)
        return -1;
    if (type == PERCENT_SUDOKU)
    {
        if (propagate_constraints_on_diagonal(sudoku, possibilities, row, col, val) == -1)
            return -1;
        if (propagate_constraints_on_block(sudoku, possibilities, row, col, val, 1, 1) == -1)
            return -1;
        if (propagate_constraints_on_block(sudoku, possibilities, row, col, val, 5, 5) == -1)
            return -1;
    }
    return 1;
}

static int propagate_constraints_on_row(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val)
{
    for (int j = 0; j < 9; j++)
    {
        if (j == col)
            continue;
        if (sudoku[row][j] != 0)
            continue;
        if (possibilities[row][j][val] == 1)
        {
            possibilities[row][j][val] = 0;
            possibilities[row][j][0]--;
            if (possibilities[row][j][0] == 0)
            {
                return -1;
            }
        }
    }
}
static int propagate_constraints_on_col(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val)
{
    for (int i = 0; i < 9; i++)
    {
        if (i == row)
            continue;
        if (sudoku[i][col] != 0)
            continue;
        if (possibilities[i][col][val] == 1)
        {
            possibilities[i][col][val] = 0;
            possibilities[i][col][0]--;
            if (possibilities[i][col][0] == 0)
            {
                return -1;
            }
        }
    }
}
static int propagate_constraints_on_diagonal(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val)
{
    if (row != 8 - col)
        return 1;
    for (int i = 0; i < 9; i++)
    {
        if (i == row)
            continue;
        if (sudoku[i][8 - i] != 0)
            continue;
        if (possibilities[i][8 - i][val] == 1)
        {
            possibilities[i][8 - i][val] = 0;
            possibilities[i][8 - i][0]--;
            if (possibilities[i][8 - i][0] == 0)
            {
                return -1;
            }
        }
    }
}
static int propagate_constraints_on_block(int sudoku[9][9], int possibilities[9][9][10], int row, int col, int val, int row_start, int col_start)
{
    if (row < row_start || row > row_start + 2 || col < col_start || col > col_start + 2)
        return 1;
    for (int i = row_start; i <= row_start + 2; i++)
    {
        for (int j = col_start; j <= col_start + 2; j++)
        {
            if (i == row && j == col)
                continue;
            if (sudoku[i][j] != 0)
                continue;
            if (possibilities[i][j][val] == 1)
            {
                possibilities[i][j][val] = 0;
                possibilities[i][j][0]--;
                if (possibilities[i][j][0] == 0)
                {
                    return -1;
                }
            }
        }
    }
}
static int fill_sudoku_smart(int sudoku[9][9], int type, int *arr, int possibilities[9][9][10])
{
    int x = -1, y = -1;
    if (find_empty_cell_smart(sudoku, &x, &y, possibilities) == -1)
    {
        return 1;
    }
    shuffle_array(arr, 9);
    for (int i = 0; i < 9; i++)
    {
        int var = arr[i];
        if (possibilities[x][y][var] == 1)
        {
            sudoku[x][y] = var;
            int backup[9][9][10];
            memcpy(backup, possibilities, sizeof(backup));
            if (propagate_constraints(sudoku, possibilities, type, x, y, var) == -1)
            {
                memcpy(possibilities, backup, sizeof(backup));
                sudoku[x][y] = 0;
                continue;
            }
            if (fill_sudoku_smart(sudoku, type, arr, possibilities) == 1)
                return 1;
            memcpy(possibilities, backup, sizeof(backup));
            sudoku[x][y] = 0;
        }
    }
    return -1;
}

// static void count_sudoku_solutions(int sudoku[9][9],int type,int *count){
//     int row = -1, col = -1;
//     int possibilities[9][9][10];
//     init_possibilities(possibilities);
//     if(find_empty_cell_smart(sudoku, &row, &col,possibilities)==-1){
//         (*count)++;
//         return;
//     }
//     for(int num=1;num<=9;num++){
//         if(possibilities[row][col][num]==1){
//             sudoku[row][col]=num;
//             int backup[9][9][10];
//             memcpy(backup,possibilities,sizeof(backup));
//             if(propagate_constraints(sudoku,possibilities,type,row,col,num)==-1){
//                 memcpy(possibilities,backup,sizeof(backup));
//                 sudoku[row][col]=0;
//                 continue;
//             }
//             count_sudoku_solutions(sudoku,type,count);
//             if(*count>1){
//                 sudoku[row][col]=0;
//                 return;
//             }
//             memcpy(possibilities,backup,sizeof(backup));
//         }
//         sudoku[row][col]=0;
//     }
//     return;
// }

int solve_sudoku(int sudoku[9][9], int branch_select_stategy, int type, double *time, int *select_time)
{
    Formula formula;
    convert_sudoku_to_formula(sudoku, &formula, type);
    int *solution = (int *)malloc((formula.variable_num + 1) * sizeof(int));
    int res = dpll_solve(&formula, branch_select_stategy, solution, time, select_time);
    if (res == RES_SAT)
    {
        convert_solution_to_sudoku(solution, sudoku);
        free(solution);
        free_formula(&formula);
        return RES_SAT;
    }
    else
    {
        free(solution);
        free_formula(&formula);
        return res;
    }
}

int generate_sudoku(int board[][9], int type)
{
    int *possibilities = (int *)malloc(sizeof(int) * 9 * 9 * 10);
    init_possibilities((int (*)[9][10])possibilities);
    memset(board, 0, sizeof(int) * 9 * 9);
    int arr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    // fill_sudoku(board, type, 0, 0, arr);
    fill_sudoku_smart(board, type, arr, (int (*)[9][10])possibilities);
    // print_sudoku(board);
    int dig_sequence[81];
    for (int i = 0; i < 81; i++)
    {
        dig_sequence[i] = i;
    }
    shuffle_array(dig_sequence, 81);
    int i = 0;
    int solution_count = 0;
    int record_last = 0;
    do
    {
        solution_count = 0;
        record_last = board[dig_sequence[i] / 9][dig_sequence[i] % 9];
        board[dig_sequence[i] / 9][dig_sequence[i] % 9] = 0;
        count_sudoku_solutions(board, type, &solution_count);
        // print_sudoku(board);
        i++;
    } while (solution_count == 1 && i < 81);
    // Formula formula;
    // do
    // {
    //     solution_count = 0;
    //     record_last = board[dig_sequence[i] / 9][dig_sequence[i] % 9];
    //     board[dig_sequence[i] / 9][dig_sequence[i] % 9] = 0;
    //     convert_sudoku_to_formula(board, &formula, type);
    //     int res = is_solution_unique(&formula, OPTIMIZED1, &solution_count);
    //     free_formula(&formula);
    //     if (res == RES_TIME_OUT)
    //     {
    //         return RES_TIME_OUT;
    //     }
    //     i++;
    // } while (solution_count == 1 && i < 81);
    if (solution_count > 1)
    {
        board[dig_sequence[i - 1] / 9][dig_sequence[i - 1] % 9] = record_last;
    }
    else if (solution_count == 0)
    {
        return -1;
    }
    return 1;
}

static int shuffle_array(int *arr, int n)
{
    for (int i = n - 1; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    return 1;
}

static int check_col(int sudoku[9][9], int x, int y, int val)
{
    for (int i = 0; i < 9; i++)
    {
        if (x == i)
            continue;
        if (sudoku[i][y] == val)
            return -1;
    }
    return 1;
}

static int check_row(int sudoku[9][9], int x, int y, int val)
{
    for (int i = 0; i < 9; i++)
    {
        if (y == i)
            continue;
        if (sudoku[x][i] == val)
            return -1;
    }
    return 1;
}
static int check_diagonal(int sudoku[9][9], int x, int y, int val)
{
    if (x != 8 - y)
    {
        return 1;
    }
    for (int i = 0; i < 9; i++)
    {
        if (i == x)
            continue;
        if (sudoku[i][8 - i] == 0)
            continue;
        else if (sudoku[i][8 - i] == val)
            return -1;
    }
    return 1;
}
static int check_block(int sudoku[9][9], int x, int y, int val, int row_start, int col_start)
{
    if (x > row_start + 2 || y > col_start + 2 || x < row_start || y < col_start)
    {
        return 1;
    }
    for (int i = row_start; i <= row_start + 2; i++)
    {
        for (int j = col_start; j <= col_start + 2; j++)
        {
            if (i == x && j == y)
                continue;
            if (sudoku[i][j] == val)
                return -1;
        }
    }
    return 1;
}
static int check_sudoku(int sudoku[9][9], int type, int x, int y)
{
    if (check_row(sudoku, x, y, sudoku[x][y]) == -1)
        return -1;
    if (check_col(sudoku, x, y, sudoku[x][y]) == -1)
        return -1;
    if (check_block(sudoku, x, y, sudoku[x][y], (x / 3) * 3, (y / 3) * 3) == -1)
        return -1;
    if (type == PERCENT_SUDOKU)
    {
        if (check_block(sudoku, x, y, sudoku[x][y], 1, 1) == -1)
            return -1;
        if (check_block(sudoku, x, y, sudoku[x][y], 5, 5) == -1)
            return -1;
        if (check_diagonal(sudoku, x, y, sudoku[x][y]) == -1)
            return -1;
    }
    return 1;
}
static int fill_sudoku(int sudoku[9][9], int type, int x, int y, int *arr)
{
    shuffle_array(arr, 9);
    for (int i = 0; i < 9; i++)
    {
        sudoku[x][y] = arr[i];
        if (check_sudoku(sudoku, type, x, y) == 1)
        {
            if (y == 8)
            {
                if (x == 8)
                    return 1;
                else
                {
                    if (fill_sudoku(sudoku, type, x + 1, 0, arr) == 1)
                        return 1;
                }
            }
            else
            {
                if (fill_sudoku(sudoku, type, x, y + 1, arr) == 1)
                    return 1;
            }
        }
        else
        {
            sudoku[x][y] = 0;
        }
    }
    return -1;
}

static int add_clause_to_list(clause_list *list, Clause *clause)
{
    if (list->count == list->capacity)
    {
        list->capacity *= 2;
        list->clause = realloc(list->clause, sizeof(Clause) * list->capacity);
        if (list->clause == NULL)
        {
            return -1;
        }
    }
    list->clause[list->count] = *clause;
    list->count++;
    return 1;
}
static int init_clause_list(clause_list *list, int capacity)
{
    list->count = 0;
    list->capacity = capacity;
    list->clause = (Clause *)malloc(sizeof(Clause) * list->capacity);
    if (list->clause == NULL)
    {
        return -1;
    }
    return 1;
}
static void free_clause_list(clause_list *list)
{
    for (int i = 0; i < list->count; i++)
    {
        free(list->clause[i].literal_array);
    }
    free(list->clause);
    list->clause = NULL;
    list->count = 0;
    list->capacity = 0;
}
int generate_sudoku_cnf(int sudoku[9][9], int type, char *output_filename)
{
    clause_list list;
    init_clause_list(&list, 1024);
    add_cell_constraint_to_list(&list);
    add_row_constraint_to_list(&list);
    add_col_constraint_to_list(&list);
    add_number_constraint_to_list(&list, sudoku);
    for (int i = 1; i <= 9; i += 3)
    {
        for (int j = 1; j <= 9; j += 3)
        {
            add_block_constraint_to_list(&list, i, j);
        }
    }

    if (type == PERCENT_SUDOKU)
    {
        add_block_constraint_to_list(&list, 2, 2);
        add_block_constraint_to_list(&list, 6, 6);
        add_diagonal_constraint_to_list(&list);
    }

    FILE *fp = fopen(output_filename, "w");
    if (fp == NULL)
    {
        return -1;
    }
    fprintf(fp, "p cnf %d %d\n", 9 * 9 * 9, list.count);
    for (int i = 0; i < list.count; i++)
    {
        for (int j = 0; j < list.clause[i].literal_num; j++)
        {
            fprintf(fp, "%d ", list.clause[i].literal_array[j]);
        }
        fprintf(fp, "0\n");
    }
    fclose(fp);
    free_clause_list(&list);
    return 1;
}
static int add_cell_constraint_to_list(clause_list *list)
{
    for (int i = 1; i <= 9; i++)
    {
        for (int j = 1; j <= 9; j++)
        {
            Clause clause;
            clause.literal_num = 9;
            clause.literal_array = (int *)malloc(sizeof(int) * 9);
            for (int k = 1; k <= 9; k++)
            {
                clause.literal_array[k - 1] = to_natural(i, j, k);
            }
            add_clause_to_list(list, &clause);
            for (int m = 1; m <= 9; m++)
            {
                for (int n = m + 1; n <= 9; n++)
                {
                    Clause clause;
                    clause.literal_num = 2;
                    clause.literal_array = (int *)malloc(sizeof(int) * 2);
                    clause.literal_array[0] = -to_natural(i, j, m);
                    clause.literal_array[1] = -to_natural(i, j, n);
                    add_clause_to_list(list, &clause);
                }
            }
        }
    }
    return 1;
}
static int add_row_constraint_to_list(clause_list *list)
{
    for (int i = 1; i <= 9; i++)
    {
        for (int k = 1; k <= 9; k++)
        {
            Clause clause;
            clause.literal_num = 9;
            clause.literal_array = (int *)malloc(sizeof(int) * 9);
            for (int j = 1; j <= 9; j++)
            {
                clause.literal_array[j - 1] = to_natural(i, j, k);
            }
            add_clause_to_list(list, &clause);
        }
        for (int m = 1; m <= 9; m++)
        {
            for (int n = m + 1; n <= 9; n++)
            {
                for (int k = 1; k <= 9; k++)
                {
                    Clause clause;
                    clause.literal_num = 2;
                    clause.literal_array = (int *)malloc(sizeof(int) * 2);
                    clause.literal_array[0] = -to_natural(i, m, k);
                    clause.literal_array[1] = -to_natural(i, n, k);
                    add_clause_to_list(list, &clause);
                }
            }
        }
    }
    return 1;
}
static int add_col_constraint_to_list(clause_list *list)
{
    for (int i = 1; i <= 9; i++)
    {
        for (int k = 1; k <= 9; k++)
        {
            Clause clause;
            clause.literal_num = 9;
            clause.literal_array = (int *)malloc(sizeof(int) * 9);
            for (int j = 1; j <= 9; j++)
            {
                clause.literal_array[j - 1] = to_natural(j, i, k);
            }
            add_clause_to_list(list, &clause);
        }
        for (int m = 1; m <= 9; m++)
        {
            for (int n = m + 1; n <= 9; n++)
            {
                for (int k = 1; k <= 9; k++)
                {
                    Clause clause;
                    clause.literal_num = 2;
                    clause.literal_array = (int *)malloc(sizeof(int) * 2);
                    clause.literal_array[0] = -to_natural(m, i, k);
                    clause.literal_array[1] = -to_natural(n, i, k);
                    add_clause_to_list(list, &clause);
                }
            }
        }
    }
    return 1;
}
static int add_block_constraint_to_list(clause_list *list, int row_start, int col_start)
{
    for (int k = 1; k <= 9; k++)
    {
        Clause clause;
        clause.literal_num = 9;
        clause.literal_array = (int *)malloc(sizeof(int) * 9);
        int index = 0;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                clause.literal_array[index++] = to_natural(row_start + i, col_start + j, k);
            }
        }
        add_clause_to_list(list, &clause);
    }
    for (int i = 1; i <= 9; i++)
    {
        for (int j = i + 1; j <= 9; j++)
        {
            for (int k = 1; k <= 9; k++)
            {
                Clause clause;
                clause.literal_num = 2;
                clause.literal_array = (int *)malloc(sizeof(int) * 2);
                clause.literal_array[0] = -to_natural(row_start + (i - 1) / 3, col_start + (i - 1) % 3, k);
                clause.literal_array[1] = -to_natural(row_start + (j - 1) / 3, col_start + (j - 1) % 3, k);
                add_clause_to_list(list, &clause);
            }
        }
    }
}
static int add_diagonal_constraint_to_list(clause_list *list)
{

    for (int k = 1; k <= 9; k++)
    {
        Clause clause;
        clause.literal_num = 9;
        clause.literal_array = (int *)malloc(sizeof(int) * 9);
        for (int i = 1; i <= 9; i++)
        {
            clause.literal_array[i - 1] = to_natural(i, 10 - i, k);
        }
        add_clause_to_list(list, &clause);
    }
    for (int i = 1; i <= 9; i++)
    {
        for (int j = i + 1; j <= 9; j++)
        {
            for (int k = 1; k <= 9; k++)
            {
                Clause clause;
                clause.literal_num = 2;
                clause.literal_array = (int *)malloc(sizeof(int) * 2);
                clause.literal_array[0] = -to_natural(i, 10 - i, k);
                clause.literal_array[1] = -to_natural(j, 10 - j, k);
                add_clause_to_list(list, &clause);
            }
        }
    }
    return 1;
}
static int add_number_constraint_to_list(clause_list *list, int sudoku[9][9])
{
    for (int i = 1; i <= 9; i++)
    {
        for (int j = 1; j <= 9; j++)
        {
            if (sudoku[i - 1][j - 1] != 0)
            {
                Clause clause;
                clause.literal_num = 1;
                clause.literal_array = (int *)malloc(sizeof(int));
                clause.literal_array[0] = to_natural(i, j, sudoku[i - 1][j - 1]);
                add_clause_to_list(list, &clause);
            }
        }
    }
    return 1;
}
static int to_natural(int row, int col, int val)
{
    return (row - 1) * 9 * 9 + (col - 1) * 9 + val;
}
static int to_sudoku(int natural, int *row, int *col, int *val)
{
    if (natural < 0 || natural > 9 * 9 * 9)
    {
        return -1;
    }
    natural--;
    *val = natural % 9 + 1;
    *col = natural / 9 % 9 + 1;
    *row = natural / 9 / 9 + 1;
    return 1;
}
int convert_solution_to_sudoku(int *solution, int sudoku[9][9])
{
    for (int i = 1; i <= 9 * 9 * 9; i++)
    {
        if (solution[i] > 0)
        {
            int row, col, val;
            to_sudoku(i, &row, &col, &val);
            sudoku[row - 1][col - 1] = val;
        }
    }
    return 1;
}
int read_sudoku_from_file(char *filename, int sudoku[9][9])
{

    static char current_filename[100] = "";
    static FILE *fp = NULL;
    if (strcmp(filename, current_filename) != 0)
    {
        if (fp)
            fclose(fp);
        fp = fopen(filename, "r");
        if (!fp)
        {
            current_filename[0] = '\0';
            return -1;
        }
        strcpy(current_filename, filename);
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            char c;
            c = fgetc(fp);
            while (c != EOF && (c < '0' || c > '9'))
                c = fgetc(fp);
            if (c == EOF)
            {
                fclose(fp);
                current_filename[0] = '\0';
                return 0;
            }
            sudoku[i][j] = c - '0';
        }
    }
    return 1;
}
int print_sudoku(int sudoku[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
    return 1;
}