#include "firstPass.h"
#include "frontend.h"
#include "structs.h"
#include <string.h>
#include "utils.h"

/* Todo change names and errors strings and move them to error_msg.h */

int first_pass(struct translation_unit *prog, const char *am_filename, FILE *am_file, struct Node *macro_list)
{
    char line[81] = {0};
    int ic = 100, dc = 0;
    int line_counter = 1;
    int is_error = 0;
    int i;
    int j;

    struct ast line_struct = {0};
    struct symbol *symbol;

    while (fgets(line, sizeof(line), am_file))
    {
        line_struct = *get_ast_from_line(line, macro_list);
        if (line_struct.lineError[0] != '\0')
        {
            printf("%s:%d:syntax error: %s", am_filename, line_counter, line_struct.lineError);
            line_counter++;
            is_error = 1;
            continue;
        }
        if (line_struct.labelName[0] != '\0' && (line_struct.line_type == ast_inst || (line_struct.line_type == ast_dir && (line_struct.ast_options.dir.dir_type == ast_data || line_struct.ast_options.dir.dir_type == ast_string))))
        {
            symbol = serach_symbol(prog->symbol_table, prog->symCount, line_struct.labelName);
            if (symbol)
            {
                if (symbol->symType == symEntry)
                {
                    symbol->symType = line_struct.line_type == ast_inst ? symEntryCode : symEntryData;
                    symbol->address = line_struct.line_type == ast_inst ? ic : dc;
                }
                else
                {
                    printf("%s: Error in line %d, redefination of symbol '%s'\n", am_filename, line_counter, line_struct.labelName);
                    is_error = 1;
                }
            }
            else
            {
                strcpy(prog->symbol_table[prog->symCount].symName, line_struct.labelName);
                prog->symbol_table[prog->symCount].symType = line_struct.line_type == ast_inst ? symCode : symData;
                prog->symbol_table[prog->symCount].address = line_struct.line_type == ast_inst ? ic : dc;
                prog->symCount++;
            }
        }
        if (line_struct.line_type == ast_inst)
        {
            ic++;
            if (line_struct.ast_options.inst.operands[0].operand_type == reg && line_struct.ast_options.inst.operands[1].operand_type == reg)
            {
                ic++;
            }
            else
            {
                ic += (line_struct.ast_options.inst.operands[0].operand_type == index_operand) + (line_struct.ast_options.inst.operands[1].operand_type == index_operand);
                ic += (line_struct.ast_options.inst.operands[0].operand_type >= num) + (line_struct.ast_options.inst.operands[1].operand_type >= num);
            }
        }
        else if (line_struct.line_type == ast_dir && line_struct.ast_options.dir.dir_type == ast_data)
        {
            for (j = 0; j < line_struct.ast_options.dir.dir_options.data_array.data_length; j++)
            {
                if (line_struct.ast_options.dir.dir_options.data_array.data[j].data_type == data_number)
                    memcpy(&prog->data_image[(prog->DC) + j], &line_struct.ast_options.dir.dir_options.data_array.data[j].data_value.number, sizeof(int));
                else if (line_struct.ast_options.dir.dir_options.data_array.data[j].data_type == data_label)
                {
                    symbol = serach_symbol(prog->symbol_table, prog->symCount, line_struct.ast_options.dir.dir_options.data_array.data[j].data_value.label);
                    memcpy(&prog->data_image[(prog->DC) + j], &symbol->address, sizeof(int));
                }
            }
            dc += line_struct.ast_options.dir.dir_options.data_array.data_length;
            prog->DC = dc;
        }
        else if (line_struct.line_type == ast_dir && line_struct.ast_options.dir.dir_type == ast_string)
        {
            for (j = 0; j < strlen(line_struct.ast_options.dir.dir_options.string); j++)
            {
                memcpy(&prog->data_image[(prog->DC) + j], line_struct.ast_options.dir.dir_options.string + j, 1);
            }
            memset(&prog->data_image[(prog->DC) + strlen(line_struct.ast_options.dir.dir_options.string)], 0, 1);

            dc += strlen(line_struct.ast_options.dir.dir_options.string) + 1;
            prog->DC = dc;
        }
        else if (line_struct.line_type == ast_define)
        {
            strcpy(prog->symbol_table[prog->symCount].symName, line_struct.ast_options.define.label);
            prog->symbol_table[prog->symCount].symType = symDefine;
            prog->symbol_table[prog->symCount].address = line_struct.ast_options.define.number;
            prog->symCount++;
        }
        else if (line_struct.line_type == ast_dir && line_struct.ast_options.dir.dir_type <= ast_entry)
        {
            symbol = serach_symbol(prog->symbol_table, prog->symCount, line_struct.ast_options.dir.dir_options.label);
            if (symbol && line_struct.ast_options.dir.dir_type == ast_entry)
            {
                if (symbol->symType == symCode)
                {
                    symbol->symType = symEntryCode;
                }
                else if (symbol->symType == symData)
                {
                    symbol->symType = symEntryData;
                }
                else
                {
                    printf("%s: Error in line %d, redefination of symbol: '%s'\n", am_filename, line_counter, line_struct.ast_options.dir.dir_options.label);
                    is_error = 1;
                }
            }
            else if (!symbol)
            {
                strcpy(prog->symbol_table[prog->symCount].symName, line_struct.ast_options.dir.dir_options.label);
                prog->symbol_table[prog->symCount].symType = line_struct.ast_options.dir.dir_type;
                prog->symCount++;
            }
            else
            {
                printf("%s: Error in line %d, redefination of symbol: '%s'\n", am_filename, line_counter, line_struct.ast_options.dir.dir_options.label);
                is_error = 1;
            }
        }
        line_counter++;
    }
    for (i = 0; i < prog->symCount; i++)
    {
        if (prog->symbol_table[i].symType == symEntry)
        {
            printf("%s: Error symbol:'%s' declared entry but was never defined", am_filename, prog->symbol_table[i].symName);
            is_error = 1;
        }
        if (prog->symbol_table[i].symType == symData || prog->symbol_table[i].symType == symEntryData)
        {
            prog->symbol_table[i].address += ic;
        }
        if (prog->symbol_table[i].symType >= symEntryCode)
        {
            prog->entries[prog->entries_count] = &prog->symbol_table[i];
            prog->entries_count++;
        }
    }
    return is_error;
}