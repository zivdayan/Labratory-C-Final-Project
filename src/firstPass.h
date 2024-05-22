#include <stdio.h>
#include "frontend.h"
#include "structs.h"

extern int first_pass(struct translation_unit *prog, const char *am_filename, FILE *am_file, struct Node *macro_list);
