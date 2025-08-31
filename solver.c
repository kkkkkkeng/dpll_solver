#include "solver.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG
int isUnitClause(Clause *clause, int *literal_status)
{
    int literal_count = clause->literal_num;
    for (int i = 0; i < clause->literal_num; i++)
    {
        int variable = abs(clause->literal_array[i]);
        if (literal_status[variable])
        {
            if (literal_status[variable] * clause->literal_array[i] < 0)
            {
                literal_count--;
            }
            else
            {
                return -1;
            }
        }
    }
    if (literal_count == 1)
    {
        return 1;
    }
    return -1;
}
int init_status(Formula *formula, int *literal_status, int *clause_status)
{
    for (int i = 0; i < formula->clause_num; i++)
    {
        clause_status[i] = 0;
    }
    for (int i = 0; i <= formula->variable_num; i++)
    {
        literal_status[i] = 0;
    }
}
int update_clause_status(Formula *formula, int *literal_status, int *clause_status)
{
    int clause_count = formula->clause_num;
    for (int i = 0; i < clause_count; i++)
    {
        if (clause_status[i] == 1)
        {
            continue;
        }
        if (check_clause(&(formula->clause_array[i]), literal_status) == 1)
        {
            clause_status[i] = 1;
        }
    }
}
int record_UnitClause(Formula *formula, int *literal_status, int *clause_status)
{
    int isnewassigned = 1;
    while (isnewassigned)
    {
        isnewassigned = 0;
        for (int i = 0; i < formula->clause_num; i++)
        {
            if (clause_status[i] == 1)
            {
                continue;
            }
            if (isUnitClause(&(formula->clause_array[i]), literal_status) == 1)
            {
                clause_status[i] = 1;
                int index = 0;
                int variable = 0;
                int iscontradict = 1;
                for (int j = 0; j < formula->clause_array[i].literal_num; j++)
                {
                    variable = abs(formula->clause_array[i].literal_array[j]);
                    if (literal_status[variable] == 0)
                    {
                        index = j;
                        iscontradict = 0;
                        break;
                    }
                }
                if (iscontradict == 1)
                {
                    return -1;
                }
                literal_status[variable] = formula->clause_array[i].literal_array[index] > 0 ? 1 : -1;
                isnewassigned = 1;
            }
        }
    }
    return 1;
}
int check_clause(Clause *clause, int *literal_status)
{
    int literal_count = clause->literal_num;
    for (int i = 0; i < clause->literal_num; i++)
    {
        int variable = abs(clause->literal_array[i]);
        if (literal_status[variable])
        {
            if (literal_status[variable] * clause->literal_array[i] > 0)
                return 1;
            else
            {
                literal_count--;
            }
        }
    }
    if (literal_count == 0)
    {
        return -1;
    }
    return 0;
}

int select_branch_variable(Formula *formula, int *literal_status)
{
    for (int i = 1; i <= formula->variable_num; i++)
    {
        if (!literal_status[i])
        {
            return i;
        }
    }
}
int check_formula(Formula *formula, int *literal_status, int *clause_status)
{
    int clause_count = formula->clause_num;
    for (int i = 0; i < formula->clause_num; i++)
    {
#ifdef DEBUG
        if (i == 34)
        {
            for (int j = 1; j <= formula->variable_num; j++)
            {
                printf("literal_status[%d]: %d\n", j, literal_status[j]);
            }
            printf("\n");
        }
#endif
        if (clause_status[i] == 1)
        {
            clause_count--;
            continue;
        }
        else
        {
            int res = check_clause(&(formula->clause_array[i]), literal_status);
            if (res == -1)
            {
                return -1;
            }
            else if (res == 1)
            {
                clause_count--;
            }
        }
    }
    if (clause_count == 0)
    {
        return 1;
    }
    return 0;
}
int dpll_recursive(Formula *formula, int *literal_status, int *clause_status)
{
    int res = record_UnitClause(formula, literal_status, clause_status);
    if (res == -1)
    {
        return -1;
    }
    res = check_formula(formula, literal_status, clause_status);
    if (res == -1)
    {
        return -1;
    }
    else if (res == 1)
    {
        return 1;
    }
    else
    {
        int next_variable = select_branch_variable(formula, literal_status);
        literal_status[next_variable] = 1;
#ifdef DEBUG
        for (int i = 1; i <= formula->variable_num; i++)
        {
            printf("literal_status[%d]: %d\n", i, literal_status[i]);
        }
#endif
        int res = dpll_recursive(formula, literal_status, clause_status);
        if (res == 1)
        {
            return 1;
        }
        else if (res == -1)
        {
            literal_status[next_variable] = -1;
            res = dpll_recursive(formula, literal_status, clause_status);
            if (res == 1)
            {
                return 1;
            }
            else
            {
                literal_status[next_variable] = 0;
                return -1;
            }
        }
    }
}
int dpll_solve(Formula *formula, int *solution)
{
    int *literal_status = (int *)malloc(sizeof(int) * (formula->variable_num + 1)); // 存储每个变量的bool值 1true -1false 0未定 index 1~n+1
    int *clause_status = (int *)malloc(sizeof(int) * formula->clause_num);          // 判断clause是否为单变元子句
    init_status(formula, literal_status, clause_status);
    int res = dpll_recursive(formula, literal_status, clause_status);
    if (res == 1)
    {
        for (int i = 1; i <= formula->variable_num; i++)
        {
            solution[i] = literal_status[i];
        }
    }
    free(literal_status);
    free(clause_status);
    return res;
}

int print_solution(Formula *formula, int *solution)
{
    for (int i = 1; i <= formula->variable_num; i++)
    {
        if (solution[i] == 1)
        {
            printf("%d ", i);
        }
        else
        {
            printf("%d ", -i);
        }
    }
    printf("\n");
}