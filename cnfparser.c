#include "cnfparser.h"
#include <stdio.h>
#include <stdlib.h>
int parse_cnf(char *filename, Formula *formula)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        return -1;
    }
    char buffer[max_buffer_length];
    int clause_count = 0;
    while (fgets(buffer, 8192, fp) != NULL)
    {
        if (buffer[0] == 'c')
        {
            continue;
        }
        else if (buffer[0] == 'p')
        {
            int variable_num = 0;
            int clause_num = 0;
            sscanf(buffer, "%*c %*s %d %d", &variable_num, &clause_num);
            formula->clause_num = clause_num;
            formula->variable_num = variable_num;
            formula->clause_array = (Clause *)malloc(sizeof(Clause) * clause_num);
            if (formula->clause_array == NULL)
            {
                return -1;
            }
        }
        else
        {
            int temp = 0;
            int literal_count = 0;
            char *ptr = buffer;
            int char_read = 0;
            sscanf(buffer, "%d %n", &temp, &char_read);
            ptr += char_read;
            while (temp)
            {
                literal_count++;
                char_read = 0;
                sscanf(ptr, "%d %n", &temp, &char_read);
                ptr += char_read;
            }
            formula->clause_array[clause_count].literal_num = literal_count;
            formula->clause_array[clause_count].literal_array = (int *)malloc(sizeof(int) * literal_count);
            if (formula->clause_array[clause_count].literal_array == NULL)
            {
                return -1;
            }
            ptr = buffer;
            for (int i = 0; i < literal_count; i++)
            {
                char_read = 0;
                sscanf(ptr, "%d %n", &temp, &char_read);
                ptr += char_read;
                formula->clause_array[clause_count].literal_array[i] = temp;
            }
            clause_count++;
        }
    }
    fclose(fp);
    return 1;
}
int modify_file_name(char *filename)
{
    int i = 0;
    while (filename[i])
    {
        if (filename[i] == '.')
        {
            for (int j = 1; j <=3; j++)
            {
                if (filename[i + j] == '\0')
                    return -1;
            }
            if(filename[i+4]){
                return -1;
            }
            if (filename[i + 1] == 'c' && filename[i + 2] == 'n' && filename[i + 3] == 'f' )
            {
                filename[i + 1] = 'r';
                filename[i + 2] = 'e';
                filename[i + 3] = 's';
                return 1;
            }
        }
        i++;
    }
    return -1;
}
