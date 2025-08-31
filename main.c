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
    dpll_solve(&formula);
    print_variable_status(&formula);
    system("pause");
    return 0;
}