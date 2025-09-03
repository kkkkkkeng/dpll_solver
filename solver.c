#include "solver.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct vector
{
    int *data;
    int size;
    int capacity;
} vector;

typedef struct queue
{
    int *data;
    int front;
    int rear;
    int capacity;
} queue;
static void init_vector(vector *v);               // 初始化动态数组
static void add_to_vector(vector *v, int value);   // 向动态数组中添加元素
static void free_vector(vector *v);               // 释放动态数组
static void init_queue(queue *q, int capacity);    // 初始化队列
static int is_empty(queue *q);                     // 判断队列是否为空
static int enqueue(queue *q, int value);          // 入队
static int dequeue(queue *q, int *value);         // 出队
static void free_queue(queue *q);                  // 释放队列

static void assign_literal(int *literal_status, int var, int value, int *trail, int *trail_pointer);                                                                // 为变元var赋值value,并将赋值信息记录到trail中
static void backtrack(int *literal_status, int level, int *trail, int *trail_pointer);                                                                              // 回溯到level层
static int findUnitLiteral(Clause *clause, int *literal_status);                                                                                                    // 查找clause中的单变元
static int record_UnitClause(Formula *formula, int *literal_status, int *trail, int *trail_pointer,vector * variable_map);                                                                // 将formula中的单变元子句记录到literal_status中; 出现矛盾直接返回-1;
static int init_status(Formula *formula, int *literal_status);                                                                                                      // 初始化literal_status数组
static int check_clause(Clause *clause, int *literal_status);                                                                                                       // 检查子句是否满足
static int select_branch_variable(Formula *formula, int *literal_status, int branch_select_straregy);                                                               // 选择分支变元
static int check_formula(Formula *formula, int *literal_status);                                                                                                    // 检查当前状态下所有子句是否有解
static int dpll_recursive(Formula *formula, int *literal_status, int branch_select_strategy, int *trail, int *trail_pointer, vector* variable_map,clock_t start_time, int *select_time); // dpll_solve内部调用 递归处理dpll算法
static int is_solution_unique_recursive(Formula *formula, int *literal_status, int *trail, int *trail_pointer,vector* variable_map, clock_t start_time, int *solution_count);

int is_solution_unique(Formula *formula, int branch_select_strategy, int *solution_count)
{
    vector* variable_map = (vector *)malloc(sizeof(vector) * (formula->variable_num + 1));
    for (int i = 0; i <= formula->variable_num; i++)
    {
        init_vector(&variable_map[i]);
    }
    for (int i = 0; i < formula->clause_num; i++)
    {
        for (int j = 0; j < formula->clause_array[i].literal_num; j++)
        {
            int var = abs(formula->clause_array[i].literal_array[j]);
            add_to_vector(&variable_map[var], i);
        }
    }
    clock_t start_time = clock();
    int *literal_status = (int *)malloc(sizeof(int) * (formula->variable_num + 1));
    int *trail = (int *)malloc(sizeof(int) * (formula->variable_num + 1));
    int trail_pointer = 0;
    *solution_count = 0;
    init_status(formula, literal_status);
    int res = is_solution_unique_recursive(formula, literal_status, trail, &trail_pointer ,variable_map,start_time, solution_count);
    if (res == RES_TIME_OUT)
    {
        return RES_TIME_OUT;
    }
    if (*solution_count > 1)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

static int is_solution_unique_recursive(Formula *formula, int *literal_status, int *trail, int *trail_pointer, vector* variable_map,clock_t start_time, int *solution_count)
{
    if ((double)(clock() - start_time) > TIME_LIMIT * CLOCKS_PER_SEC)
    {
        return RES_TIME_OUT;
    }
    int res = record_UnitClause(formula, literal_status, trail, trail_pointer,variable_map);
    if (res == -1)
    {
        return RES_UNSAT;
    }
    res = check_formula(formula, literal_status);
    if (res == -1)
    {
        return RES_UNSAT;
    }
    else if (res == 1)
    {
        (*solution_count)++;
        if (*solution_count > 1)
        {
            return RES_SAT;
        }
        else
            return RES_UNSAT;
    }
    else
    {
        int next_variable = select_branch_variable(formula, literal_status, OPTIMIZED1);
        if (next_variable == 0)
        {
            return RES_UNSAT;
        }
        int level = *trail_pointer;
        assign_literal(literal_status, next_variable, 1, trail, trail_pointer);
        int res = is_solution_unique_recursive(formula, literal_status, trail, trail_pointer, variable_map,start_time, solution_count);
        if (res == RES_TIME_OUT)
        {
            return RES_TIME_OUT;
        }
        else if (res == RES_UNSAT)
        {
            backtrack(literal_status, level, trail, trail_pointer);
            assign_literal(literal_status, next_variable, -1, trail, trail_pointer);
            res = is_solution_unique_recursive(formula, literal_status, trail, trail_pointer,variable_map, start_time, solution_count);
            if (res == RES_TIME_OUT)
            {
                return RES_TIME_OUT;
            }
            else if (res == RES_UNSAT)
            {
                backtrack(literal_status, level, trail, trail_pointer);
                return RES_UNSAT;
            }
            else
            {
                return RES_SAT;
            }
        }
        else
        {
            return RES_SAT;
        }
    }
}

static int findUnitLiteral(Clause *clause, int *literal_status)
{
    int literal_count = 0;
    int unit_literal = 0;
    for (int i = 0; i < clause->literal_num; i++)
    {
        int variable = abs(clause->literal_array[i]);
        if (!literal_status[variable])
        {
            literal_count++;
            unit_literal = clause->literal_array[i];
        }
        else if (literal_status[variable] * clause->literal_array[i] > 0)
        {
            return 0;
        }
    }
    if (literal_count == 1)
    {
        return unit_literal;
    }
    return 0;
}
static int init_status(Formula *formula, int *literal_status)
{
    for (int i = 0; i <= formula->variable_num; i++)
    {
        literal_status[i] = 0;
    }
}


static void init_vector(vector *v)
{
    v->capacity = 16;
    v->size = 0;
    v->data = (int *)malloc(sizeof(int) * v->capacity);
}
static void add_to_vector(vector *v, int value)
{
    if (v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = (int *)realloc(v->data, sizeof(int) * v->capacity);
    }
    v->data[v->size] = value;
    v->size++;
}
static void free_vector(vector *v)
{
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}


static void init_queue(queue *q,int capacity){
    q->capacity = capacity;
    q->front = 0;
    q->rear = 0;
    q->data = (int *)malloc(sizeof(int)*capacity);
}
static int is_empty(queue *q){
    return q->front == q->rear;
}
static int enqueue(queue *q,int value){
    if((q->rear + 1) % q->capacity == q->front){
        return -1;
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % q->capacity;
    return 1;
}
static int dequeue(queue *q,int *value){
    if(is_empty(q)){
        return -1;
    }
    *value = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    return 1;
}
static void free_queue(queue *q){
    free(q->data);
    q->data = NULL;
    q->front = 0;
    q->rear = 0;
    q->capacity = 0;
}


static int record_UnitClause(Formula *formula,int *literal_status,int *trail,int* trail_pointer,vector *variable_map){
    queue q;
    int* inqueue_flag = (int *)malloc(sizeof(int)*(formula->clause_num + 1));
    init_queue(&q, formula->clause_num + 1);
    for (int i = 0;i< formula->clause_num;i++){
        int unit_literal = findUnitLiteral(&(formula->clause_array[i]), literal_status);
        if (unit_literal){
            int variable = abs(unit_literal);
            assign_literal(literal_status, variable, unit_literal > 0 ? 1 : -1, trail, trail_pointer);
            for (int j = 0;j<variable_map[variable].size;j++){
                int clause_index = variable_map[variable].data[j];
                if (inqueue_flag[clause_index]){
                    continue;
                }
                if (enqueue(&q, clause_index) == -1)
                {
                    for (int k = 0; k <= formula->variable_num; k++)
                    {
                        free_vector(&variable_map[k]);
                    }
                    free_queue(&q);
                    free(inqueue_flag);
                    return -1;
                }
                inqueue_flag[clause_index] = 1;
            }
        }
    }
    while(!is_empty(&q)){
        int clause_index = 0;
        dequeue(&q,&clause_index);
        inqueue_flag[clause_index] = 0;
        int unit_literal = findUnitLiteral(&(formula->clause_array[clause_index]), literal_status);
        if(unit_literal){
            int variable = abs(unit_literal);
            assign_literal(literal_status, variable, unit_literal > 0 ? 1 : -1, trail, trail_pointer);
            for (int j = 0; j < variable_map[variable].size; j++)
            {
                int clause_index = variable_map[variable].data[j];
                if (inqueue_flag[clause_index])
                {
                    continue;
                }
                if (enqueue(&q, clause_index) == -1)
                {
                    for (int k = 0; k <= formula->variable_num; k++)
                    {
                        free_vector(&variable_map[k]);
                    }
                    free_queue(&q);
                    free(inqueue_flag);
                    return -1;
                }
                inqueue_flag[clause_index] = 1;
            }
        }
    }
    for (int i = 0;i<= formula->variable_num;i++){
        free_vector(&variable_map[i]);
    }
    free_queue(&q);
    free(inqueue_flag);
    return 1;
}

// static int record_UnitClause(Formula *formula, int *literal_status, int *trail, int *trail_pointer)
// {
//     int isnewassigned = 1;
//     while (isnewassigned)
//     {
//         isnewassigned = 0;
//         for (int i = 0; i < formula->clause_num; i++)
//         {
//             int unit_literal = findUnitLiteral(&(formula->clause_array[i]), literal_status);
//             if (unit_literal)
//             {
//                 int variable = abs(unit_literal);
//                 assign_literal(literal_status, variable, unit_literal > 0 ? 1 : -1, trail, trail_pointer);
//                 isnewassigned = 1;
//             }
//         }
//     }
//     return 1;
// }
static int check_clause(Clause *clause, int *literal_status)
{
    int literal_count = clause->literal_num;
    for (int i = 0; i < clause->literal_num; i++)
    {
        int variable = abs(clause->literal_array[i]);
        if (literal_status[variable])
        {
            if (literal_status[variable] * clause->literal_array[i] > 0)
                return 1;
            else
            {
                literal_count--;
            }
        }
    }
    if (literal_count == 0)
    {
        return -1;
    }
    return 0;
}

static int select_branch_variable(Formula *formula, int *literal_status, int branch_select_strategy)
{
    if (branch_select_strategy == RANDOM)
    {
        for (int i = 1; i <= formula->variable_num; i++)
        {
            if (!literal_status[i])
            {
                return i;
            }
        }
    }
    else
    {
        int count_variable[formula->variable_num + 1];
        for (int i = 0; i <= formula->variable_num; i++)
        {
            count_variable[i] = 0;
        }
        for (int i = 0; i < formula->clause_num; i++)
        {
            for (int j = 0; j < formula->clause_array[i].literal_num; j++)
            {
                int var = abs(formula->clause_array[i].literal_array[j]);
                if (!literal_status[var])
                {
                    count_variable[var]++;
                }
            }
        }
        int max_var = 0;
        for (int i = 1; i <= formula->variable_num; i++)
        {
            if (!literal_status[i] && count_variable[i] > count_variable[max_var])
            {
                max_var = i;
            }
        }
        return max_var;
    }
}
static int check_formula(Formula *formula, int *literal_status)
{
    int clause_count = formula->clause_num;
    for (int i = 0; i < formula->clause_num; i++)
    {
        int res = check_clause(&(formula->clause_array[i]), literal_status);
        if (res == -1)
        {
            return -1;
        }
        else if (res == 1)
        {
            clause_count--;
        }
    }
    if (clause_count == 0)
    {
        return 1;
    }
    return 0;
}

static int dpll_recursive(Formula *formula, int *literal_status, int branch_select_straregy, int *trail, int *trail_pointer,vector* variable_map, clock_t start_time, int *select_time)
{
    if ((double)(clock() - start_time) > TIME_LIMIT * CLOCKS_PER_SEC)
    {
        return RES_TIME_OUT;
    }
    int res = record_UnitClause(formula, literal_status, trail, trail_pointer,variable_map);
    if (res == -1)
    {
        return RES_UNSAT;
    }
    res = check_formula(formula, literal_status);
    if (res == -1)
    {
        return RES_UNSAT;
    }
    else if (res == 1)
    {
        return RES_SAT;
    }
    else
    {
        int next_variable = select_branch_variable(formula, literal_status, branch_select_straregy);
        (*select_time)++;
        if (next_variable == 0)
        {
            return RES_UNSAT;
        }
        int level = *trail_pointer;
        assign_literal(literal_status, next_variable, 1, trail, trail_pointer);
        int res = dpll_recursive(formula, literal_status, branch_select_straregy, trail, trail_pointer, variable_map, start_time, select_time);
        if (res == RES_TIME_OUT)
        {
            return RES_TIME_OUT;
        }
        else if (res == 1)
        {
            return RES_SAT;
        }
        backtrack(literal_status, level, trail, trail_pointer);
        assign_literal(literal_status, next_variable, -1, trail, trail_pointer);
        res = dpll_recursive(formula, literal_status, branch_select_straregy, trail, trail_pointer, variable_map, start_time, select_time);
        if (res == RES_TIME_OUT)
        {
            return RES_TIME_OUT;
        }

        else if (res == 1)
        {
            return RES_SAT;
        }
        backtrack(literal_status, level, trail, trail_pointer);
        return RES_UNSAT;
    }
}
int dpll_solve(Formula *formula, int branch_select_strategy, int *solution, double *time, int *select_time)
{
    clock_t start_time = clock();
    *select_time = 0;
    int *literal_status = (int *)malloc(sizeof(int) * (formula->variable_num + 1)); // 存储每个变量的bool值 1true -1false 0未定 index 1~n+1         // 存储每个clause是否为单变元子句
    int *trail = (int *)malloc(sizeof(int) * (formula->variable_num + 1));
    int trail_pointer = 0;
    init_status(formula, literal_status);
    vector variable_map[formula->variable_num + 1];
    for (int i = 0; i <= formula->variable_num; i++)
    {
        init_vector(&variable_map[i]);
    }
    for (int i = 0; i < formula->clause_num; i++)
    {
        for (int j = 0; j < formula->clause_array[i].literal_num; j++)
        {
            int var = abs(formula->clause_array[i].literal_array[j]);
            add_to_vector(&variable_map[var], i);
        }
    }
    int res = dpll_recursive(formula, literal_status, branch_select_strategy, trail, &trail_pointer, variable_map,start_time, select_time);
    clock_t end_time = clock();
    *time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    if (res == RES_SAT)
    {
        for (int i = 1; i <= formula->variable_num; i++)
        {
            solution[i] = literal_status[i];
        }
    }
    for (int i = 0; i <= formula->variable_num; i++)
    {
        free_vector(&variable_map[i]);
    }
    free(literal_status);
    free(trail);
    return res;
}
static void assign_literal(int *literal_status, int var, int value, int *trail, int *trail_pointer)
{
    literal_status[var] = value;
    trail[*trail_pointer] = var;
    (*trail_pointer)++;
}
static void backtrack(int *literal_status, int level, int *trail, int *trail_pointer)
{
    while (*trail_pointer > level)
    {
        int var = trail[*trail_pointer - 1];
        literal_status[var] = 0;
        (*trail_pointer)--;
    }
}
int verify_solution(Formula *formula, int *solution)
{
    for (int i = 0; i < formula->clause_num; i++)
    {
        int is_clause_satisfied = 0;
        for (int j = 0; j < formula->clause_array[i].literal_num; j++)
        {
            int variable = abs(formula->clause_array[i].literal_array[j]);
            if (solution[variable] * formula->clause_array[i].literal_array[j] > 0)
            {
                is_clause_satisfied = 1;
                break;
            }
        }
        if (is_clause_satisfied == 0)
        {
            return -1;
        }
    }
    return 1;
}
int print_solution(Formula *formula, int *solution)
{
    for (int i = 1; i <= formula->variable_num; i++)
    {
        if (solution[i] == 1)
        {
            printf("%d ", i);
        }
        else
        {
            printf("%d ", -i);
        }
    }
    printf("\n");
}
int output_solution_tofile(Formula *formula, int *solution, char *filename, int res, double time, int select_time)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("open file failed\n");
        return -1;
    }
    if (res == RES_UNSAT)
    {
        fprintf(fp, "s 0\n");
    }
    else if (res == RES_SAT)
    {
        fprintf(fp, "s 1\n");
        fprintf(fp, "v ");
        for (int i = 1; i <= formula->variable_num; i++)
        {
            fprintf(fp, "%d ", solution[i] > 0 ? i : -i);
        }
        fprintf(fp, "\n");
    }
    else if (res == RES_TIME_OUT)
    {
        fprintf(fp, "s -1\n");
    }
    fprintf(fp, "t finding solution after %.6Lf seconds and %d times selecting branch variable\n", time, select_time);
    fclose(fp);
    return 1;
}