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
    print_formula(&formula);
    int *solution = (int *)malloc(sizeof(int) * (formula.variable_num + 1));
    if (dpll_solve(&formula, solution) == 1)
    {
        print_solution(&formula, solution);
        if (verify_solution(&formula, solution) == 1)
        {
            printf("verify success");
        }
        else
        {
            printf("verify fail");
        }
    }
    else
    {
        printf("error");
    }
    system("pause");
    free_formula(&formula);
    free(solution);
    return 0;
}