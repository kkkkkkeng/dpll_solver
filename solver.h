#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"                      
int isUnitClause(Clause *clause,int *literal_status);             // �ж�clause�Ƿ�Ϊ����Ԫ�Ӿ�
int record_UnitClause(Formula *formula,int *literal_status,int *clause_status);      // ��formula�еĵ���Ԫ�Ӿ��¼��clause_status��literal_status��; ����ì��ֱ�ӷ���-1;
int init_status(Formula *formula,int *literal_status,int *clause_status);            // ��ʼ��clause_status��literal_status����
int check_clause(Clause *clause,int *literal_status);             // ����Ӿ��Ƿ�����
int select_branch_variable(Formula *formula,int *literal_status); // ѡ���֧��Ԫ
int check_formula(Formula *formula,int *literal_status,int *clause_status);          // ��鵱ǰ״̬�������Ӿ��Ƿ��н�
int dpll_recursive(Formula *formula,int *literal_status,int *clause_status);         // dpll_solve�ڲ����� �ݹ鴦��dpll�㷨
int dpll_solve(Formula *formula,int *solution);             // ����dpll�㷨���ǰ����
int print_solution(Formula *formula,int *solution);             // �����
#endif