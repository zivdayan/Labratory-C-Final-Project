#include "frontend.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>


#define SPACES " \t\v\f"

struct inst{
    char *name;
    const char *source;
    const char *dest;


};

struct inst inst_table[16] = {
    {"mov", 0, "0123"}
};

static int is_number(char* str, int max, int min, int* result, struct inst *inst)
{
    /*
    */
   return 0;

}

static int is_label(char* str)
{

}

static void parse_operand(char* operand, int operand_type, struct ast* ast)
{

}

static void parse_operands(char* operands,  struct ast* ast)
{

}
struct ast get_ast_from_line(char* line)
{
    struct ast ast = {0};


    return ast;

}



struct string_sep_result {
    char *strings[80];
    int strings_count;
};
struct string_sep_result string_sep(char * str) {
    int strings_count = 0;
    char *s;
    struct string_sep_result ssr = {0};
    while(isspace(*str)) str++;
    if(*str == '\0') {
        return ssr;
    }

    do {
        ssr.strings[strings_count++] = str;
        s = strpbrk(str,SPACES);
        if(s) {
            *s = '\0';
            s++;
            while(isspace(*s))s++;
            if(*s == '\0')
                break;
            str = s;    
        }else {
            break;
        }
    }while(1);
    ssr.strings_count = strings_count;
    return ssr;
}