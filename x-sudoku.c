#include "x-sudoku.h"
#include "datatype.h"
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
static int fill_sudoku(int sudoku[9][9], int type, int x, int y);                                     // 随机生成一个满足要求的数独

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
    for (int i = 0; i < x; i++)
    {
        if (sudoku[i][y] == val)
            return -1;
    }
    return 1;
}

static int check_row(int sudoku[9][9], int x, int y, int val)
{
    for (int i = 0; i < y; i++)
    {
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
    for (int i = 0; i < x; i++)
    {
        if (sudoku[i][8 - i] == val)
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
                return 1;
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
static int fill_sudoku(int sudoku[9][9], int type, int x, int y)
{
    int arr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
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
                    if (fill_sudoku(sudoku, type, x + 1, 0) == 1)
                        return 1;
                }
            }
            else
            {
                if (fill_sudoku(sudoku, type, x, y + 1) == 1)
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
            filename[0] = '\0';
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