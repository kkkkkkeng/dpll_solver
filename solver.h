#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"
int *clause_status;
int *literal_status;
int isUnitClause(Clause *clause);//判断clause是否为单变元子句
int record_UnitClause(Formula *formula); // 将formula中的单变元子句记录到clause_status和literal_status中; 出现矛盾直接返回-1;
int init_status(Formula *formula);
int check_clause(Clause *clause);
int select_branch_variable(Formula *formula);
int check_formula(Formula *formula);
int dpll_recursive(Formula *formula);
int dpll_solve(Formula *formula);
int print_variable_status(Formula *formula);
#endif