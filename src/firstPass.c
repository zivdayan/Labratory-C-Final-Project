#include "firstPass.h"
#include "frontend.h"
#include "structs.h"
#include <string.h>
#include "utils.h"

/* Todo change names and errors strings and move them to error_msg.h */

int firstPass(struct translation_unit *prog, const char *amFileName, FILE *amFile, struct Node *macro_list)
{
    char line[81] = {0};
    int ic = 100, dc = 0;
    int errorFlag = 0;
    int lineC = 1;
    int i;
    int j;

    struct ast line_struct = {0};
    struct symbol *SymFind;

    while (fgets(line, sizeof(line), amFile))
    {
        line_struct = *get_ast_from_line(line, macro_list);
        if (line_struct.lineError[0] != '\0')
        {
            printf("%s:%d:syntax error: %s", amFileName, lineC, line_struct.lineError);
            lineC++;
            errorFlag = 1;
            continue;
        }
        if (line_struct.labelName[0] != '\0' && (line_struct.line_type == ast_inst || (line_struct.line_type == ast_dir && (line_struct.ast_options.dir.dir_type == ast_data || line_struct.ast_options.dir.dir_type == ast_string))))
        {
            SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.labelName);
            if (SymFind)
            {
                if (SymFind->symType == symEntry)
                {
                    SymFind->symType = line_struct.line_type == ast_inst ? symEntryCode : symEntryData;
                    SymFind->address = line_struct.line_type == ast_inst ? ic : dc;
                }
                else
                {
                    printf("%s: error in line %d redefination of symbol '%s'\n", amFileName, lineC, line_struct.labelName);
                    errorFlag = 1;
                }
            }
            else
            {
                /* Todo - change it to dynamic data structure */
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
            for(j=0;j<line_struct.ast_options.dir.dir_options.data_array.data_length;j++)
            {
                if(line_struct.ast_options.dir.dir_options.data_array.data[j].data_type == data_number)
                    memcpy(&prog->data_image[(prog->DC)+j], &line_struct.ast_options.dir.dir_options.data_array.data[j].data_value.number, sizeof(int));
                else if (line_struct.ast_options.dir.dir_options.data_array.data[j].data_type == data_label)
                {
                    SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.ast_options.dir.dir_options.data_array.data[j].data_value.label);
                    memcpy(&prog->data_image[(prog->DC)+j], &SymFind->address, sizeof(int));
                }
            }
            dc += line_struct.ast_options.dir.dir_options.data_array.data_length;
            prog->DC = dc;
        }
        else if (line_struct.line_type == ast_dir && line_struct.ast_options.dir.dir_type == ast_string)
        {
            for(j=0;j<strlen(line_struct.ast_options.dir.dir_options.string);j++)
            {
                memcpy(&prog->data_image[(prog->DC)+j], line_struct.ast_options.dir.dir_options.string + j, 1);
            }
            memset(&prog->data_image[(prog->DC)+strlen(line_struct.ast_options.dir.dir_options.string)],0,1);
            
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
            SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.ast_options.dir.dir_options.label);
            if (SymFind && line_struct.ast_options.dir.dir_type == ast_entry)
            {
                if (SymFind->symType == symCode)
                {
                    SymFind->symType = symEntryCode;
                }
                else if (SymFind->symType == symData)
                {
                    SymFind->symType = symEntryData;
                }
                else
                {
                    printf("%s: error in line %d redefination of symbol: '%s'\n", amFileName, lineC, line_struct.ast_options.dir.dir_options.label);
                    errorFlag = 1;
                }
            }
            else if (!SymFind)
            {
                strcpy(prog->symbol_table[prog->symCount].symName, line_struct.ast_options.dir.dir_options.label);
                prog->symbol_table[prog->symCount].symType = line_struct.ast_options.dir.dir_type;
                prog->symCount++;
            }
            else
            {
                printf("%s: error in line %d redefination of symbol: '%s'\n", amFileName, lineC, line_struct.ast_options.dir.dir_options.label);
                errorFlag = 1;
            }
        }
        lineC++;
    }
    for (i = 0; i < prog->symCount; i++)
    {
        if (prog->symbol_table[i].symType == symEntry)
        {
            printf("%s: error symbol:'%s' declared entry but was never defined", amFileName, prog->symbol_table[i].symName);
            errorFlag = 1;
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
    return errorFlag;
}