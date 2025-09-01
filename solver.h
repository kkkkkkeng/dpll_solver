#ifndef __SOLVER_H
#define __SOLVER_H
#define TIME_LIMIT 15
#define RES_SAT 1
#define RES_UNSAT -1
#define RES_TIME_OUT -2
#include "datatype.h"

int dpll_solve(Formula *formula, int *solution, double *time);                                     // ����dpll�㷨���ǰ����
int print_solution(Formula *formula, int *solution);                                               // �����
int verify_solution(Formula *formula, int *solution);                                              // ��֤���Ƿ���ȷ
int output_solution_tofile(Formula *formula, int *solution, char *filename, int res, double time); // ��������.res�ļ�

#endif