#ifndef __CNFPARSER_H
#define __CNFPARSER_H
#include "datatype.h"
#define max_buffer_length 8192
// ��ȡcnf�ļ� ���ݴ洢��Formula�ṹ����
int parse_cnf(char *filename, Formula *formula);
#endif