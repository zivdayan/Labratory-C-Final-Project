#include <stdio.h>
#include "frontend.h"
#include "structs.h"

extern int firstPass(struct tranlsation_unit *prog, const char * amFileName, FILE * amFile, struct Node *macro_list);