#ifndef __DATATYPE_H
#define __DATATYPE_H
typedef struct Clause{
    int literal_num;
    int *literal_array;
} Clause;
typedef struct Formula{
    int clause_num;
    int variable_num;
    Clause* clause_array;
} Formula;
void print_formula(Formula *formula);
void free_formula(Formula *formula);
#endif