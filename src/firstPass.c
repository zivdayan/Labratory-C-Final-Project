#include "firstPass.h"

int firstPass(struct tranlsation_unit *prog, const char * amFileName, FILE * amFile, struct Node *macro_list)
{
    char line[81] = {0};
    int ic=100, dc=0;
    int errorFlag=0;
    int lineC=1;
    int i;

    struct ast line_struct={0};
    struct symbol *SymFind;

    while(fgets(line, sizeof(line), amFile))
    {
        line_struct = get_ast_from_line(line, macro_list);
    }


}