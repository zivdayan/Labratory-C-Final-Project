#include "structs.h"

#define SPLIT_BY_FIRST_SPACE(str, item1, item2)     \
    do                                              \
    {                                               \
        const char *space_pos = strchr((str), ' '); \
        if (space_pos)                              \
        {                                           \
            int len = space_pos - (str);            \
            (item1) = strndup((str), len);          \
            (item2) = strdup(space_pos + 1);        \
        }                                           \
        else                                        \
        {                                           \
            (item1) = strdup((str));                \
            (item2) = strdup("");                   \
        }                                           \
    } while (0)

char *preproc(char *bname, struct Node **output_macro_list);
