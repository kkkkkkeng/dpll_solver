#include "solver.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int isUnitClause(Clause *clause)
{
    int literal_count = clause->literal_num;
    for (int i = 0; i < clause->literal_num; i++)
    {
        int variable = abs(clause->literal_array[i]);
        if (literal_status[variable] && literal_status[variable] * clause->literal_array[i] < 0)
        {
            literal_count--;
        }
    }
    if (literal_count == 1)
    {
        return 1;
    }
    return -1;
}
int init_status(Formula *formula)
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
int record_UnitClause(Formula *formula)
{
    for (int i = 0; i < formula->clause_num; i++)
    {
        if (clause_status[i] == 1)
        {
            continue;
        }
        if (isUnitClause(&(formula->clause_array[i])) == 1)
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
        }
    }
    return 1;
}
int check_clause(Clause *clause)
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

int select_branch_variable(Formula *formula)
{
    for (int i = 0; i < formula->variable_num; i++)
    {
        if (!literal_status[i])
        {
            return i;
        }
    }
}
int check_formula(Formula *formula)
{
    int clause_count = formula->clause_num;
    for (int i = 0; i < formula->clause_num; i++)
    {
        if (clause_status[i] == 1)
        {
            clause_count--;
            continue;
        }
        else
        {
            int res = check_clause(&(formula->clause_array[i]));
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
int dpll_recursive(Formula *formula)
{
    int res = record_UnitClause(formula);
    if (res == -1)
    {
        return -1;
    }
    res = check_formula(formula);
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
        int next_variable = select_branch_variable(formula);
        literal_status[next_variable] = 1;
        int res = dpll_recursive(formula);
        if (res == -1)
        {
            literal_status[next_variable] = -1;
            return dpll_recursive(formula);
        }
        else
        {
            return res;
        }
    }
}
int dpll_solve(Formula *formula)
{
    literal_status = (int *)malloc(sizeof(int) * (formula->variable_num + 1));
    clause_status = (int *)malloc(sizeof(int) * formula->clause_num);
    init_status(formula);
    return dpll_recursive(formula);
}
int print_variable_status(Formula *formula)
{
    for (int i = 1; i <= formula->variable_num; i++)
    {
        printf("%d ", literal_status[i]);
    }
}