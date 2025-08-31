#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"
int *clause_status;
int *literal_status;
int isUnitClause(Clause *clause);//�ж�clause�Ƿ�Ϊ����Ԫ�Ӿ�
int record_UnitClause(Formula *formula); // ��formula�еĵ���Ԫ�Ӿ��¼��clause_status��literal_status��; ����ì��ֱ�ӷ���-1;
int init_status(Formula *formula);
int check_clause(Clause *clause);
int select_branch_variable(Formula *formula);
int check_formula(Formula *formula);
int dpll_recursive(Formula *formula);
int dpll_solve(Formula *formula);
int print_variable_status(Formula *formula);
#endif