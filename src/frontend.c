#include "frontend.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>


#define SPACES " \t\v\f"

#define ERROR_CODE_44 44
#define MAX_LABEL_LENGTH 31

char *REGISTERS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char *INSTRUCTIONS[] = {".data", ".string", ".extern", ".entry"};

char *OPCODES[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
struct inst{
    char *name;
    int opcode;
    const char *source;
    const char *dest;
};

int syntax_error_line_too_long(char *file_name)
{
    return 0;
}

struct inst inst_table[16] = {
    {"mov", 0, "0123", "123"},
    {"cmp", 1, "0123", "0123"},
    {"add", 2, "0123", "123"},
    {"sub", 3, "0123", "123"},
    {"not", 4, "", "123"},
    {"clr", 5, "", "123"},
    {"lea", 6, "12", "123"},
    {"inc", 7, "12", "123"},
    {"dec", 8, "", "123"},
    {"jmp", 9, "", "13"},
    {"bne", 10, "", "13"},
    {"red", 11, "", "123"},
    {"prn", 12, "", "0123"},
    {"jsr", 13, "", "13"},
    {"rts", 14, "", ""},
    {"hlt", 15, "", ""}
};


static int is_basic_valid_string_label(char* str)
{
    return strlen(str) > MAX_LABEL_LENGTH || !isalpha(*str);
}


/**
 * @brief Check if a label/symbol is a saved word: such as registers, instruction.
 * 
 * @param str 
 * @return int 
 */
static int is_saved_keyword(char* str)
{
    int is_reg = is_keyword(str,REGISTERS, REG_LEN);
    int is_opcode = is_keyword(str,OPCODES, OPCODES_LEN);
    int is_instruction = is_keyword(str,INSTRUCTIONS, INSTRUCTIONS_LEN);
    return (is_reg || is_opcode || is_instruction);
}



static int is_number(char *str, int max, int min, int * result)
{

    char *endstr;

    if (str == NULL || *str == '\0') {
        /** Handle NULL or empty string **/ 
        return 0;
    }

    if(!(*str == '+' || *str == '-'))
        return 0;

    strtol(str, &endstr, 10);

    if(*endstr == '\0')
        return 1;

    return 0;
}

/**
 * @brief  The function checks if a string a actually a register.
 * 
 * @param str 
 * @param int* reg_index returns as a result, which register is it (in case of success).
 * @return int  1 - success, the string is a registry, 0 - otherwise.
 */

int is_keyword(char *str, char *collection[], int length)
{
    int i;

    if (str == NULL) {
        return 0;
    }

    for (i = 0; i < length; i++) {
        if ( 0 == strcmp(str, collection[i])) {
            return 1;
        }
    }
    return 0;    
}

static int is_valid_label_string(char *str)
{
    if (strlen(str) > MAX_LABEL_LENGTH)
        return 0;

    while (*str) {
        if (!isalpha(*str) && !isdigit(*str)) {
            return 0; /* Return false if character is not a digit or alphabet */ 
        }
        str++;
    }
    return 1; /* Return true if all characters passed the check */ 
}

static int check_label_decl_suffix(char *str)
{
    int len;
    if (str == NULL || *str == '\0') {
        /* Handle NULL or empty string */ 
        return 0;
    }

    len = strlen(str); /* Get the length of the string */ 
    return str[len - 1] == ':'; /* Check if the last character is a colon */ 

}


/* TODO: add a check - label cannot be macro/const */ 


static int is_valid_label_declaration(char* str)
{
    if (str == NULL) {
        return 0;
    }

    /*Checks if the label contains any saved keyword: opcode/instruction/register. */ 
    if(!is_saved_keyword(str))
         return 0;

    /* Check if the label is a valid string - digits / alphabet characters. */ 
    if(!is_valid_label_string(str))
        return 0;

    /* Checks if the declartion ends with a colon (:) */ 
    if(!check_label_decl_suffix(str))
        return 0;

    return 1;
}

/* TODO: add a check - label cannot be macro/const */ 
static int is_valid_label(char* str)
{
    if (str == NULL) {
        return 0;
    }

    /* Checks if the label contains any saved keyword: opcode/instruction/register. */
    if(!is_saved_keyword(str))
         return 0;

    /* Check if the label is a valid string - digits / alphabet characters. */ 
    if(!is_valid_label_string(str))
        return 0;

    return 0;

}


static void parse_operand(char* operand, int operand_type, struct ast* ast)
{

}

static void parse_operands(char* operands,  struct ast* ast)
{

}

static int is_valid_line(char *line)
{
    return strlen(line) > MAX_LINE_LENGTH;
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
int main() {
    struct string_sep_result ssr;
    char line[81];
    FILE * f;
    int line_count =1;
    int i;
    f = fopen("ps.as","r");
    while(fgets(line,sizeof(line),f)) {
        line[strcspn(line, "\r\n")] = 0;
        ssr = string_sep(line);

        printf("line%d:\n",line_count);
        for(i=0;i<ssr.strings_count;i++) {
            printf("\tstr[%d]='%s'\n",i,ssr.strings[i]);
        }
        line_count++;
    }
    fclose(f);
    return 0;
}