#include "frontend.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

static int line_contains_label_decleration(struct string_sep_result *ssr);
static int is_keyword(char *str, char *collection[], int length);
static int is_number(char *str, int max, int min);
static int is_instruction_line(struct string_sep_result ssr);
static int is_dir_line(struct string_sep_result ssr);
static int parse_inst_operand(char *operand, int operand_type, struct ast *ast, struct Instruction inst);
static int is_valid_label(char *str);
static char *remove_last_char(char *str);
static struct string_sep_result *strip_first_element(struct string_sep_result *ssr);

char *REGISTERS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char *DIRECTIVES[] = {".data", ".string", ".extern", ".entry"};

char *DEFINE = ".define";

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
    {".data", DIR_DATA},
    {".string", DIR_STRING},
    {".extern", DIR_EXTERN},
    {".entry", DIR_ENTRY}};

char *INSTRUCTIONS[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};

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
    int i;
    struct Directive default_dir = {0};

    for (i = 0; i < DIRECTIVES_LEN; i++)
    {
        if (!strcmp(directives[i].text_type, inst))
            return directives[i];
    }

    return default_dir;
}

static struct Instruction get_instruction_obj_by_opcode(int opcode)
{
    int i;
    struct Instruction default_dir = {0};

    for (i = 0; i < INSTRUCTIONS_LEN; i++)
    {
        if (opcode == inst_table[i].opcode)
            return inst_table[i];
    }

    return default_dir;
}

static struct Instruction get_instruction_obj_by_text(char *inst)
{
    int i;
    struct Instruction default_dir = {0};

    for (i = 0; i < INSTRUCTIONS_LEN; i++)
    {
        if (!strcmp(inst_table[i].name, inst))
            return inst_table[i];
    }

    return default_dir;
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
    return (is_reg || is_instruction || is_directive);
}

static int is_number(char *str, int max, int min)
{

    char *endstr;
    long result;
    int sign = 1;

    if (str == NULL || *str == '\0')
    {
        /** Handle NULL or empty string **/
        return 0;
    }

    if (*str == '+')
        sign = 1;
    if (*str == '-')
        sign = -1;

    result = strtol(str, &endstr, 10);
    result = result * sign;

    if (!((min <= result && result <= max) || (min == DEFAULT_INT || max == DEFAULT_INT)))
        return 0;

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

int split_label_and_index(char *operand, char **label, int *index, char **label_index)
{

    char *index_start = strpbrk(operand, "[");
    char *index_end = strpbrk(operand, "]");
    int is_index_number, is_index_label;
    *index = DEFAULT_INT;

    if (index_start == NULL || index_end == NULL)
        return 0;

    if (*(index_end + 1) != '\0')
        return 0;

    *(index_end) = '\0';

    is_index_number = is_number(index_start + 1, DEFAULT_INT, DEFAULT_INT);
    is_index_label = is_valid_label(index_start + 1);
    if (!(is_index_label || is_index_number))
        return 0;

    if (is_index_number)
    {
        *index = atoi((index_start + 1));
    }
    else
    {
        if (is_index_label)
            strcpy(*label_index, index_start + 1);
    }

    *index_start = '\0';
    if (!is_valid_label(operand))
        return 0;

    strcpy(*label, operand);
    return 1;
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
    int i;

    /* Make sure the string is 'wrapped' with quotes, we take in account that it may contains spaces/tabs/etc.
    Thus, it will be delimitered but considered within the string */
    if (!(suffix[0] == '"' && prefix[strlen(prefix) - 1] == '"'))
        return 0;
    for (i = 1; i < operand.strings_count - 1; i++)
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
    char tmp_str[MAX_LINE_LENGTH + 1];
    char *str_colon_removed;
    if (str == NULL)
    {
        return 0;
    }

    strcpy(tmp_str, str);

    /* Checks if the declartion ends with a colon (:) */
    if (!check_label_decl_suffix(tmp_str))
        return 0;

    str_colon_removed = remove_last_char(tmp_str);

    /*Checks if the label contains any saved keyword: opcode/instruction/register. */
    if (is_saved_keyword(str_colon_removed))
        return 0;

    /* Check if the label is a valid string - digits / alphabet characters. */
    if (!is_valid_label_string(str_colon_removed))
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
    if (is_saved_keyword(str))
        return 0;

    /* Check if the label is a valid string - digits / alphabet characters. */
    if (!is_valid_label_string(str))
        return 0;

    return 1;
}

static void parse_string_dir(char *line, struct ast *ast_ptr)
{
    char *start_of_string = strchr(line, '"');
    char *end_of_string = strrchr(line, '"');

    *(end_of_string) = '\0';

    strcpy(ast_ptr->ast_options.dir.dir_options.string, ++start_of_string);
}

static int parse_data_dir_operands(struct string_sep_result operands, struct ast *ast_ptr)
{
    int i;
    ast_ptr->ast_options.dir.dir_options.data_array.data_length = 0;
    for (i = 0; i < operands.strings_count; i++)
    {
        char *operand = operands.strings[i];
        int curr_array_length = ast_ptr->ast_options.dir.dir_options.data_array.data_length;

        if (i % 2 == 1)
        {
            if (strcmp(operand, ","))
                return 0;
            else
                continue;
        }

        if (is_number(operand, DEFAULT_INT, DEFAULT_INT))
        {
            ast_ptr->ast_options.dir.dir_options.data_array.data[curr_array_length].data_type = data_number;
            ast_ptr->ast_options.dir.dir_options.data_array.data[curr_array_length].data_value.number = atoi(operand);

            ast_ptr->ast_options.dir.dir_options.data_array.data_length++;
            continue;
        }
        if (is_valid_label(operand))
        {
            ast_ptr->ast_options.dir.dir_options.data_array.data[curr_array_length].data_type = data_label;
            strcpy(ast_ptr->ast_options.dir.dir_options.data_array.data[curr_array_length].data_value.label, operand);

            ast_ptr->ast_options.dir.dir_options.data_array.data_length++;
            continue;
        }

        return 0;
    }

    return 1;
}

static int check_operand_addressing_mode(int operand_type, struct Instruction inst, int addressing_mode)
{
    if (operand_type == SRC_OPERAND)
    {
        if (strchr(inst.source, addressing_mode) == NULL)
            return 0;
    }
    if (operand_type == TARGET_OPERAND)
    {
        if (strchr(inst.dest, addressing_mode) == NULL)
            return 0;
    }
    if (operand_type == TARGET_SINGLE_OPERAND)
    {
        if (strchr(inst.dest, addressing_mode) == NULL)
            return 0;
    }

    return 1;
}

static int parse_inst_operand(char *operand, int operand_type, struct ast *ast, struct Instruction inst)
{
    int number_index;
    int operand_type_index;
    char *label_index;
    char *index_label;
    char *immediate_operand = {0};

    operand_type_index = operand_type % 2;
    /* Direct register addressing */
    if (is_keyword(operand, REGISTERS, REG_LEN))
    {
        if (!check_operand_addressing_mode(operand_type, inst, REGISTER_ADDRESSING))
            return 0;

        ast->ast_options.inst.operands[operand_type_index].operand_type = reg;
        ast->ast_options.inst.operands[operand_type_index].operand_options.reg = atoi((operand + 1));
        ast->ast_options.inst.operands[operand_type_index].addrs_mode = addrs_register;
        return 1;
    }

    /* Immediate addressing */
    if (operand[0] == '#')
    {
        immediate_operand = operand + 1; /* Remove prefix hashtag */

        if (!check_operand_addressing_mode(operand_type, inst, IMMEDIATE_ADDRESSING))
            return 0;

        if (is_valid_label(immediate_operand))
        {
            ast->ast_options.inst.operands[operand_type_index].operand_type = label;
            strcpy(ast->ast_options.inst.operands[operand_type_index].operand_options.label, immediate_operand);
            ast->ast_options.inst.operands[operand_type_index].addrs_mode = addrs_immed_label;
            return 1;
        }

        if (is_number(immediate_operand, DEFAULT_INT, DEFAULT_INT))
        {
            ast->ast_options.inst.operands[operand_type_index].operand_type = num;
            ast->ast_options.inst.operands[operand_type_index].operand_options.immed = atoi(immediate_operand);
            ast->ast_options.inst.operands[operand_type_index].addrs_mode = addrs_immed_const;
            return 1;
        }
    }

    /* Index addressing */
    index_label = malloc(sizeof(char) * (MAX_LABEL_LENGTH + 1));
    label_index = malloc(sizeof(char) * (MAX_LABEL_LENGTH + 1));
    if (split_label_and_index(operand, &index_label, &number_index, &label_index))
    {
        if (!check_operand_addressing_mode(operand_type, inst, INDEX_ADDRESSING))
            return 0;

        ast->ast_options.inst.operands[operand_type_index].operand_type = index_operand;

        /* A number is valid label, so we'll check this beforehand */
        if (number_index != DEFAULT_INT)
        {
            ast->ast_options.inst.operands[operand_type_index].operand_options.index.index_option.number = number_index;
            ast->ast_options.inst.operands[operand_type_index].addrs_mode = adddrs_index_const;
        }
        else
        {
            if (label_index != NULL)
            {
                strcpy(ast->ast_options.inst.operands[operand_type_index].operand_options.index.index_option.label, label_index);
                ast->ast_options.inst.operands[operand_type_index].addrs_mode = adddrs_index_label;
            }
        }

        strcpy(ast->ast_options.inst.operands[operand_type_index].operand_options.index.label, index_label);

        return 1;
    }

    /* Direct addressing */
    if (is_valid_label(operand))
    {
        if (!check_operand_addressing_mode(operand_type, inst, DIRECT_ADDRESSING))
            return 0;

        ast->ast_options.inst.operands[operand_type_index].operand_type = label;
        strcpy(ast->ast_options.inst.operands[operand_type_index].operand_options.label, operand);
        ast->ast_options.inst.operands[operand_type_index].addrs_mode = addrs_label;
        return 1;
    }

    return 0;
}

static int parse_operands(struct string_sep_result operands, struct ast *ast_ptr)
{
    struct Instruction inst;
    char src_operand[MAX_LINE_LENGTH];
    char target_operand[MAX_LINE_LENGTH];

    switch (ast_ptr->line_type)
    {
    case ast_dir:
        switch (ast_ptr->ast_options.dir.dir_type)
        {
        case ast_data:
            parse_data_dir_operands(operands, ast_ptr);
            break;

        default:
            return 0;
            break;
        }
        break;

    case ast_inst:
        inst = get_instruction_obj_by_opcode((int)ast_ptr->ast_options.inst.inst_type);

        ast_ptr->ast_options.inst.operands[0].operand_type = none;
        ast_ptr->ast_options.inst.operands[1].operand_type = none;

        ast_ptr->ast_options.inst.operands[0].addrs_mode = addrs_none;
        ast_ptr->ast_options.inst.operands[1].addrs_mode = addrs_none;

        switch (ast_ptr->ast_options.inst.inst_type)
        {
        case inst_mov:
        case inst_cmp:
        case inst_add:
        case inst_sub:
        case inst_lea:
            if (operands.strings_count == 3)
            {
                strcpy(src_operand, operands.strings[0]);
                strcpy(target_operand, operands.strings[2]);
                parse_inst_operand(src_operand, SRC_OPERAND, ast_ptr, inst);
                parse_inst_operand(target_operand, TARGET_OPERAND, ast_ptr, inst);
                break;
            }
            else
            {
                strcpy(ast_ptr->lineError, SYNTAX_ERROR_INVALID_NUMBER_OPERANDS);
                return 0;
            }

        case inst_not:
        case inst_clr:
        case inst_inc:
        case inst_dec:
        case inst_jmp:
        case inst_bne:
        case inst_red:
        case inst_prn:
        case inst_jsr:
            if (operands.strings_count == 1)
            {
                strcpy(target_operand, operands.strings[0]);
                parse_inst_operand(target_operand, TARGET_SINGLE_OPERAND, ast_ptr, inst);
                ast_ptr->ast_options.inst.operands[1].operand_type = none;
                break;
            }
            else
            {
                strcpy(ast_ptr->lineError, SYNTAX_ERROR_INVALID_NUMBER_OPERANDS);
                return 0;
            }

        case inst_rts:
        case inst_hlt:
            if (operands.strings_count != 0)
            {
                strcpy(ast_ptr->lineError, SYNTAX_ERROR_INVALID_NUMBER_OPERANDS);
                return 0;
            }
            break;
        }
        break;

    case ast_define:
        strcpy(ast_ptr->ast_options.define.label, operands.strings[0]);
        ast_ptr->ast_options.define.number = atoi(operands.strings[2]);
        break;
    default:
        break;
    }

    return 1;
}

static int is_valid_line(char *line)
{
    return strlen(line) <= MAX_LINE_LENGTH;
}

static char *remove_last_char(char *str)
{
    char *tmp_label;
    if (str == NULL)
    {
        return 0;
    }

    tmp_label = str;
    tmp_label[strlen(str) - 1] = '\0';
    return tmp_label;
}

static int line_contains_label_decleration(struct string_sep_result *ssr)
{
    char *tmp_label = ssr->strings[0];
    int line_contains_label_dec;

    line_contains_label_dec = is_valid_label_declaration(tmp_label);
    return line_contains_label_dec;
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

    return !strcmp(initial_directive_keyword, DEFINE);
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
    char *variable;
    char *equals;
    char *value;

    if (3 != operands.strings_count)
        return 0;

    variable = operands.strings[0];
    equals = operands.strings[1];
    value = operands.strings[2];

    return (is_valid_label(variable) && '=' == *equals && is_number(value, DEFAULT_INT, DEFAULT_INT));
}

static int check_inst_commas_validity(struct string_sep_result operands)
{
    if (operands.strings_count == 0)
        return 1;
    if (operands.strings_count == 1)
    {
        if (*(operands.strings[0]) != ',')
            return 1;
        else
            return 0;
    }

    if (operands.strings_count != 3)
        return 0;

    if (*(operands.strings[0]) != ',' && *(operands.strings[1]) == ',' && *(operands.strings[2]) != ',')
        return 1;
    return 0;
}
static int check_data_commas_validity(struct string_sep_result operands)
{
    char *first_operand = operands.strings[0];
    char *last_operand = operands.strings[operands.strings_count - 1];
    char *left_operand;
    char *right_operand;
    int i;

    /* Check a case in which there is a comma at the start/end of operands
    Example:
     .data ,22,22,32
    OR
    .data 5,42,1,
    */

    if ((',' == first_operand[0]) || (',' == last_operand[strlen(last_operand) - 1]))
        return 0;

    for (i = 0; i < operands.strings_count - 1; i++)
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
    char original_line[MAX_LINE_LENGTH + 1] = {0};
    struct string_sep_result *ssr_ptr;
    struct string_sep_result *operands_ptr;

    struct string_sep_result ssr;
    struct string_sep_result operands;

    int error_occured;
    struct ast *ast_ptr = {0};
    int label_declared;

    ssr_ptr = malloc(sizeof(struct string_sep_result));
    operands_ptr = malloc(sizeof(struct string_sep_result));
    label = malloc(sizeof(char) * (MAX_LABEL_LENGTH + 1));

    ssr = *ssr_ptr;
    operands = *operands_ptr;

    ast_ptr = malloc(sizeof(struct ast));
    label_declared = 0;

    error_occured = 0;
    strcpy(ast_ptr->lineError, "");

    if (*line == '\n' || *line == ';')
        return ast_ptr;

    if (!is_valid_line(line))
    {
        strcpy(ast_ptr->lineError, SYNTAX_ERROR_LINE_TOO_LONG);
        error_occured = 1;
        goto invalid_syntax;
    }

    line[strcspn(line, "\r\n")] = 0; /* Mark end of line */

    /* The parsing is based on delimitered - space,tabs,etc. We need to make sure commas are
    space seperated in order to not treat them as one string.
    For example .data 3,2,3 would produce 3,2,3 as a single operand. Appending a space to the comma, helps
    us to parse every line correclty, without compromising the context/logic.  */

    char_sanitize(&line, ',');
    char_sanitize(&line, '=');

    strcpy(original_line, line);

    string_sep(line, &ssr);

    if (line_contains_label_decleration(&ssr))
    {
        label = remove_last_char(ssr.strings[0]);
        strcpy(ast_ptr->labelName, label);
        ssr_ptr = strip_first_element(&ssr); /* Truncated the "LABEL:" */
        ssr = *ssr_ptr;
        label_declared = 1;

        /* TODO: Warning if label is declared and this is .entry */
    }

    operands_ptr = strip_first_element(&ssr);
    operands = *operands_ptr;

    if (is_define_line(ssr))
    {
        if (label_declared)
        {
            strcpy(ast_ptr->lineError, SYNTAX_ERROR_LINE_TOO_LONG);
            error_occured = 1;
            goto invalid_syntax;
        }

        ast_ptr->line_type = ast_define;
        if (check_define_validity(operands))
        {
            if (!parse_operands(operands, ast_ptr))
            {
                error_occured = 1;
                goto invalid_syntax;
            }
        }
        else
        {
            error_occured = 1;
            goto invalid_syntax;
        }
    }

    if (is_instruction_line(ssr))
    {
        char *inst_type = ssr.strings[0];
        struct Instruction inst_type_obj = get_instruction_obj_by_text(inst_type);
        ast_ptr->line_type = ast_inst;
        ast_ptr->ast_options.inst.inst_type = inst_type_obj.opcode;
        if (check_inst_commas_validity(operands))
        {
            if (!parse_operands(operands, ast_ptr))
            {
                error_occured = 1;
                goto invalid_syntax;
            }
        }
        else
        {
            error_occured = 1;
            goto invalid_syntax;
        }
    }

    if (is_dir_line(ssr))
    {
        char *dir_type = ssr.strings[0];
        struct Directive dir_type_obj = get_directive_obj_by_text(dir_type);
        switch (dir_type_obj.enum_type)
        {
        case DIR_DATA:
            ast_ptr->line_type = ast_dir;
            ast_ptr->ast_options.dir.dir_type = ast_data;
            if (check_data_commas_validity(operands))
            {
                if (!parse_operands(operands, ast_ptr))
                {
                    error_occured = 1;
                    goto invalid_syntax;
                }
            }
            else
            {
                error_occured = 1;
                goto invalid_syntax;
            }

            break;

        case DIR_STRING:
            ast_ptr->line_type = ast_dir;
            ast_ptr->ast_options.dir.dir_type = ast_string;
            if (is_valid_string_operand(operands))
            {
                /* Since normal string delimiters could be a proper part of the string, we will not use
                the oeprands object, but the actual string line. */
                parse_string_dir(original_line, ast_ptr);
            }

            else
            {
                error_occured = 1;
                goto invalid_syntax;
            }

            break;

        case DIR_EXTERN:
        case DIR_ENTRY:
            if (is_valid_extern_or_entry(ssr))
            {
                if (is_valid_label(ssr.strings[1]))
                {
                    ast_ptr->line_type = ast_dir;

                    if (dir_type_obj.enum_type == DIR_ENTRY)
                        ast_ptr->ast_options.dir.dir_type = ast_entry;

                    if (dir_type_obj.enum_type == DIR_EXTERN)
                        ast_ptr->ast_options.dir.dir_type = ast_extern;

                    strcpy(ast_ptr->ast_options.dir.dir_options.label, ssr.strings[1]);
                }
            }
            else
            {
                error_occured = 1;
                goto invalid_syntax;
            }

            break;
        default:
            error_occured = 1;
            goto invalid_syntax;
            break;
        }
    }

invalid_syntax:
    if (error_occured)
        printf("Error has occured at: %s! \n %s \n\n", line, ast_ptr->lineError);

    return ast_ptr;
}

void string_sep(char *original_str, struct string_sep_result *ssr)
{
    int strings_count = 0;
    char *s;
    char str_arr[sizeof(char) * (MAX_LINE_LENGTH + 1)];
    char *str;
    char tmp_str[sizeof(char) * (MAX_LINE_LENGTH + 1)];
    int i;

    str = str_arr;
    strcpy(str, original_str);

    while (isspace(*str))
        str++;
    if (*str == '\0')
    {
        return;
    }

    do
    {
        ssr->strings[strings_count++] = str;
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

    for (i = 0; i < strings_count; i++)
    {
        strcpy(tmp_str, ssr->strings[i]);
        ssr->strings[i] = malloc(sizeof(char) * (MAX_LINE_LENGTH + 1));
        strcpy(ssr->strings[i], tmp_str);
    }

    ssr->strings_count = strings_count;
}

static struct string_sep_result *strip_first_element(struct string_sep_result *ssr)
{
    int i;
    struct string_sep_result *stripped_ssr = malloc(sizeof(struct string_sep_result));
    stripped_ssr->strings_count = ssr->strings_count - 1;
    for (i = 0; i < stripped_ssr->strings_count; i++)
        stripped_ssr->strings[i] = ssr->strings[i + 1];

    return stripped_ssr;
}