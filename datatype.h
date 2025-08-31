#ifndef __DATATYPE_H
#define __DATATYPE_H
typedef struct Clause{
    int literal_num;
    int literal_count;
    int *literal_array;
} Clause;
typedef struct Formula{
    int clause_num;
    int clause_count;
    int variable_num;
    int variable_count;
    Clause* clause_array;
} Formula;
#endif