#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"
int *clause_status;                           // 存储每个子句是否满足的状态 index 0~n
int *literal_status;                          // 存储每个变量的bool值 1true -1false 0未定 index 1~n+1
int isUnitClause(Clause *clause);             // 判断clause是否为单变元子句
int record_UnitClause(Formula *formula);      // 将formula中的单变元子句记录到clause_status和literal_status中; 出现矛盾直接返回-1;
int init_status(Formula *formula);            // 初始化clause_status和literal_status数组
int check_clause(Clause *clause);             // 检查子句是否满足
int select_branch_variable(Formula *formula); // 选择分支变元
int check_formula(Formula *formula);          // 检查当前状态下所有子句是否有解
int dpll_recursive(Formula *formula);         // dpll_solve内部调用 递归处理dpll算法
int dpll_solve(Formula *formula);             // 调用dpll算法解答当前算例
int print_variable_status(Formula *formula);  // 输出所有变元的状态
#endif