// #include <stdio.h>
// #include <stdlib.h>
// #include "cnfparser.h"
// #include "solver.h"
// #include "x-sudoku.h"
// #include <conio.h>
// #include <string.h>
// #include <time.h>
// void print_menu()
// {
//     system("cls");
//     for (int i = 0; i < 50; i++)
//         printf("*");
//     printf("\n");
//     printf("         Welcome to DPLL Solver!\n");
//     printf("         1. Solve CNF\n");
//     printf("         2. Solve Sudoku\n");
//     printf("         3. Exit\n");
//     for (int i = 0; i < 50; i++)
//         printf("*");
//     printf("\n");
// }
// void print_sudoku_mean()
// {
//     system("cls");
//     for (int i = 0; i < 50; i++)
//         printf("*");
//     printf("\n");
//     printf("         Sudoku Solver\n");
//     printf("         1. Generate Sudoku board\n");
//     printf("         2. Read Sudoku board from file\n");
//     printf("         3. Back to Main Menu\n");
//     printf("         4. Exit\n");
//     for (int i = 0; i < 50; i++)
//         printf("*");
//     printf("\n");
// }

// int main()
// {
//     srand(time(NULL));
//     char choice;
//     while (1)
//     {
//         print_menu();
//         choice = getch();
//         switch (choice)
//         {
//         case '1':
//             system("cls");
//             printf("Please enter the filename of the CNF file:\n");
//             char filename[100];
//             gets(filename);
//             Formula formula;
//             int res = parse_cnf(filename, &formula);
//             if (res == -1)
//             {
//                 printf("Error: Failed to parse CNF file.\n");
//                 getchar();
//             }
//             else
//             {
//                 print_formula(&formula);
//                 printf("CNF file parsed successfully.\n");
//                 int select_time = 0;
//                 double time = 0.0;
//                 int *solution = (int *)malloc((formula.variable_num + 1) * sizeof(int));
//                 printf("Solving...\n");
//                 res = dpll_solve(&formula, 1, solution, &time, &select_time);
//                 printf("DPLL Solver finished.\n");
//                 modify_file_name(filename);
//                 output_solution_tofile(&formula, solution, filename, res, time, select_time);
//                 printf("The solution has been written to the file %s\n", filename);
//                 getchar();
//             }
//             break;
//         case '2':
//             while (1)
//             {
//                 print_sudoku_mean();
//                 choice = getch();
//                 int back = 0;
//                 switch(choice){
//                     case '1':

//                         break;
//                     case '2':
//                         system("cls");
//                         printf("Please enter the filename of the Sudoku file:\n");
//                         char filename[100];
//                         gets(filename);
//                         printf("Please choose the type(1:normal sudoku 2:%%sudoku):\n");
//                         char ch = getchar();
//                         getchar();
//                         int category;
//                         if(ch=='1'){
//                             category=SUDOKU;
//                         }
//                         else if(ch=='2'){
//                             category=PERCENT_SUDOKU;
//                         }
//                         else{
//                             printf("Error: Invalid category.\n");
//                             getchar();
//                             break;
//                         }
//                         int sudoku[9][9];
//                         int count = 0;
//                         char cnf_filename[100];
//                         int res=read_sudoku_from_file(filename,sudoku);
//                         while(res!=0&&res!=-1){
//                             res=read_sudoku_from_file(filename,sudoku);
//                             count++;
//                             printf("The Sudoku board %d is:\n",count);
//                             print_sudoku(sudoku);
//                             sprintf(cnf_filename,"temp/%s_%d.cnf",filename,count);
//                             int res2=generate_sudoku_cnf(sudoku,category,cnf_filename);
//                             if(res2==-1){
//                                 printf("Error: Failed to generate CNF file.\n");
//                                 break;
//                             }
//                         };
//                         if(res==-1){
//                             printf("Error: Failed to read Sudoku board from file.\n");
//                             break;
//                         }
//                         printf("Read %d Sudoku boards from file %s\n",count,filename);
//                         printf("Generate %d CNF files into temp filefolder\n",count);
//                         printf("Continue to solve Sudoku board?(y/n)\n");
//                         char c;
//                         c = getchar();
//                         getchar();
//                         if(c=='n'){
//                             break;
//                         }
//                         else{
//                             char cnf_filename[100];
//                             for (int i = 1; i <= count; i++)
//                             {
//                                 int sudoku[9][9];
//                                 sprintf(cnf_filename,"temp/%s_%d.cnf",filename,i);
//                                 Formula formula;
//                                 int *solution=(int *)malloc((formula.variable_num+1)*sizeof(int));
//                                 double time;
//                                 int select_time;
//                                 parse_cnf(cnf_filename,&formula);
//                                 dpll_solve(&formula,1,solution,&time,&select_time);
//                                 printf("The solution of Sudoku board %d is:\n",i);
//                                 convert_solution_to_sudoku(solution, sudoku);
//                                 print_sudoku(sudoku);
//                                 printf("Time: %f\n",time);
//                                 printf("Selecting branch variable times: %d\n",select_time);
//                                 getchar();
//                             }
//                         }
//                             break;
//                         case '3':
//                             back = 1;
//                             break;
//                         case '4':
//                             exit(0);
//                             break;
//                         default:
//                             break;
//                 }
//                 if(back){
//                     break;
//                 }
//             }
//             break;
//         case '3':
//             exit(0);
//             break;
//         default:
//             break;
//         }
//     }
//     system("pause");
//     return 0;
// }