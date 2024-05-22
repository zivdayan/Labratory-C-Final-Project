#include "utils.h"
#include "global_consts.h"
#include "frontend.h"
#include "structs.h"

char *strcat_with_malloc(const char *s1, const char *s2)
{
    size_t len_s1 = strlen(s1);
    size_t len_s2 = strlen(s2);
    size_t len_result = len_s1 + len_s2 + 1;
    char *result = (char *)malloc(len_result * sizeof(char));
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void char_sanitize(char **original_str, char c)
{
    char *str = *original_str;
    char newstr[MAX_LINE_LENGTH + 1];
    int i, j;

    newstr[0] = '\0';

    i = 0;
    j = 0;
    for (i = 0; i < strlen(*original_str); i++)
    {
        if (str[i] == c)
        {
            newstr[j++] = ' '; /* Add a space before the comma */
            newstr[j++] = c;   /* Add a comma */
            newstr[j++] = ' '; /* Add a space after the comma */
        }
        else
        {
            *(newstr + j) = *(str + i);
            j++;
        }
    }
    newstr[j] = '\0'; /* Null-terminate the new string */

    strcpy(*original_str, newstr);
}

struct symbol *search_symbol(struct symbol *symbol_table, const int symbol_table_size, const char *name)
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

struct extr *search_external(struct extr *externals, const int externals_size, const char *name)
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

int choose_addressing(int addrs_mode)
{

    switch (addrs_mode)
    {
    case addrs_immed_const:
    case addrs_immed_label:
        return addressing_immed;
    case addrs_label:
        return addressing_direct;
    case adddrs_index_label:
    case adddrs_index_const:
        return addressing_index;
    case addrs_register:
        return addressing_reg;

    default:
        return -1;
    }
    return -1;
}