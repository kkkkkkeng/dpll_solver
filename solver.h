#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"                      
int isUnitClause(Clause *clause,int *literal_status);             // 判断clause是否为单变元子句
int record_UnitClause(Formula *formula,int *literal_status,int *clause_status);      // 将formula中的单变元子句记录到clause_status和literal_status中; 出现矛盾直接返回-1;
int init_status(Formula *formula,int *literal_status,int *clause_status);            // 初始化clause_status和literal_status数组
int check_clause(Clause *clause,int *literal_status);             // 检查子句是否满足
int select_branch_variable(Formula *formula,int *literal_status); // 选择分支变元
int check_formula(Formula *formula,int *literal_status,int *clause_status);          // 检查当前状态下所有子句是否有解
int dpll_recursive(Formula *formula,int *literal_status,int *clause_status);         // dpll_solve内部调用 递归处理dpll算法
int dpll_solve(Formula *formula,int *solution);             // 调用dpll算法解答当前算例
int print_solution(Formula *formula,int *solution);             // 输出解
#endif