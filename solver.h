#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"
int *clause_status;                           // �洢ÿ���Ӿ��Ƿ������״̬ index 0~n
int *literal_status;                          // �洢ÿ��������boolֵ 1true -1false 0δ�� index 1~n+1
int isUnitClause(Clause *clause);             // �ж�clause�Ƿ�Ϊ����Ԫ�Ӿ�
int record_UnitClause(Formula *formula);      // ��formula�еĵ���Ԫ�Ӿ��¼��clause_status��literal_status��; ����ì��ֱ�ӷ���-1;
int init_status(Formula *formula);            // ��ʼ��clause_status��literal_status����
int check_clause(Clause *clause);             // ����Ӿ��Ƿ�����
int select_branch_variable(Formula *formula); // ѡ���֧��Ԫ
int check_formula(Formula *formula);          // ��鵱ǰ״̬�������Ӿ��Ƿ��н�
int dpll_recursive(Formula *formula);         // dpll_solve�ڲ����� �ݹ鴦��dpll�㷨
int dpll_solve(Formula *formula);             // ����dpll�㷨���ǰ����
int print_variable_status(Formula *formula);  // ������б�Ԫ��״̬
#endif