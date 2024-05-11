#include "structs.h";
struct symbol *symbolLookUp(struct symbol *symbol_table, const int symbol_table_size, const char *name);
struct extr *extSearch(struct extr *externals, const int externals_size, const char *name);