#define MAX_LENGTH_DATA_TYPE 80
#define SYNTAX_ERROR_LENGTH 200
#include "global_consts.h"
#include <stdlib.h>
#include <string.h>
#include "structs.h"

struct string_sep_result
{
    char *strings[80];
    int strings_count;
};

struct ast
{
    char lineError[SYNTAX_ERROR_LENGTH];
    const char *labelName;
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
            const char *label;
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
                char *label;
                char *string;
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
                            char *label;
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
                    addrs_immed,
                    addrs_label,
                    adddrs_index_const,
                    adddrs_index_label,
                    addrs_register
                } addrs_mode;
                union
                {
                    int *immed;
                    char *label;
                    int reg;
                    /* Define the index addressing case - Example: mov x[2], r2 */
                    struct
                    {
                        char *label;
                        union
                        {
                            int number;
                            char *label;
                        } index_option;
                    } index;
                } operand_options;
                /*operands = {source-operand, target-operand}*/
            } operands[2];
        } inst;
    } ast_options;
};

struct ast *get_ast_from_line(char *line, struct Node *macro_list);

int is_keyword(char *str, char *collection[], int length);
static int is_number(char *str, int max, int min);
static int is_instruction_line(struct string_sep_result ssr);
static int is_dir_line(struct string_sep_result ssr);
static void parse_inst_operand(char *operand, int operand_type, struct ast *ast, struct inst *inst);
