#include "solver.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void assign_literal(int *literal_status, int var, int value, int *trail, int *trail_pointer);              // Ϊ��Ԫvar��ֵvalue,������ֵ��Ϣ��¼��trail��
static void backtrack(int *literal_status, int level, int *trail, int *trail_pointer);                            // ���ݵ�level��
static int findUnitLiteral(Clause *clause, int *literal_status);                                                  // ����clause�еĵ���Ԫ
static int record_UnitClause(Formula *formula, int *literal_status, int *trail, int *trail_pointer);              // ��formula�еĵ���Ԫ�Ӿ��¼��literal_status��; ����ì��ֱ�ӷ���-1;
static int init_status(Formula *formula, int *literal_status);                                                    // ��ʼ��literal_status����
static int check_clause(Clause *clause, int *literal_status);                                                     // ����Ӿ��Ƿ�����
static int select_branch_variable(Formula *formula, int *literal_status);                                         // ѡ���֧��Ԫ
static int check_formula(Formula *formula, int *literal_status);                                                  // ��鵱ǰ״̬�������Ӿ��Ƿ��н�
static int dpll_recursive(Formula *formula, int *literal_status, int *trail, int *trail_pointer, int start_time); // dpll_solve�ڲ����� �ݹ鴦��dpll�㷨

static int findUnitLiteral(Clause *clause, int *literal_status)
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
static int init_status(Formula *formula, int *literal_status)
{
    for (int i = 0; i <= formula->variable_num; i++)
    {
        literal_status[i] = 0;
    }
}

static int record_UnitClause(Formula *formula, int *literal_status, int *trail, int *trail_pointer)
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
static int check_clause(Clause *clause, int *literal_status)
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

static int select_branch_variable(Formula *formula, int *literal_status)
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
static int check_formula(Formula *formula, int *literal_status)
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
static int dpll_recursive(Formula *formula, int *literal_status, int *trail, int *trail_pointer, int start_time)
{
    if (clock() - start_time > TIME_LIMIT * CLOCKS_PER_SEC)
    {
        return RES_TIME_OUT;
    }
    int res = record_UnitClause(formula, literal_status, trail, trail_pointer);
    if (res == -1)
    {
        return RES_UNSAT;
    }
    res = check_formula(formula, literal_status);
    if (res == -1)
    {
        return RES_UNSAT;
    }
    else if (res == 1)
    {
        return RES_SAT;
    }
    else
    {
        int next_variable = select_branch_variable(formula, literal_status);
        if (next_variable == 0)
        {
            return RES_UNSAT;
        }
        int level = *trail_pointer;
        assign_literal(literal_status, next_variable, 1, trail, trail_pointer);
        int res = dpll_recursive(formula, literal_status, trail, trail_pointer, start_time);
        if (res == 1)
        {
            return RES_SAT;
        }
        backtrack(literal_status, level, trail, trail_pointer);
        assign_literal(literal_status, next_variable, -1, trail, trail_pointer);
        res = dpll_recursive(formula, literal_status, trail, trail_pointer, start_time);
        if (res == 1)
        {
            return RES_SAT;
        }
        backtrack(literal_status, level, trail, trail_pointer);
        return RES_UNSAT;
    }
}
int dpll_solve(Formula *formula, int *solution, double *time)
{
    clock_t start_time = clock();
    int *literal_status = (int *)malloc(sizeof(int) * (formula->variable_num + 1)); // �洢ÿ��������boolֵ 1true -1false 0δ�� index 1~n+1         // �洢ÿ��clause�Ƿ�Ϊ����Ԫ�Ӿ�
    int *trail = (int *)malloc(sizeof(int) * (formula->variable_num + 1));
    int trail_pointer = 0;
    init_status(formula, literal_status);
    int res = dpll_recursive(formula, literal_status, trail, &trail_pointer, start_time);
    clock_t end_time = clock();
    *time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    if (res == RES_SAT)
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
static void assign_literal(int *literal_status, int var, int value, int *trail, int *trail_pointer)
{
    literal_status[var] = value;
    trail[*trail_pointer] = var;
    (*trail_pointer)++;
}
static void backtrack(int *literal_status, int level, int *trail, int *trail_pointer)
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
int output_solution_tofile(Formula *formula, int *solution, char *filename, int res, double time)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("open file failed\n");
        return -1;
    }
    if (res == RES_UNSAT)
    {
        fprintf(fp, "s 0\n");
    }
    else if (res == RES_SAT)
    {
        fprintf(fp, "s 1\n");
        fprintf(fp, "v ");
        for (int i = 1; i <= formula->variable_num; i++)
        {
            fprintf(fp, "%d ", solution[i] > 0 ? i : -i);
        }
        fprintf(fp, "\n");
    }
    else if(res == RES_TIME_OUT){
        fprintf(fp, "s -1\n");
    }
    fprintf(fp, "t %lf seconds\n", time);
    fclose(fp);
    return 1;
}