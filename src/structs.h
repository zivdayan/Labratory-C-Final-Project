#define MAX_MEM_SIZE 80
#define MAX_SYMBOLS 80

#ifndef DEFINED_STRUCTS_H
#define DEFINED_STRUCTS_H

struct symbol
{
    char symName[32];
    enum
    {
        symExtern,
        symEntry,
        symCode,
        symData,
        symEntryCode,
        symEntryData
    } symType;
    int address;
};

struct extr
{
    char *externalName;
    int addresses[1024];
    int address_count;
};

struct translation_unit
{
    int code_image[MAX_MEM_SIZE];
    int IC;
    int data_image[MAX_MEM_SIZE];
    int DC;
    struct symbol symbol_table[MAX_SYMBOLS];
    int symCount;
    struct extr externals[MAX_SYMBOLS];
    int extCount;
    struct symbol *entries[MAX_SYMBOLS];
    int entries_count;
};

struct Node
{
    char *value;
    struct Node *next;
};

#endif
