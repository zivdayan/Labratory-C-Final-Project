#include "utils.h"
#include "global_consts.h"

void char_sanitize(char **original_str, char c)
{   
    char* str = *original_str;
    char newstr[MAX_LINE_LENGTH+1];
    int i, j;

    newstr[0]='\0';

    i = 0;
    j = 0;
    for (i = 0; i < strlen(*original_str); i++) {
        if (str[i] == c) {
            newstr[j++] = ' ';  /* Add a space before the comma */ 
            newstr[j++] = c;  /* Add a comma */ 
            newstr[j++] = ' ';  /* Add a space after the comma */ 
        }
        else
        {
            *(newstr + j) = *(str + i);
            j++;
        }
    }
    newstr[j] = '\0';  /* Null-terminate the new string */ 

    strcpy(*original_str,newstr);
}

struct symbol *symbolLookUp(struct symbol *symbol_table, const int symbol_table_size, const char *name)
{
    int i;
    for (i = 0; i < symbol_table_size; i++)
    {
        if (strcmp(symbol_table[i].symName, name) == 0)
        {
            return &symbol_table[i];
        }
    }
    return NULL;
}

struct extr *extSearch(struct extr *externals, const int externals_size, const char *name)
{
    int i;
    for (i = 0; i < externals_size; i++)
    {
        if (strcmp(name, externals[i].externalName) == 0)
        {
            return &externals[i];
        }
    }
    return NULL;
}