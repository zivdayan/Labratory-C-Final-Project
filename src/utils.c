#include "utils.h"

static void comma_sanitize(char **original_str)
{   
    char* str = *original_str;
    char newstr[strlen(str)+1];
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        newstr[j++] = str[i];
        if (str[i] == ',') {
            newstr[j++] = ' ';  // Add a space after the comma
        }
    }
    newstr[j] = '\0';  // Null-terminate the new string

    *original_str=newstr;
}

static void char_sanitize(char **original_str, char c)
{   
    char* str = *original_str;
    char newstr[strlen(str)+1];
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        newstr[j++] = str[i];
        if (str[i] == c) {
            newstr[j++] = ' ';  // Add a space after the comma
        }
    }
    newstr[j] = '\0';  // Null-terminate the new string

    *original_str=newstr;
}

