#ifndef DEFINED_BACKEND_H
#define DEFINED_BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <stdio.h>  
#include <string.h>  


void print_ent_file(const char *bname, const struct translation_unit* program);
void print_ob_file(const char *bname, const struct translation_unit* program);
void print_ext_file(const char *bname, const struct translation_unit* program);


#endif