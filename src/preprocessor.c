#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "preprocessor.h"
#include "utils.h"
#include "global_consts.h"

extern char *strdup(const char *);
char *strndup(const char *str, size_t size);

void trim_leading_spaces(char *str)
{
    int index = 0, i = 0;

    /* Find the index of the first non-space character */
    while (str[index] != '\0' && isspace((unsigned char)str[index]))
    {
        index++;
    }

    /* Shift all characters to the left */
    if (index != 0)
    {
        while (str[index] != '\0')
        {
            str[i++] = str[index++];
        }
        str[i] = '\0'; /* Null-terminate the trimmed string */
    }
}

void split_by_first_space(char *str, char **item1, char **item2)
{
    /* Use strtok to split the string */
    char *token = strchr(str, ' ');

    /* If there's no space, the entire string is the first part */
    if (token != NULL)
    {
        /* Allocate memory and copy the first part */
        *item1 = strndup(str, token - str);

        /* Allocate memory and copy the second part */
        *item2 = strdup(token + 1);
    }
    else
    {
        *item1 = strdup(str); /* No space found, whole string is first part */
        *item2 = "";
    }
}

struct Macro
{
    char macroName[MAX_MACRO_LEN + 1];
    char **macroLines;
    int lineCounter;
};

struct Macro *search_macro(struct Macro *macroTable, const int tableSize, char *name)
{
    int i;
    name[strcspn(name, "\r\n")] = 0; /* Mark end of line */
    for (i = 0; i < tableSize; i++)
    {
        if (strcmp(macroTable[i].macroName, name) == 0)
        {
            return &macroTable[i];
        }
    }
    return NULL;
}

static void get_macro_list(struct Macro *macroTable, int macro_count, struct Node *output_macro_list)
{
    struct Node *curr_node;
    struct Node *next_node;
    int i = 0;

    if (!macro_count)
        return;

    output_macro_list = malloc(sizeof(struct Node));

    curr_node = output_macro_list;
    strcpy(curr_node->value, macroTable[i++].macroName);

    while (i < macro_count)
    {
        next_node = malloc(sizeof(struct Node));
        strcpy(next_node->value, macroTable[i++].macroName);

        curr_node->next = next_node;
        curr_node = next_node;
    }
}

int macro_line(char *s, struct Macro **macro, struct Macro *macro_table, int *table_size)
{
    char *item1;
    char *item2;
    struct Macro newMacro;
    char *c2;
    struct Macro *f;

    item1 = malloc(sizeof(char *) * (MAX_LINE_LENGTH + 1));
    item2 = malloc(sizeof(char *) * (MAX_LINE_LENGTH + 1));

    trim_leading_spaces(s);

    split_by_first_space(s, &item1, &item2);

    if (*item1 == ';')
        return 1;

    if (strstr(item1, "endmcr"))
    {
        *macro = NULL;
        return 0;
    }

    if (!strcmp(item1, "mcr"))
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
    f = search_macro(macro_table, *table_size, item1);

    free(item1);
    if (strcmp(item2, ""))
        free(item2);

    if (f)
    {
        *macro = f;
        return 2;
    }

    return 3;
}

char *preproc(char *bname, struct Node *output_macro_list)
{
    char line[MAX_LINE_LENGTH] = {0};
    struct Macro *macro_table = malloc(10 * sizeof(struct Macro));
    int macro_count = 0;
    FILE *as_file;
    FILE *am_file;
    struct Macro *macro = NULL;

    char *am_filename = strcat_with_malloc(bname, AM_FILE_EXT);
    char *as_filename = strcat_with_malloc(bname, AS_FILE_EXT);

    int i, j;

    output_macro_list = malloc(sizeof(struct Node) * MAX_LINE_LENGTH);

    as_file = fopen(as_filename, "r");
    am_file = fopen(am_filename, "w");

    if (!as_file || !am_file)
    {
        free(macro_table);
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, as_file))
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
    free(as_filename);

    return am_filename;
}
