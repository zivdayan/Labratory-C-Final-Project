#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define AM_FILE_EXT ".am"
#define AS_FILE_EXT ".as"

void split_by_first_space(char *str, char **item1, char **item2);
char *preproc(char *bname, struct Node *output_macro_list);
