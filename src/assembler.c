
#include <stdio.h>
#include <stdlib.h>
#include "preprocessor.h"
#include "structs.h"
#include "firstPass.h"
#include "secondPass.h"
#include "backend.h"

/**
 * Main function to handle and assemble an input file.
 *
 * Input:
 * The function gets as an input arguments through the command line, and generated the required
 * output files after assembly.
 * Example: ./assembler example, will initiate assembly process to example.as file, and will
 * generate example.am, example.ob, example.ext, example.ent
 *
 * @param argc The number of input arguments.
 * @param argv An array of strings containing the input arguments.
 * @return Returns 0 on successful assembly.
 */

int main(int argc, char *argv[])
{
    char **pargv;
    struct translation_unit *program;
    if (argc < 2)
    {
        printf("ERROR: You need at least one argument.\n Example: ./assembler example");
        return 1;
    }
    for (pargv = argv + 1; *pargv != argv[argc]; pargv++)
    {
        char *am_filename = "";
        FILE *am_file;
        char *current_file = *pargv;
        struct Node *output_macro_list = NULL;

        printf("Starting preprocessor - %s \n", current_file);

        am_filename = preproc(current_file, output_macro_list);

        if (!strcmp(am_filename, ""))
            continue;

        printf("Preprocessor done, file name: %s \n", am_filename);

        am_file = fopen(am_filename, "r");

        printf("Starting first pass - %s \n", current_file);

        /* -- Execute First pass -- */

        program = malloc(sizeof(struct translation_unit));

        if (!first_pass(program, am_filename, am_file, output_macro_list))
        {
            rewind(am_file);
            printf("Starting second pass - %s \n", current_file);

            /* -- Execute Second pass -- */
            if (!second_pass(program, am_filename, am_file, output_macro_list))
            {

                print_ob_file(current_file, program);
                if (program->entries_count >= 1)
                    print_ent_file(current_file, program);

                if (program->extCount >= 1)
                    print_ext_file(current_file, program);
            }
        }
    }
    printf("end\n");
    return 0;
}
