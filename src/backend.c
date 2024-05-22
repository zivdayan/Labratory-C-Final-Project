#include "backend.h"

void print_ob_file(const char *bname, const struct translation_unit *program)
{
    /* Each instruction is 14 bits. We will use base4, which means we will take every 2 bits and turn in into base4-enc
    That means we should have 7 signs in every line  */

    char base4enc_table[] = {'*', '#', '%', '!'};

    char *obFileName;
    FILE *obFile;

    int i;

    obFileName = malloc(sizeof(char *) * (strlen(bname) + 5));
    strcpy(obFileName, bname);
    strcat(obFileName, ".ob");

    obFile = fopen(obFileName, "w");

    if (obFile)
    {
        fprintf(obFile, "%4d %d\n", program->IC, program->DC);
        for (i = 0; i < program->IC; i++)
        {
            fprintf(obFile, "%04d %c%c%c%c%c%c%c\n",
                    START_LINE + i,
                    base4enc_table[(program->code_image[i] >> 12) & 0x3],
                    base4enc_table[(program->code_image[i] >> 10) & 0x3],
                    base4enc_table[(program->code_image[i] >> 8) & 0x3],
                    base4enc_table[(program->code_image[i] >> 6) & 0x3],
                    base4enc_table[(program->code_image[i] >> 4) & 0x3],
                    base4enc_table[(program->code_image[i] >> 2) & 0x3],
                    base4enc_table[(program->code_image[i]) & 0x3]);
        }

        for (i = 0; i < program->DC; i++)
        {
            fprintf(obFile, "%04d %c%c%c%c%c%c%c\n",
                    START_LINE + program->IC + i,
                    base4enc_table[(program->data_image[i] >> 12) & 0x3],
                    base4enc_table[(program->data_image[i] >> 10) & 0x3],
                    base4enc_table[(program->data_image[i] >> 8) & 0x3],
                    base4enc_table[(program->data_image[i] >> 6) & 0x3],
                    base4enc_table[(program->data_image[i] >> 4) & 0x3],
                    base4enc_table[(program->data_image[i] >> 2) & 0x3],
                    base4enc_table[(program->data_image[i]) & 0x3]);
        }
        fclose(obFile);
    }

    free(obFileName);
}

void print_ent_file(const char *bname, const struct translation_unit *program)
{

    char *entFileName;
    FILE *entFile;

    int i;
    if (program->entries_count)
    {
        entFileName = malloc(sizeof(char *) * (strlen(bname) + 5));
        strcpy(entFileName, bname);
        strcat(entFileName, ".ent");

        entFile = fopen(entFileName, "w");

        if (entFile)
        {
            for (i = 0; i < program->entries_count; i++)
            {
                fprintf(entFile, "%s\t%04d\n", program->entries[i]->symName, program->entries[i]->address); 
            }
            fclose(entFile);
        }
        free(entFileName);
    }
}

void print_ext_file(const char *bname, const struct translation_unit *program)
{

    char *extFileName;
    FILE *extFile;

    int i, j;

    if (program->extCount)
    {
        extFileName = malloc(sizeof(char *) * (strlen(bname) + 5));
        strcpy(extFileName, bname);
        strcat(extFileName, ".ext");

        extFile = fopen(extFileName, "w"); 

        if (extFile)
        {
            for (i = 0; i < program->extCount; i++)
            {
                for (j = 0; j < program->externals[i].address_count; j++)
                {
                    fprintf(extFile, "%s\t%04d\n", program->externals[i].externalName, program->externals[i].addresses[j]);
                }
            }

            fclose(extFile);
        }

        free(extFileName);
    }
}