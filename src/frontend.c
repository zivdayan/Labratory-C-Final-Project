#include "frontend.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

#define SPACES " \t\v\f"

#define ERROR_CODE_44 44
#define MAX_LABEL_LENGTH 31

char *REGISTERS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char *DIRECTIVES[] = {".data", ".string", ".extern", ".entry"};

char *DEFINE=".define";

struct Directive
{
    char *text_type;
    enum DirectiveType
    {
        DIR_DATA,
        DIR_STRING,
        DIR_EXTERN,
        DIR_ENTRY
    } enum_type;
};


struct Directive directives[] = {
    [0] = {.text_type = '.data', .enum_type = DIR_DATA},
    [1] = {.text_type = '.string', .enum_type = DIR_STRING},
    [2] = {.text_type = '.extern', .enum_type = DIR_EXTERN},
    [3] = {.text_type = '.entry', .enum_type = DIR_ENTRY}};

char *INSTRUCTIONS[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};
struct Instruction
{
    char *name;
    int opcode;
    const char *source;
    const char *dest;
};

int syntax_error_line_too_long(char *file_name)
{
    return 0;
}

struct Instruction inst_table[16] = {
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
    {"hlt", 15, "", ""}};

static struct Directive get_directive_obj_by_text(char *inst)
{
    struct Directive default_dir = {0};

    for (int i = 0; i < DIRECTIVES_LEN; i++)
    {
        if (inst == directives[i].text_type)
            return directives[i];
    }

    return default_dir;
}

static struct Instruction get_instruction_obj_by_opcode(int opcode)
{
    struct Instruction default_dir = {0};

    for (int i = 0; i < INSTRUCTIONS_LEN; i++)
    {
        if (opcode == inst_table[i].opcode)
            return inst_table[i];
    }

    return default_dir;
}

static struct Instruction get_instruction_obj_by_text(char *inst)
{
    struct Instruction default_dir = {0};

    for (int i = 0; i < INSTRUCTIONS_LEN; i++)
    {
        if (inst == inst_table[i].name)
            return inst_table[i];
    }

    return default_dir;
}


static int is_basic_valid_string_label(char *str)
{
    return strlen(str) > MAX_LABEL_LENGTH || !isalpha(*str);
}

/**
 * @brief Check if a label/symbol is a saved word: such as registers, instruction.
 *
 * @param str
 * @return int
 */
static int is_saved_keyword(char *str)
{
    int is_reg = is_keyword(str, REGISTERS, REG_LEN);
    int is_instruction = is_keyword(str, INSTRUCTIONS, INSTRUCTIONS_LEN);
    int is_directive = is_keyword(str, DIRECTIVES, DIRECTIVES_LEN);
    return (is_reg || is_instruction || is_instruction);
}

static int is_number(char *str, int max, int min)
{

    char *endstr;
    long result;

    if (str == NULL || *str == '\0')
    {
        /** Handle NULL or empty string **/
        return 0;
    }

    if (!(*str == '+' || *str == '-'))
        return 0;

    result = strtol(str, &endstr, 10);

    if(!( (min<=result && result<=max) || (min==NULL || max==NULL) ))

    if (*endstr == '\0')
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

    if (str == NULL)
    {
        return 0;
    }

    for (i = 0; i < length; i++)
    {
        if (0 == strcmp(str, collection[i]))
        {
            return 1;
        }
    }
    return 0;
}

static int is_valid_label_string(char *str)
{
    if (strlen(str) > MAX_LABEL_LENGTH)
        return 0;

    while (*str)
    {
        if (!isalpha(*str) && !isdigit(*str))
        {
            return 0; /* Return false if character is not a digit or alphabet */
        }
        str++;
    }
    return 1; /* Return true if all characters passed the check */
}

static int check_label_decl_suffix(char *str)
{
    int len;
    if (str == NULL || *str == '\0')
    {
        /* Handle NULL or empty string */
        return 0;
    }

    len = strlen(str);          /* Get the length of the string */
    return str[len - 1] == ':'; /* Check if the last character is a colon */
}

static int is_valid_string_operand(struct string_sep_result operand)
{
    char *suffix = operand.strings[0];
    char *prefix = operand.strings[operand.strings_count - 1];
    char *str;

    /* Make sure the string is 'wrapped' with quotes, we take in account that it may contains spaces/tabs/etc.
    Thus, it will be delimitered but considered within the string */
    if (!(suffix[0] == '"' && prefix[strlen(prefix) - 1] == '"'))
        return 0;
    for (int i = 0; i < operand.strings_count; i++)
    {
        str = operand.strings[i];
        while (*str)
        {
            /* A string must contains valid ASCII chars */
            if (!isalpha(*str))
            {
                return 0; /* Return false if character is not a digit or alphabet */
            }
            str++;
        }
    }

    return 1;
}

/* TODO: add a check - label cannot be macro/const */

static int is_valid_label_declaration(char *str)
{
    if (str == NULL)
    {
        return 0;
    }

    /*Checks if the label contains any saved keyword: opcode/instruction/register. */
    if (!is_saved_keyword(str))
        return 0;

    /* Check if the label is a valid string - digits / alphabet characters. */
    if (!is_valid_label_string(str))
        return 0;

    /* Checks if the declartion ends with a colon (:) */
    if (!check_label_decl_suffix(str))
        return 0;

    return 1;
}

/* TODO: add a check - label cannot be macro/const */
static int is_valid_label(char *str)
{
    if (str == NULL)
    {
        return 0;
    }

    /* Checks if the label contains any saved keyword: opcode/instruction/register. */
    if (!is_saved_keyword(str))
        return 0;

    /* Check if the label is a valid string - digits / alphabet characters. */
    if (!is_valid_label_string(str))
        return 0;

    return 0;
}


static void parse_string_dir(char* line, struct ast *ast_ptr)
{
    struct ast ast = *ast_ptr;
    char* start_of_string = strchr(line,'"');
    char* end_of_string = strrchr(line, '"');

    *(end_of_string)='\0';

    strcpy(ast.ast_options.dir.dir_options.string, ++start_of_string);
    
}

static int parse_data_dir_operands(struct string_sep_result operands, struct ast *ast_ptr)
{
    struct ast ast = *ast_ptr;
    ast.ast_options.dir.dir_options.data_array.data_length=0;
    for (int i = 0; i < operands.strings_count; i++)
    {
        char* operand = operands.strings[i];
        int curr_array_length = ast.ast_options.dir.dir_options.data_array.data_length;

       if(i%2==1)
        {
            if(operand != ",")
                return 0;
            else
                continue;
        }
        
        
        if(is_number(operand, NULL, NULL))
        {
            ast.ast_options.dir.dir_options.data_array.data[curr_array_length].data_type = data_number;
            ast.ast_options.dir.dir_options.data_array.data[curr_array_length].data_value.number = atoi(operand);
        }
        if(is_valid_label(operand))
        {
            ast.ast_options.dir.dir_options.data_array.data[curr_array_length].data_type=data_label;
            strcpy(ast.ast_options.dir.dir_options.data_array.data[curr_array_length].data_value.label,operand);
        }
        
        ast.ast_options.dir.dir_options.data_array.data_length++;
            
    }
    
}

static int parse_inst_operand(char *operand, int operand_type, struct ast *ast, struct Instruction inst)
{
    /* Immediate addressing */
    if("#" == operand[0] && operand_type==SRC_OPERAND)
    {
        operand++;        
        ast->ast_options.inst.operands[operand_type].addrs_mode = addrs_immed;

        if(strchr(inst.source, IMMEDIATE_ADDRESSING) == NULL) 
                return 1;

        if(is_valid_label(operand))
        {
            ast->ast_options.inst.operands[operand_type].operand_type=label;
            strcpy(ast->ast_options.inst.operands[operand_type].operand_options.label,operand);
        }

        if(is_number(operand, NULL, NULL))
        {
            ast->ast_options.inst.operands[operand_type].operand_type=num;
            strcpy(ast->ast_options.inst.operands[operand_type].operand_options.immed,operand);   
        }
        
    }
        
}







static void parse_operands(struct string_sep_result operands, struct ast *ast_ptr)
{
    struct ast ast = *ast_ptr;
    switch (ast.line_type)
    {
        case ast_dir:
            switch (ast.ast_options.dir.dir_type)
            {
                case ast_data:
                    parse_data_dir_operands(operands,ast_ptr);
                    break;

                default:
                    break;
            }
            break;

        case ast_inst:
            struct Instruction inst = get_instruction_obj_by_opcode((int)ast.ast_options.inst.inst_type);

            char *src_operand;
            char *target_operand;
            
            ast.ast_options.inst.operands[0].operand_type = none;
            ast.ast_options.inst.operands[1].operand_type = none;

            ast.ast_options.inst.operands[0].addrs_mode = addrs_none;
            ast.ast_options.inst.operands[1].addrs_mode = addrs_none;
            
            switch(ast.ast_options.inst.inst_type)
            {
                case inst_mov:
                case inst_cmp:
                case inst_add:
                case inst_sub:
                case inst_lea:

                    strcpy(src_operand,operands.strings[0]);
                    strcpy(target_operand,operands.strings[2]);
                    parse_inst_operand(src_operand,SRC_OPERAND,ast_ptr,inst);
                    parse_inst_operand(target_operand,TARGET_OPERAND,ast_ptr,inst);
                
                case inst_not:
                case inst_clr:
                case inst_inc:
                case inst_dec:
                case inst_jmp:
                case inst_bne:              
                case inst_red:
                case inst_prn:
                case inst_jsr:
                    strcpy(target_operand,operands.strings[0]);
                    parse_inst_operand(target_operand,TARGET_OPERAND,ast_ptr,inst);
                    ast.ast_options.inst.operands[1].operand_type = none;
                    break;

                case inst_rts:
                case inst_hlt:
                    break;
            }
            break;

        case ast_define:
            strcpy(ast.ast_options.define.label, operands.strings[0]);
            ast.ast_options.define.number=operands.strings[2];
            break;
        default:    
            break;
    }
}

static int is_valid_line(char *line)
{
    return strlen(line) > MAX_LINE_LENGTH;
}

static char *remove_last_char(char *str)
{
    if (str == NULL)
    {
        return 0;
    }

    char *tmp_label;
    tmp_label = str;
    tmp_label[strlen(str) - 1] = '\0';
    return tmp_label;
}

static int line_contains_label_decleration(struct string_sep_result ssr, char **label)
{
    char *tmp_label = ssr.strings[0];
    int line_contains_label_dec = is_valid_label_declaration(tmp_label);
    if (line_contains_label_dec)
        *label = remove_last_char(tmp_label); /* Removed the colon from "LABEL:" */

    return is_valid_label_declaration(tmp_label);
}

static int is_dir_line(struct string_sep_result ssr)
{
    char *initial_directive_keyword;
    initial_directive_keyword = ssr.strings[0];

    return is_keyword(initial_directive_keyword, DIRECTIVES, DIRECTIVES_LEN);
}

static int is_define_line(struct string_sep_result ssr)
{
    char *initial_directive_keyword;
    initial_directive_keyword = ssr.strings[0];

    return DEFINE == initial_directive_keyword;


}

static int is_instruction_line(struct string_sep_result ssr)
{
    char *initial_directive_keyword;
    initial_directive_keyword = ssr.strings[0];

    return is_keyword(initial_directive_keyword, INSTRUCTIONS, INSTRUCTIONS_LEN);
}

static int is_valid_extern_or_entry(struct string_sep_result ssr)
{
    return (ssr.strings_count == 2) && (is_valid_label(ssr.strings[1]));
}

static int check_define_validity(struct string_sep_result operands)
{
    if(3 != operands.strings_count)
        return 0;

    char *variable = operands.strings[0];
    char *equals = operands.strings[1];
    char *value = operands.strings[2];

    return (is_valid_label(variable) && "=" == equals && is_number(value, NULL, NULL));

}

static int check_inst_commas_validity(struct string_sep_result operands)
{
    if(operands.strings_count == 1)
        if(operands.strings[0] != ',')
            return 1;
        else
            return 0;


    if(operands.strings_count!=3)
        return 0;

    if (operands.strings[0] != "," && operands.strings[1]=="," && operands.strings[2]!=',')
        return 1;
    return 0;
}
static int check_data_commas_validity(struct string_sep_result operands)
{
    char *first_operand = operands.strings[0];
    char *last_operand = operands.strings[operands.strings_count - 1];
    char *left_operand;
    char *right_operand;

    /* Check a case in which there is a comma at the start/end of operands
    Example:
     .data ,22,22,32
    OR
    .data 5,42,1,
    */

    if ((',' == first_operand[0]) || (',' == last_operand[strlen(last_operand) - 1]))
        return 0;

    for (int i = 0; i < operands.strings_count - 1; i++)
    {
        left_operand = operands.strings[i];
        right_operand = operands.strings[i + 1];

        /* This statement makes sure that between each operand (space/tab/etc delimitered) there is exactly ONE comma seperating */

        if (!((left_operand[strlen(left_operand) - 1] == ',' && right_operand[0] != ',') || (left_operand[strlen(left_operand) - 1] != ',' && right_operand[0] == ',')))
            return 0;
    }

    return 1;
}

/**
 * @brief Get the Abstract syntax tree from an assembly line.
 *
 * @param line  - a string represents an assembly line of code.
 * @return a point to a struct ast object - parsed representation of the given line
 */
struct ast *get_ast_from_line(char *line, struct Node *macro_list)
{
    char *label;
    char *original_line;
    int is_extern, is_entry;
    struct string_sep_result ssr;
    struct string_sep_result operands;

    struct ast *ast_ptr = malloc(sizeof(struct ast));
    struct ast ast = *ast_ptr;
    int label_declared=0;

    if (line == '\n' || line == ';')
        return ast_ptr;

    /* The parsing is based on delimitered - space,tabs,etc. We need to make sure commas are 
    space seperated in order to not treat them as one string.
    For example .data 3,2,3 would produce 3,2,3 as a single operand. Appending a space to the comma, helps
    us to parse every line correclty, without compromising the context/logic.  */
    char_sanitize(&line, ",");
    char_sanitize(&line, "=");

    strcpy(original_line,line);

    line[strcspn(line, "\r\n")] = 0; /* Mark end of line */

    ssr = string_sep(line);
    int error_code = 0;

    if (line_contains_label_decleration(ssr, &label))
    {
        strcpy(ast.labelName,label);
        ssr = strip_first_element(ssr); /* Truncated the "LABEL:" */
        operands = strip_first_element(ssr);
        label_declared=1;
    }

    if(is_define_line(ssr))
    {
        if(label_declared)
            goto invalid_syntax;
        ast.line_type = ast_define;
        if(check_define_validity(operands))
            parse_operands(operands,ast_ptr);
        else
            goto invalid_syntax;
    }

    if(is_instruction_line(ssr))
    {   
        char *inst_type = ssr.strings[0];
        struct Instruction inst_type_obj = get_instruction_obj_by_text(inst_type);
        ast.line_type = ast_inst;
        ast.ast_options.inst.inst_type=inst_type_obj.opcode;
        if(check_inst_commas_validity(operands))
            parse_operands(operands,ast_ptr);
        else
            goto invalid_syntax;
    }

    if (is_dir_line(ssr))
    {
        char *dir_type = ssr.strings[0];
        struct Directive dir_type_obj = get_directive_obj_by_text(dir_type);
        switch (dir_type_obj.enum_type)
        {
        case DIR_DATA:
            ast.line_type = ast_dir;
            ast.ast_options.dir.dir_type = ast_data;
            if (check_data_commas_validity(operands))
                parse_operands(operands, ast_ptr);
            else
                goto invalid_syntax;
            break;

        case DIR_STRING:
            ast.line_type = ast_dir;
            ast.ast_options.dir.dir_type = ast_string;
            if (is_valid_string_operand(operands))
            {
                /* Since normal string delimiters could be a proper part of the string, we will not use
                the oeprands object, but the actual string line. */
                parse_string_dir(original_line, ast_ptr);
            }
                
            else
                goto invalid_syntax;
            break;

        case DIR_EXTERN:
        case DIR_ENTRY:
            if (is_valid_extern_or_entry(ssr))
            {
                if (is_valid_label(ssr.strings[1]))
                {
                    ast.line_type = ast_dir;

                    if (dir_type_obj.enum_type == DIR_ENTRY)
                        ast.ast_options.dir.dir_type = ast_entry;

                    if (dir_type_obj.enum_type == DIR_EXTERN)
                        ast.ast_options.dir.dir_type = ast_extern;

                    ast.ast_options.dir.dir_options.label = label;
                }
            }
            else
                goto invalid_syntax;
            break;
        default:
            goto invalid_syntax;
            break;
        }

    invalid_syntax:
        free(ast_ptr);
        goto cleanup;

    cleanup:
        free(ast_ptr);
        return ast_ptr;
    }
}

static struct string_sep_result string_sep(char *str)
{
    int strings_count = 0;
    char *s;
    struct string_sep_result ssr = {0};
    while (isspace(*str))
        str++;
    if (*str == '\0')
    {
        return ssr;
    }

    do
    {
        ssr.strings[strings_count++] = str;
        s = strpbrk(str, SPACES);
        if (s)
        {
            *s = '\0';
            s++;
            while (isspace(*s))
                s++;
            if (*s == '\0')
                break;
            str = s;
        }
        else
        {
            break;
        }
    } while (1);
    ssr.strings_count = strings_count;

    return ssr;
}

static struct string_sep_result strip_first_element(struct string_sep_result ssr)
{
    struct string_sep_result stripped_ssr = {0};
    stripped_ssr.strings_count = ssr.strings_count - 1;
    for (int i = 0; i < stripped_ssr.strings_count; i++)
        stripped_ssr.strings[i] = ssr.strings[i + 1];

    return stripped_ssr;
}
