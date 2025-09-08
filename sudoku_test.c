// #include "x-sudoku.h"
// #include "datatype.h"
// #include "cnfparser.h"
// #include "solver.h"
// #include <stdio.h>
// #include <time.h>
// #include <stdlib.h>
// int main()
// {
//     srand(time(NULL));
//     int sudoku[9][9] = {0};
//     int arr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
//     int pos[81];
//     for (int i = 0; i < 81; i++)
//         pos[i] = i;
//     shuffle_array(pos, 81);
//     shuffle_array(arr, 9);
//     for (int i = 0; i < 9; i++)
//     {
//         int row = pos[i] / 9;
//         int col = pos[i] % 9;
//         sudoku[row][col] = arr[i];
//     }
//     double time = 0;
//     int select_time = 0;
//     // int res = solve_sudoku(sudoku, OPTIMIZED1, SUDOKU, &time, &select_time);
//     generate_sudoku_cnf(sudoku, PERCENT_SUDOKU, "sudokutest.cnf");
//     Formula formula;
//     parse_cnf("sudokutest.cnf", &formula);
//     int *solution = (int *)malloc((formula.variable_num + 1) * sizeof(int));
//     int res = dpll_solve(&formula, OPTIMIZED1, solution, &time, &select_time);
//     convert_solution_to_sudoku(solution, sudoku);
//     if (res == RES_SAT)
//         print_sudoku(sudoku);
//     else if (res == RES_TIME_OUT)
//         printf("Time out!");
//     else
//     {
//         printf("No solution");
//     }
//     system("pause");
//     return 0;
// }
