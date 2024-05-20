#include "secondPass.h"
#include "frontend.h"
#include "utils.h"
#include "structs.h"

int secondPass(struct translation_unit *prog, const char *amFileName, FILE *amFile, struct Node *macro_list)
{
    /* Todo change names and errors strings and move them to error_msg.h */
    char line[81] = {0};
    int errorFlag = 0;
    int lineC = 1;
    int i;
    int addressing;

    struct ast line_struct = {0};
    struct symbol *SymFind;
    struct extr *ExtFind;

    while (fgets(line, sizeof(line), amFile))
    {
        line_struct = *get_ast_from_line(line, macro_list);
        if (line_struct.line_type == ast_inst)
        {
            prog->code_image[prog->IC] = line_struct.ast_options.inst.inst_type << 6;
            if (line_struct.ast_options.inst.operands[0].operand_type != none)
            {
                if (line_struct.ast_options.inst.operands[1].operand_type == none)
                {
                    addressing = chooseAddressing(line_struct.ast_options.inst.operands[0].addrs_mode);
                    prog->code_image[prog->IC] |= addressing << 2;
                }
                else
                {
                    addressing = chooseAddressing(line_struct.ast_options.inst.operands[0].addrs_mode);
                    prog->code_image[prog->IC] |= addressing << 4;

                    addressing = chooseAddressing(line_struct.ast_options.inst.operands[1].addrs_mode);
                    prog->code_image[prog->IC] |= addressing << 2;
                }
            }

            prog->IC++;
            if (line_struct.ast_options.inst.operands[0].operand_type == reg && line_struct.ast_options.inst.operands[1].operand_type == reg)
            {
                prog->code_image[prog->IC] = line_struct.ast_options.inst.operands[0].operand_options.reg << 5;
                prog->code_image[prog->IC] |= line_struct.ast_options.inst.operands[1].operand_options.reg << 2;
                prog->IC++;
            }
            else
            {
                for (i = 0; i < 2; i++)
                {
                    if (line_struct.ast_options.inst.operands[i].operand_type == reg)
                    {
                        prog->code_image[prog->IC] = (reg << (5 - (i * 3)));
                    }
                    else if (line_struct.ast_options.inst.operands[i].operand_type == index_operand)
                    {
                        if (line_struct.ast_options.inst.operands[i].addrs_mode == adddrs_index_label)
                        {
                            SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.label);
                            if (SymFind)
                            {
                                prog->code_image[prog->IC] = SymFind->address << 2; /* Todo: implement extern */
                                prog->code_image[prog->IC] |= 2;                    /* Todo: implement extern */

                                prog->IC++;
                            }
                            SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.index.index_option.label);
                            if (SymFind)
                            {
                                prog->code_image[prog->IC] = SymFind->address << 2; /* Todo: implement extern */
                                                                                    /* Todo: implement extern */
                            }
                            else
                            {
                                prog->code_image[prog->IC] = line_struct.ast_options.inst.operands[i].operand_options.index.index_option.number;
                            }
                        }
                        else if (line_struct.ast_options.inst.operands[i].addrs_mode == adddrs_index_const)
                        {
                            SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.label);
                            if (SymFind)
                            {
                                prog->code_image[prog->IC] = SymFind->address << 2; /* Todo: implement extern */
                                prog->code_image[prog->IC] |= 2;                    /* Todo: implement extern */

                                prog->IC++;
                            }
                            prog->code_image[prog->IC] = line_struct.ast_options.inst.operands[i].operand_options.index.index_option.number << 2;
                        }
                    }

                    else if (line_struct.ast_options.inst.operands[i].operand_type == label)
                    {
                        SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.label);
                        if (SymFind)
                        {
                            prog->code_image[prog->IC] = SymFind->address << 2;
                            if (SymFind->symType == symExtern)
                            {
                                prog->code_image[prog->IC] |= 1;
                                ExtFind = extSearch(prog->externals, prog->extCount, SymFind->symName);
                                if (ExtFind)
                                {
                                    ExtFind->addresses[ExtFind->address_count] = prog->IC + 100;
                                    ExtFind->address_count++;
                                }
                                else
                                {
                                    prog->externals[prog->extCount].externalName = SymFind->symName;
                                    prog->externals[prog->extCount].addresses[prog->externals[prog->extCount].address_count] = prog->IC + 100;
                                    prog->externals[prog->extCount].address_count++;
                                    prog->extCount++;
                                }
                            }
                            else
                            {
                                if (SymFind->symType != symDefine)
                                {
                                    prog->code_image[prog->IC] |= 2;
                                }
                            }
                        }
                        else
                        {
                            printf("%s: error in line %d undefined label:'%s'\n", amFileName, lineC, line_struct.ast_options.inst.operands[i].operand_options.label);
                            errorFlag = 1;
                        }
                    }
                    else if (line_struct.ast_options.inst.operands[i].operand_type == num)
                    {
                        prog->code_image[prog->IC] = line_struct.ast_options.inst.operands[i].operand_options.immed << 2;
                    }
                    if (line_struct.ast_options.inst.operands[i].operand_type != none)
                    {
                        prog->IC++;
                    }
                }
            }
        }
    }
    lineC++;
    return errorFlag;
}