#include "utils.h"
#include "global_consts.h"

void char_sanitize(char **original_str, char c)
{   
    char* str = *original_str;
    char newstr[MAX_LINE_LENGTH+1];
    int i, j;

    i = 0;
    j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == c) {
            newstr[j++] = ' ';  /* Add a space before the comma */ 
            newstr[j++] = c;  /* Add a comma */ 
            newstr[j++] = ' ';  /* Add a space after the comma */ 
        }
        else
        {
            newstr[j++] = str[i];
        }
    }
    newstr[j] = '\0';  /* Null-terminate the new string */ 

    *original_str=newstr;
}

