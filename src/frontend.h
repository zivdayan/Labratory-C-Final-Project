#define MAX_LENGTH_DATA_TYPE 80
#define SYNTAX_ERROR_LENGTH 200
#include "global_consts.h"
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include <limits.h>
#include <stdio.h>
#include "error_msg.h"

#define SPACES " \t\v\f"

#define ERROR_CODE_44 44
#define MAX_LABEL_LENGTH 31
#define IMMEDIATE_ADDRESSING '0'
#define DIRECT_ADDRESSING '1'
#define INDEX_ADDRESSING '2'
#define REGISTER_ADDRESSING '3'

#define SRC_OPERAND 0
#define TARGET_OPERAND 1
#define TARGET_SINGLE_OPERAND 2

#define DEFAULT_INT INT_MAX

struct Instruction
{
    char *name;
    int opcode;
    const char *source;
    const char *dest;
};

struct string_sep_result
{
    char *strings[80];
    int strings_count;
};

struct ast
{
    char lineError[SYNTAX_ERROR_LENGTH];
    char labelName[MAX_LABEL_LENGTH];
    /* Define all possible directives */
    enum
    {
        ast_inst,
        ast_dir,
        ast_define,
        ast_empty
    } line_type;
    union
    {
        struct
        {
            char label[MAX_LABEL_LENGTH];
            int number;
        } define;
        struct
        {
            enum
            {
                ast_extern,
                ast_entry,
                ast_string,
                ast_data
            } dir_type;

            union
            {
                char label[MAX_LABEL_LENGTH];
                char string[MAX_LINE_LENGTH];
                struct
                {
                    int data_length;
                    struct
                    {
                        enum
                        {
                            data_label,
                            data_number
                        } data_type;
                        union
                        {
                            char label[MAX_LABEL_LENGTH];
                            int number;
                        } data_value;
                    } data[80];
                } data_array;
            } dir_options;

        } dir;

        /* Define all possible instructions */
        struct
        {
            enum
            {
                inst_mov = 0,
                inst_cmp = 1,
                inst_add = 2,
                inst_sub = 3,
                inst_not = 4,
                inst_clr = 5,
                inst_lea = 6,
                inst_inc = 7,
                inst_dec = 8,
                inst_jmp = 9,
                inst_bne = 10,
                inst_red = 11,
                inst_prn = 12,
                inst_jsr = 13,
                inst_rts = 14,
                inst_hlt = 15
            } inst_type;

            struct
            {
                enum
                {
                    addrs_none,
                    addrs_immed_const,
                    addrs_immed_label,
                    addrs_label,
                    adddrs_index_const,
                    adddrs_index_label,
                    addrs_register
                } addrs_mode;
                union
                {
                    int immed;
                    char label[MAX_LABEL_LENGTH];
                    int reg;
                    /* Define the index addressing case - Example: mov x[2], r2 */
                    struct
                    {
                        char label[MAX_LABEL_LENGTH];
                        union
                        {
                            int number;
                            char label[MAX_LABEL_LENGTH];
                        } index_option;
                    } index;
                } operand_options;
                enum
                {
                    none,
                    num,
                    label,
                    reg,
                    index_operand
                } operand_type;
                /*operands = {source-operand, target-operand}*/
                /*operands = {target-operand, NONE}*/
            } operands[2];
        } inst;
    } ast_options;
};

struct ast *get_ast_from_line(char *line, struct Node *macro_list);
static int line_contains_label_decleration(struct string_sep_result *ssr);
static int is_keyword(char *str, char *collection[], int length);
static int is_number(char *str, int max, int min);
static int is_instruction_line(struct string_sep_result ssr);
static int is_dir_line(struct string_sep_result ssr);
void string_sep(char *original_str, struct string_sep_result *ssr);
static int parse_inst_operand(char *operand, int operand_type, struct ast *ast, struct Instruction inst);
static int is_valid_label(char *str);
static char *remove_last_char(char *str);
static struct string_sep_result *strip_first_element(struct string_sep_result *ssr);