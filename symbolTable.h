#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 100
#define MAX_EXTERNS 100
#define MAX_ADRESSES 100
#define MAX_DATA 1000
#define MAX_LABEL_LENGTH 32

struct symbol {
    char symbolName[MAX_LABEL_LENGTH];
    enum {
        symbolExtern,
        symbolEntry,
        symbolCode,
        symbolData,
        symbolEntryCode,
        symbolEntryData
    } symbolType;
    int address;
    struct symbol *next; 
};


struct exte {
    char *extername;
    int addresses[MAX_ADRESSES];
    int addressCount;

};

struct compilation_part
{
    int dataPicture[MAX_DATA] ;
    int DC ;
    int codePicture[MAX_DATA];
    int IC;
    struct exte extrn [MAX_EXTERNS];
    int extCount;

};

struct exte * findext(struct exte * extArray, int extCount, const char* symbolName) ;




struct symbol* create_symbol(const char *name, int type, int address);
   



void add_symbol(struct symbol** head_ref, const char *name, int type, int address);
   


struct symbol* findSymbol(struct symbol* head, const char* name);
   



void processSymbols(struct symbol *symbolListHead, struct symbol *entries[], int *entriesCount, int *error, int ic);
     
     
