#include <stdio.h>
#include <stdlib.h>
#include "cnfparser.h"
#include "solver.h"
int main()
{
    Formula formula;
    char filename[20];
    scanf("%s", filename);
    parse_cnf(filename, &formula);
    print_formula(formula);
    if(dpll_solve(&formula)==1) print_variable_status(&formula);
    else{
        printf("error");
    }
    system("pause");
    return 0;
}