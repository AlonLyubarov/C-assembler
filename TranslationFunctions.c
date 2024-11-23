#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "TranslationFunctions.h"

/** 
 *print from the 15 bit to octal
 * @param file the file to print to
 * @param value the 15 bit value
 * @return nothing
 */
void print_octal_with_address(FILE *file, int address, int value) {
    int mask = 7; 
    int octal_value[5]; /*asked for 5 numbers*/
    int i;

    for (i = 0; i < 5; i++) {
        octal_value[i] = value & mask;
        value >>= 3; /*15 so we move 3 15/3 = 5*/
    }

    fprintf(file, "%04d ", address);  /*address print*/


    for (i = 4; i >= 0; i--) {
        fprintf(file, "%d", octal_value[i]); /*octal value print*/
    }
    fprintf(file, "\n");
}

/** 
 *print the code to the ob file
 * @param unit  stores the translated information and track ic dc also including externals struct 
 * @param file file to print to
 * @return nothing
 */
void print_codePicture_values(struct compilation_part *unit, FILE *file) {
    int i;
    if (unit->IC < 0 || unit->IC > MAX_DATA) {
        printf("Error: IC value is out of bounds.\n");
        return;
    }

    for ( i = 0; i < unit->IC; i++) {
        print_octal_with_address(file, 100 + i, unit->codePicture[i]); 
    }
}

/** 
 *print the data to the ob file
 * @param unit  stores the translated information and track ic dc also including externals struct 
 * @param file file to print to
 * @return nothing
 */
void print_dataPicture_values(struct compilation_part *unit, FILE *file) {
    int i;
    if (unit->DC < 0 || unit->DC > MAX_DATA) {
        printf("Error: DC value is out of bounds.\n");
        return;
    }

    for ( i = 0; i < unit->DC; i++) {
        print_octal_with_address(file, 100 + unit->IC + i, unit->dataPicture[i]); 
    }
}
/** 
 *print the data  and code to the ob file
 * @param unit  stores the translated information and track ic dc also including externals struct 
 * @param baseName file name to be
 * @return nothing
 */
void print_ob_file(struct compilation_part *unit, const char *baseName) {
      FILE *file;
    char *fileName;
     fileName= malloc(strlen(baseName) + 4); 
    if (fileName == NULL) {
        fprintf(stderr, "malloc failed\n");
        return;
    }
    strcpy(fileName, baseName);
    strcat(fileName, ".ob");

    file = fopen(fileName, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(fileName);
        return;
    }

    fprintf(file, "  %d %d\n", unit->IC, unit->DC);

    print_codePicture_values(unit, file);

    print_dataPicture_values(unit, file);

    fclose(file);
    free(fileName);
}


/** 
 *print entries to entries file
 * @param entries  entries array
 * @param entriesCount the entries count
 * @param basename file name to be
 * @return nothing
 */
void print_Entries(struct symbol *entries[], int entriesCount, const char *baseName) {
    char *entFileName; 
     FILE *entFile;
     int i;
    entFileName = malloc(strlen(baseName) + 5); 
    if (entFileName == NULL) {
        fprintf(stderr, "malloc failed\n");
        return;
    }
    strcpy(entFileName, baseName);
    strcat(entFileName, ".ent");

    entFile = fopen(entFileName, "w");
    if (entFile == NULL) {
        perror("Error opening file");
        free(entFileName);
        return;
    }

    for ( i = 0; i < entriesCount; i++) {
        fprintf(entFile, "%s %04d\n", entries[i]->symbolName, entries[i]->address); /*print with adress and name */
    }

    fclose(entFile);

    free(entFileName);
}

/** 
 *print externs to externs file
 * @param unit stores the extern struct array
 * @param basename file name to be
 * @return nothing
 */
void print_Externs(struct compilation_part *unit, const char *baseName) {
    char *extFileName;
    FILE *extFile;
    int i;
    int j;
    extFileName = malloc(strlen(baseName) + 5); 

    if (extFileName == NULL) {
        fprintf(stderr, "malloc failed\n");
        return;
    }
    strcpy(extFileName, baseName);
    strcat(extFileName, ".ext");

    extFile = fopen(extFileName, "w");
    if (extFile == NULL) {
        perror("Error opening file");
        free(extFileName);
        return;
    }

    for ( i = 0; i < unit->extCount; i++) {
        for ( j = 0; j < unit->extrn[i].addressCount; j++) {
            fprintf(extFile, "%s %04d\n", unit->extrn[i].extername, unit->extrn[i].addresses[j]);
        }
    }

    fclose(extFile);

    free(extFileName);
}


/** 
 *free externs
 * @param unit stores the extern struct array
 * @return nothing
 */
void free_externs(struct compilation_part *unit) {
    int i;
    for ( i = 0; i < unit->extCount; i++) {
        free(unit->extrn[i].extername); 
        unit->extrn[i].extername = NULL; 
    }
}

/** 
 *free symbol list
 * @param head list head
 * @return nothing
 */
void free_symbol_list(struct symbol *head) {
    struct symbol *current = head;
    while (current != NULL) {
        struct symbol *next = current->next;
        free(current);
        current = next;
    }
}

