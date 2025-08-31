#include <stdio.h>
#include <stdlib.h>
#include "cnfparser.h"
#include "solver.h"
#include <time.h>
int main()
{
    clock_t start, end;
    double cpu_time_used;
    Formula formula;
    char filename[20];
    scanf("%s", filename);
    parse_cnf(filename, &formula);
    // print_formula(&formula);
    int *solution = (int *)malloc(sizeof(int) * (formula.variable_num + 1));
    start = clock();
    if (dpll_solve(&formula, solution) == 1)
    {
        print_solution(&formula, solution);
        if (verify_solution(&formula, solution) == 1)
        {
            printf("verify success\n");
        }
        else
        {
            printf("verify fail\n");
        }
    }
    else
    {
        printf("error\n");
    }
    free_formula(&formula);
    free(solution);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time used: %f seconds\n", cpu_time_used);
    system("pause");
    return 0;
}