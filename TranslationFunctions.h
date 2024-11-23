#include "asttree.h"
#include "symbolTable.h"



void print_octal_with_address(FILE *file, int address, int value);
void print_codePicture_values(struct compilation_part *unit, FILE *file);
void print_dataPicture_values(struct compilation_part *unit, FILE *file);
void print_ob_file(struct compilation_part *unit, const char *baseName);
void print_Entries(struct symbol *entries[], int entriesCount, const char *baseName);
void print_Externs(struct compilation_part *unit, const char *baseName);
void free_symbol_list(struct symbol *head);
void free_externs(struct compilation_part *unit);






