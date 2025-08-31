#ifndef __CNFPARSER_H
#define __CNFPARSER_H
#include "datatype.h"
#define max_buffer_length 8192
// 读取cnf文件 数据存储到Formula结构体中
int parse_cnf(char *filename, Formula *formula);
#endif