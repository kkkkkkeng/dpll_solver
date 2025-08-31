#ifndef __SOLVER_H
#define __SOLVER_H
#include "datatype.h"
int findUnitLiteral(Clause *clause, int *literal_status);                                     // ����clause�еĵ���Ԫ
int record_UnitClause(Formula *formula, int *literal_status, int *trail, int *trail_pointer); // ��formula�еĵ���Ԫ�Ӿ��¼��literal_status��; ����ì��ֱ�ӷ���-1;
int init_status(Formula *formula, int *literal_status);                                       // ��ʼ��literal_status����
int check_clause(Clause *clause, int *literal_status);                                        // ����Ӿ��Ƿ�����
int select_branch_variable(Formula *formula, int *literal_status);                            // ѡ���֧��Ԫ
int check_formula(Formula *formula, int *literal_status);                                     // ��鵱ǰ״̬�������Ӿ��Ƿ��н�
int dpll_recursive(Formula *formula, int *literal_status, int *trail, int *trail_pointer);    // dpll_solve�ڲ����� �ݹ鴦��dpll�㷨
int dpll_solve(Formula *formula, int *solution);                                              // ����dpll�㷨���ǰ����
int print_solution(Formula *formula, int *solution);                                          // �����
void assign_literal(int *literal_status, int var, int value, int *trail, int *trail_pointer); // Ϊ��Ԫvar��ֵvalue,������ֵ��Ϣ��¼��trail��
void backtrack(int *literal_status, int level, int *trail, int *trail_pointer);               // ���ݵ�level��
int verify_solution(Formula *formula, int *solution);                                         // ��֤���Ƿ���ȷ
#endif