#include <string.h>
#include <stdio.h>
#include "structs.h"

void char_sanitize(char **original_str, char c);
struct symbol *serach_symbol(struct symbol *symbol_table, const int symbol_table_size, const char *name);
struct extr *search_external(struct extr *externals, const int externals_size, const char *name);
int choose_addressing(int addrs_mode);
