#include "middle_common.h";
#include <string.h>;

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