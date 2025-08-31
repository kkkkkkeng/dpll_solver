#include "solver.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG
int findUnitLiteral(Clause *clause, int *literal_status)
{
    int literal_count = 0;
    int unit_literal = 0;
    for (int i = 0; i < clause->literal_num; i++)
    {
        int variable = abs(clause->literal_array[i]);
        if (!literal_status[variable])
        {
            literal_count++;
            unit_literal = clause->literal_array[i];
        }
        else if (literal_status[variable] * clause->literal_array[i] > 0)
        {
            return 0;
        }
    }
    if (literal_count == 1)
    {
        return unit_literal;
    }
    return 0;
}
int init_status(Formula *formula, int *literal_status)
{
    for (int i = 0; i <= formula->variable_num; i++)
    {
        literal_status[i] = 0;
    }
}

int record_UnitClause(Formula *formula, int *literal_status, int *trail, int *trail_pointer)
{
    int isnewassigned = 1;
    while (isnewassigned)
    {
        isnewassigned = 0;
        for (int i = 0; i < formula->clause_num; i++)
        {
            int unit_literal = findUnitLiteral(&(formula->clause_array[i]), literal_status);
            if (unit_literal)
            {
                int variable = abs(unit_literal);
                assign_literal(literal_status, variable, unit_literal > 0 ? 1 : -1, trail, trail_pointer);
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
    int i = 0;
    for (i = 1; i <= formula->variable_num; i++)
    {
        if (!literal_status[i])
        {
            return i;
        }
    }
    return 0;
}
int check_formula(Formula *formula, int *literal_status)
{
    int clause_count = formula->clause_num;
    for (int i = 0; i < formula->clause_num; i++)
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
    if (clause_count == 0)
    {
        return 1;
    }
    return 0;
}
int dpll_recursive(Formula *formula, int *literal_status, int *trail, int *trail_pointer)
{
    int res = record_UnitClause(formula, literal_status, trail, trail_pointer);
    if (res == -1)
    {
        return -1;
    }
    res = check_formula(formula, literal_status);
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
        if (next_variable == 0)
        {
            return -1;
        }
        int level = *trail_pointer;
        assign_literal(literal_status, next_variable, 1, trail, trail_pointer);
        int res = dpll_recursive(formula, literal_status, trail, trail_pointer);
        if (res == 1)
        {
            return 1;
        }
        backtrack(literal_status, level, trail, trail_pointer);
        assign_literal(literal_status, next_variable, -1, trail, trail_pointer);
        res = dpll_recursive(formula, literal_status, trail, trail_pointer);
        if (res == 1)
        {
            return 1;
        }
        backtrack(literal_status, level, trail, trail_pointer);
        return -1;
    }
}
int dpll_solve(Formula *formula, int *solution)
{
    int *literal_status = (int *)malloc(sizeof(int) * (formula->variable_num + 1)); // 存储每个变量的bool值 1true -1false 0未定 index 1~n+1         // 存储每个clause是否为单变元子句
    int *trail = (int *)malloc(sizeof(int) * (formula->variable_num + 1));
    int trail_pointer = 0;
    init_status(formula, literal_status);
    int res = dpll_recursive(formula, literal_status, trail, &trail_pointer);
    if (res == 1)
    {
        for (int i = 1; i <= formula->variable_num; i++)
        {
            solution[i] = literal_status[i];
        }
    }
    free(literal_status);
    free(trail);
    return res;
}
void assign_literal(int *literal_status, int var, int value, int *trail, int *trail_pointer)
{
    literal_status[var] = value;
    trail[*trail_pointer] = var;
    (*trail_pointer)++;
}
void backtrack(int *literal_status, int level, int *trail, int *trail_pointer)
{
    while (*trail_pointer > level)
    {
        int var = trail[*trail_pointer - 1];
        literal_status[var] = 0;
        (*trail_pointer)--;
    }
}
int verify_solution(Formula *formula, int *solution)
{
    for (int i = 0; i < formula->clause_num; i++)
    {
        int is_clause_satisfied = 0;
        for (int j = 0; j < formula->clause_array[i].literal_num; j++)
        {
            int variable = abs(formula->clause_array[i].literal_array[j]);
            if (solution[variable] * formula->clause_array[i].literal_array[j] > 0)
            {
                is_clause_satisfied = 1;
                break;
            }
        }
        if (is_clause_satisfied == 0)
        {
            return -1;
        }
    }
    return 1;
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