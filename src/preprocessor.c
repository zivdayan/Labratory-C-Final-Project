#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "preprocessor.h"

#define as_file_ext ".as"
#define am_file_ext ".am"
#define MAX_MACRO_LEN 31
#define SPACES " \t\v\f\n"
#define MAX_LINE_LEN 80

/*Todo: change macro table to nodes list*/
struct Macro
{
    char macroName[MAX_MACRO_LEN + 1];
    char **macroLines;
    int lineCounter;
};

struct Macro *searchMacro(struct Macro *macroTable, const int tableSize, const char *name)
{
    int i;
    for (i = 0; i < tableSize; i++)
    {
        if (strcmp(macroTable[i].macroName, name) == 0)
        {
            return &macroTable[i];
        }
    }
    return NULL;
}

static void get_macro_list(struct Macro *macroTable, int macro_count, struct Node **output_macro_list)
{
    struct Node *curr_node;
    struct Node *next_node;
    int i = 0;

    if (!macro_count)
        return;

    curr_node = malloc(sizeof(struct Node));
    curr_node->value = macroTable[i++].macroName;

    *output_macro_list = curr_node;

    while (i < macro_count)
    {
        next_node = malloc(sizeof(struct Node));
        next_node->value = macroTable[i++].macroName;

        curr_node->next = next_node;
        curr_node = next_node;
    }
}

int macro_line(char *s, struct Macro **macro, struct Macro *macro_table, int *table_size)
{
    char *item1 = NULL;
    char *item2 = NULL;
    struct Macro newMacro;
    char *c2;
    struct Macro *f;

    SPLIT_BY_FIRST_SPACE(s, item1, item2);

    if (strstr(item1, "endmcr"))
    {
        *macro = NULL;
        return 0;
    }

    if (strstr(item1, "mcr"))
    {
        char *c2 = strpbrk(item2, SPACES);
        if (c2)
            *c2 = '\0';

        strncpy(newMacro.macroName, item2, MAX_MACRO_LEN);
        newMacro.lineCounter = 0;
        newMacro.macroLines = malloc(10 * sizeof(char *));
        macro_table[*table_size] = newMacro;
        *macro = &macro_table[*table_size];
        (*table_size)++;
        return 1;
    }

    c2 = strpbrk(item2, SPACES);
    if (c2)
        *c2 = '\0';
    f = searchMacro(macro_table, *table_size, item2);

    if (f)
    {
        *macro = f;
        return 2;
    }

    return 3;
}

char *strcatWithMalloc(const char *s1, const char *s2)
{
    size_t len_s1 = strlen(s1);
    size_t len_s2 = strlen(s2);
    size_t len_result = len_s1 + len_s2 + 1;
    char *result = (char *)malloc(len_result * sizeof(char));
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *preproc(char *bname, struct Node **output_macro_list)
{
    char line[MAX_LINE_LEN] = {0};
    struct Macro *macro_table = malloc(10 * sizeof(struct Macro));
    int macro_count = 0;
    FILE *as_file;
    FILE *am_file;
    struct Macro *macro = NULL;
    char *asFileName = strcatWithMalloc(bname, as_file_ext);
    char *amFileName = strcatWithMalloc(bname, am_file_ext);

    int i, j;

    as_file = fopen(asFileName, "r");
    am_file = fopen(amFileName, "w");

    if (!as_file || !am_file)
    {
        free(macro_table);
        return NULL;
    }

    while (fgets(line, MAX_LINE_LEN, as_file))
    {
        switch (macro_line(line, &macro, macro_table, &macro_count))
        {
        case 0:
            macro = NULL;
            break;
        case 1:
            break;
        case 2:
            for (i = 0; i < macro->lineCounter; i++)
            {
                fputs(macro->macroLines[i], am_file);
            }
            macro = NULL;
            break;
        case 3:
            if (macro)
            {
                if (macro->lineCounter >= 10)
                {
                    macro->macroLines = realloc(macro->macroLines, (macro->lineCounter + 10) * sizeof(char *));
                }
                macro->macroLines[macro->lineCounter] = malloc((strlen(line) + 1) * sizeof(char));
                strcpy(macro->macroLines[macro->lineCounter], line);
                macro->lineCounter++;
            }
            else
            {
                fputs(line, am_file);
            }
            break;
        }
    }

    fclose(am_file);
    fclose(as_file);

    get_macro_list(macro_table, macro_count, output_macro_list);

    for (i = 0; i < macro_count; i++)
    {
        for (j = 0; j < macro_table[i].lineCounter; j++)
        {
            free(macro_table[i].macroLines[j]);
        }
        free(macro_table[i].macroLines);
    }

    free(macro_table);
    free(asFileName);
    free(amFileName);

    return amFileName;
}
