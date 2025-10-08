#include "datatype.h"
#include <stdio.h>
#include <stdlib.h>

void print_formula(Formula *formula)
{
    printf("clause num:%d\n", formula->clause_num);
    printf("variable_num:%d\n", formula->variable_num);
    for (int i = 0; i < formula->clause_num; i++)
    {
        for (int j = 0; j < formula->clause_array[i].literal_num; j++)
        {
            printf("%d ", formula->clause_array[i].literal_array[j]);
        }
        printf("\n");
    }
}
void free_formula(Formula *formula)
{
    for (int i = 0; i < formula->clause_num;i++){
        free(formula->clause_array[i].literal_array);
    }
    free(formula->clause_array);
}