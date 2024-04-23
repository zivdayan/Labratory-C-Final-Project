#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "preprocessor.h"


#define as_file_ext ".as"
#define am_file_ext ".am"
#define MAX_MACRO_LEN 31
#define MAX_MACROS 100
#define SPACES " \t\v\f\n"
#define MAX_LINE_LEN 80

struct Macro
{
    char macroName[MAX_MACRO_LEN + 1];
    char macroLines[MAX_MACROS][MAX_MACRO_LEN];
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

int macro_line(char *s, struct Macro **macro, struct Macro *macro_table, int *table_size)
{
    char *c1 = s, *c2;
    struct Macro *f;
    /* AFTER_SPACE(s);*/
    c1 = strstr(s, "endmcr");
    if (c1)
    {
        *macro = NULL;
        return 0;
    }
    c1 = strstr(s, "mcr");
    if (c1)
    {
        c1 += 4;
        /* AFTER_SPACE(c1); */
        c2 = strpbrk(c1, SPACES);
        if (c2)
            *c2 = '\0';
        strcpy(macro_table[*table_size].macroName, c1);
        *macro = &macro_table[*table_size];
        (*table_size)++;
        return 1;
    }

    c2 = strpbrk(s, SPACES);
    if (c2)
    {
        c1 = c2;
        /* AFTER_SPACE(c2); */
        if (*c2 != '\0')
            return 3;
        *c1 = '\0';
    }

    f = searchMacro(macro_table, *table_size, s);
    if (f)
    {
        (*macro) = f;
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

char *preproc(char *bname)
{
    char line[MAX_LINE_LEN] = {0};
    struct Macro macro_table[MAX_MACROS];
    int macro_count = 0;

    FILE *as_file;
    FILE *am_file;

    struct Macro *macro = NULL;

    char *asFileName;
    char *amFileName;

    int line_co = 1;
    int i;

    asFileName = strcatWithMalloc(bname, as_file_ext);
    amFileName = strcatWithMalloc(bname, am_file_ext);

    as_file = fopen(asFileName, "r");
    am_file = fopen(amFileName, "w");

    if (!as_file || !am_file)
        return NULL;

    while (fgets(line, MAX_LINE_LEN, as_file) != 0)
    {
        switch (macro_line(line, &macro, &macro_table[0], &macro_count))
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
                strcpy(macro->macroLines[macro->lineCounter], line);
                macro->lineCounter++;
            }
            else
            {
                fputs(line, am_file);
            }
            break;
        }
        line_co++;
    }

    fclose(am_file);
    fclose(as_file);
    free(asFileName);
    return amFileName;
}
