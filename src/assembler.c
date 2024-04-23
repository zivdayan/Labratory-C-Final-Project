
#include <stdio.h>
#include <stdlib.h>
#include "preprocessor.h"

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
    /* char *as_file, *am_file */;
    if (argc < 2)
    {
        printf("ERROR: You need at least one argument.\n Example: ./assembler example");
        return 1;
    }
    for (pargv = argv + 1; *pargv != argv[argc]; pargv++)
    {
        char *amFileName;
        char *current_file = *pargv;

        printf("Starting preprocessor - %s \n", current_file);

        amFileName = preproc(current_file);

        printf("Preprocessor done, file name: %s \n", amFileName);

        printf("Starting first pass - %s \n", current_file);

        /* -- Execute First pass -- */

        printf("Starting second pass - %s \n", current_file);

        /* -- Execute First pass -- */
    }
    printf("end\n");
    return 0;
}