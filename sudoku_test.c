#include "x-sudoku.h"
#include "datatype.h"
#include "cnfparser.h"
#include "solver.h"
#include <stdio.h>
#include <stdlib.h>
int main()
{
    int sudoku[9][9];
    char filename[20];
    printf("input filename:");
    scanf("%s", filename);
    read_sudoku_from_file(filename, sudoku);
    print_sudoku(sudoku);
    char output_filename[20];
    printf("input output filename:");
    scanf("%s", output_filename);
    generate_sudoku_cnf(sudoku, SUDOKU, output_filename);
    printf("generate cnf success\n");
    Formula formula;
    parse_cnf(output_filename, &formula);
    int *solution = (int *)malloc((formula.variable_num + 1) * sizeof(int));
    dpll_solve(&formula, solution);
    printf("solve success\n");
    convert_solution_to_sudoku(solution, sudoku);
    print_sudoku(sudoku);
    system("pause");
    free(solution);
    free_formula(&formula);
}
