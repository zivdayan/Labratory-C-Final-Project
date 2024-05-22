#include "secondPass.h"
#include "frontend.h"
#include "utils.h"
#include "structs.h"

int second_pass(struct translation_unit *prog, const char *am_filename, FILE *am_file, struct Node *macro_list)
{
    char line[81] = {0};
    int is_error = 0;
    int line_counter = 1;
    int addressing;
    int i;

    struct symbol *symbol;
    struct extr *external;
    struct ast line_struct = {0};

    while (fgets(line, sizeof(line), am_file))
    {
        line_struct = *get_ast_from_line(line, macro_list);
        if (line_struct.line_type == ast_inst)
        {
            prog->code_image[prog->IC] = line_struct.ast_options.inst.inst_type << 6;
            if (line_struct.ast_options.inst.operands[0].operand_type != none)
            {
                if (line_struct.ast_options.inst.operands[1].operand_type == none)
                {
                    addressing = choose_addressing(line_struct.ast_options.inst.operands[0].addrs_mode);
                    prog->code_image[prog->IC] |= addressing << 2;
                }
                else
                {
                    addressing = choose_addressing(line_struct.ast_options.inst.operands[0].addrs_mode);
                    prog->code_image[prog->IC] |= addressing << 4;

                    addressing = choose_addressing(line_struct.ast_options.inst.operands[1].addrs_mode);
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
                            symbol = search_symbol(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.label);
                            if (symbol)
                            {
                                prog->code_image[prog->IC] = symbol->address << 2;
                                prog->code_image[prog->IC] |= symbol->symType == symExtern ? 1 : 2;

                                prog->IC++;
                            }
                            symbol = search_symbol(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.index.index_option.label);
                            if (symbol)
                            {
                                prog->code_image[prog->IC] = symbol->address << 2;
                            }
                            else
                            {
                                prog->code_image[prog->IC] = line_struct.ast_options.inst.operands[i].operand_options.index.index_option.number;
                            }
                        }
                        else if (line_struct.ast_options.inst.operands[i].addrs_mode == adddrs_index_const)
                        {
                            symbol = search_symbol(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.label);
                            if (symbol)
                            {
                                prog->code_image[prog->IC] = symbol->address << 2;
                                prog->code_image[prog->IC] |= symbol->symType == symExtern ? 1 : 2;
                                prog->IC++;
                            }
                            prog->code_image[prog->IC] = line_struct.ast_options.inst.operands[i].operand_options.index.index_option.number << 2;
                        }
                    }

                    else if (line_struct.ast_options.inst.operands[i].operand_type == label)
                    {
                        symbol = search_symbol(prog->symbol_table, prog->symCount, line_struct.ast_options.inst.operands[i].operand_options.label);
                        if (symbol)
                        {
                            prog->code_image[prog->IC] = symbol->address << 2;
                            if (symbol->symType == symExtern)
                            {
                                prog->code_image[prog->IC] |= 1;
                                external = search_external(prog->externals, prog->extCount, symbol->symName);
                                if (external)
                                {
                                    external->addresses[external->address_count] = prog->IC + 100;
                                    external->address_count++;
                                }
                                else
                                {
                                    prog->externals[prog->extCount].externalName = symbol->symName;
                                    prog->externals[prog->extCount].addresses[prog->externals[prog->extCount].address_count] = prog->IC + 100;
                                    prog->externals[prog->extCount].address_count++;
                                    prog->extCount++;
                                }
                            }
                            else
                            {
                                if (symbol->symType != symDefine)
                                {
                                    prog->code_image[prog->IC] |= 2;
                                }
                            }
                        }
                        else
                        {
                            printf("%s: Error in line %d, undefined label:'%s'\n", am_filename, line_counter, line_struct.ast_options.inst.operands[i].operand_options.label);
                            is_error = 1;
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
        line_counter++;
    }
    return is_error;
}