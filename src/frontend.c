#include "frontend.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>


#define SPACES " \t\v\f"

#define ERROR_CODE_44 44
#define MAX_LABEL_LENGTH 31

char *REGISTERS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char *DIRECTIVES[] = {".data", ".string", ".extern", ".entry"};

struct Directive {
    char *text_type;
    enum DirectiveType {
    DIR_DATA,
    DIR_STRING,
    DIR_EXTERN,
    DIR_ENTRY
    } enum_type;  
};

struct Directive directives[]={
    [0] = {.text_type = '.data', .enum_type = DIR_DATA},
    [1] = {.text_type = '.string', .enum_type = DIR_STRING},
    [2] = {.text_type = '.extern', .enum_type = DIR_EXTERN},
    [3] = {.text_type = '.entry', .enum_type = DIR_ENTRY}
};

char *INSTRUCTIONS[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
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


static struct Directive get_directive_obj_by_text(char* inst)
{
    struct Directive default_dir = {0};

    for (int i = 0; i < DIRECTIVES_LEN; i++)
    {
        if(inst == directives[i].text_type)
            return directives[i];
    }
    
    return default_dir;

}

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
    int is_instruction = is_keyword(str,INSTRUCTIONS, INSTRUCTIONS_LEN);
    int is_directive = is_keyword(str,DIRECTIVES, DIRECTIVES_LEN);
    return (is_reg || is_instruction || is_instruction);
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


static void parse_operand(char* operand, int operand_type, struct ast* ast, struct inst * inst)
{

}

static void parse_operands(char* operands,  struct ast* ast)
{

}

static int is_valid_line(char *line)
{
    return strlen(line) > MAX_LINE_LENGTH;
}

static int is_dir_line(char* line, struct string_sep_result ssr)
{
    line[strcspn(line, "\r\n")] = 0; /* Mark end of line */
    return is_keyword(ssr.strings[0],DIRECTIVES, DIRECTIVES_LEN);
}

static int is_valid_extern_or_entry(struct string_sep_result ssr)
{
    return (ssr.strings_count == 2) && (is_valid_label(ssr.strings[1]));
}


struct ast get_ast_from_line(char* line)
{
    char *label;
    int is_extern, is_entry;
    struct ast ast = {0};
    if (line == '\n' || line == ';')
        return ast;
    
    struct string_sep_result ssr = string_sep(line);
    int error_code = 0;
    
    if(is_dir_line(line, ssr))
    {
        char dir_type = ssr.strings[0];
        struct Directive dir_type_obj =  get_directive_obj_by_text(dir_type);
        switch(dir_type_obj.enum_type)
        {
            case DIR_DATA:
                ;

            case DIR_STRING:
                ;

            case DIR_EXTERN:
            case DIR_ENTRY:
                if(is_valid_extern_or_entry(ssr))
                {
                    *label = ssr.strings[0];
                    if(is_valid_label(label))
                    {
                        ast.line_type=ast_inst;
                        ast.line_type=ast_dir;
                        
                        if (dir_type_obj.enum_type==DIR_ENTRY)
                            ast.ast_options.dir.dir_type=ast_entry;
                        if (dir_type_obj.enum_type==DIR_EXTERN)
                            ast.ast_options.dir.dir_type=ast_extern;

                        ast.ast_options.dir.dir_options.label=label;
                        return ast;
                    }
                };
            default:
                ;
        }



    }

        



    return ast;

}


static struct string_sep_result string_sep(char * str) {
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