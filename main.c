#include <stdio.h>
#include <stdlib.h>
#include "cnfparser.h"
#include "solver.h"
#include "x-sudoku.h"
int main()
{
    Formula* formula;
    formula= (Formula*)malloc(sizeof(Formula));
    char filename[20];
    printf("input filename:");
    scanf("%s", filename);
    parse_cnf(filename, formula);
    int *solution = (int *)malloc((formula->variable_num + 1) * sizeof(int));
    double time=0;
    int select_time=0;
    int res=dpll_solve(formula,0, solution, &time,&select_time);
    printf("solve success\n");
    print_solution(formula,solution);
    output_solution_tofile(formula,solution, "solution.res", res, time,select_time);
    free(solution);
    free_formula(formula);
    free(formula);
}