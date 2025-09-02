#include "x-sudoku.h"
#include "datatype.h"
#include "cnfparser.h"
#include "solver.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int main()
{
    srand(time(NULL));
    int sudoku[9][9]={0};
    fill_sudoku(sudoku, PERCENT_SUDOKU, 0, 0);
    print_sudoku(sudoku);
    system("pause");
}
 