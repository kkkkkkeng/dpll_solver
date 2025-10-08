#ifndef __SOLVER_H
#define __SOLVER_H
#define TIME_LIMIT 300
#define RES_SAT 1
#define RES_UNSAT -1
#define RES_TIME_OUT -2
#define RANDOM 1
#define OPTIMIZED1 2
#define OPTIMIZED2 3
#define OPTIMIZED3 4
#include "datatype.h"

int dpll_solve(Formula *formula, int branch_select_strategy, int *solution, double *time, int *select_time);        // ����dpll�㷨���ǰ����
int print_solution(Formula *formula, int *solution);                                                                     // �����
int verify_solution(Formula *formula, int *solution);                                                                    // ��֤���Ƿ���ȷ
int output_solution_tofile(Formula *formula, int *solution, char *filename, int res, double time, int select_time); // ��������.res�ļ�
int is_solution_unique(Formula *formula, int branch_select_strategy,  int *solution_count); // ����������
#endif