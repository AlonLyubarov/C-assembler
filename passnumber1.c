
#include "asttree.h"
#include <ctype.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "globals.h"


#define MAX_LINE 81
#define NoAddressYet 0

void handle_label_definition(const char *fileNameAM, struct as_tree *lineBuild, struct symbol **symbolListHead, struct symbol **symsearcher, int *error, int lineCounter, int ic, int dc);
void handle_instruction(struct as_tree *lineBuild, int *ic);
void handle_data_directive(struct as_tree *lineBuild, struct compilation_part *unit, int *dc);
void handle_string_directive(struct as_tree *lineBuild, struct compilation_part *unit, int *dc);
void handle_entry_extern(const char *fileNameAM,struct as_tree *lineBuild, struct symbol **symbolListHead, struct symbol **symsearcher, int *error, int lineCounter);
 
/** 
 * find all symbols definitions and add them and their attributes to the symbol list also add the entry symbols to an entries array 
 * and encode the whole data found to the data picture. (labels with data / separate data)
 * @param fileNameAM, pointer to file to print the file reference  in case of errors for user
 * @param amFile  actual file we scan
 * @param symbolListHead symbol table list head
 * @param entries[] entries array for entry initialization
 * @param entriesCount counter of entries
 * @param unit store the translated information and track ic dc also including externals struct 
 * @return return 1 if we have error in the line otherswise 0 
 */
int pass_one(const char *fileNameAM, FILE *amFile, struct symbol **symbolListHead, struct symbol *entries[], int *entriesCount, struct compilation_part *unit) {
    int error = 0;
    char line[MAX_LINE] = {0};
    int ic = 100;
    int dc = 0;
    int lineCounter = 1;
    struct as_tree *lineBuild = NULL;
    struct symbol *symsearcher;

    /*scan and initialize last line char to '\0' */
    while (fgets(line, sizeof(line), amFile)) {
        

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        /*if we get error from the praser function we print it other wise continue*/
        lineBuild = summarize_ast_line(line,lineCounter);

        if (lineBuild->error[0] != '\0') {
            printf("syntx Error in file %s: in line %d: error details: %s\n", fileNameAM, lineCounter, lineBuild->error);
            error = 1;
        }
        /*if its a label first (definition) and the first value of the label is instruction or its directive  of string or data type*/
        if (lineBuild->labelname != NULL && (lineBuild->ast_type == ast_inst || (lineBuild->ast_type == ast_dir && (lineBuild->dir.dir_type == ast_data || lineBuild->dir.dir_type == ast_string)))) {
            symsearcher = findSymbol(*symbolListHead, lineBuild->labelname); /*search if added symbol already*/
            handle_label_definition(fileNameAM, lineBuild, symbolListHead, &symsearcher, &error, lineCounter, ic, dc); 
        }
        /*handle any type we may encounter, that suits the label*/
        if (lineBuild->ast_type == ast_inst) {
            handle_instruction(lineBuild, &ic);
        } else if (lineBuild->ast_type == ast_dir) {
            if (lineBuild->dir.dir_type == ast_string) {
                handle_string_directive(lineBuild, unit, &dc);
            } else if (lineBuild->dir.dir_type == ast_data) {
                handle_data_directive(lineBuild, unit, &dc);
            } else if (lineBuild->dir.dir_type <= ast_entry) {
                symsearcher = findSymbol(*symbolListHead, lineBuild->dir.operand_type.label);
                handle_entry_extern(fileNameAM,lineBuild, symbolListHead, &symsearcher, &error, lineCounter);
            }
        }
       
        lineCounter++;
        free_ast(lineBuild);
    }
        
    processSymbols(*symbolListHead, entries, entriesCount, &error, ic );
     
    return error;
}

 
/** 
 * if we found a symbol definition and it entry symbol was added already we check if the definition in line is instruction or directive and initialize by type 
 * otherwise if its entry definition again but was added to the symbol tabel its redefinition
 * and if we didnt find the symbol at all  meaning we didnt add it yet we perfrom add symbol.
 * @param lineBuild the summarized line 
 * @param symbolListHead symbol table list head
 * @param symsearcher pointer to the head helping us to search symbols
 * @param error error flag
 * @param lineCounter line counter
 * @param ic instruction counter
 * @param dc data counter
 * @return nothing we just initialize
 */
void handle_label_definition(const char *fileNameAM, struct as_tree *lineBuild, struct symbol **symbolListHead, struct symbol **symsearcher, int *error, int lineCounter, int ic, int dc) {
int existingEntryCheck;
int instructionCheck;
    if (*symsearcher && (*symsearcher)->symbolType != symbolEntry) {
        printf("Error in file %s: error in line %d: redefinition of symbol %s\n", fileNameAM, lineCounter, lineBuild->labelname);
        *error = 1;
        return;
    }

     existingEntryCheck = *symsearcher && (*symsearcher)->symbolType == symbolEntry;
     instructionCheck = lineBuild->ast_type == ast_inst;

    if (existingEntryCheck) { 
            if (instructionCheck) {
            (*symsearcher)->symbolType = symbolEntryCode;
            (*symsearcher)->address = ic;
        } else {
            (*symsearcher)->symbolType = symbolEntryData;
            (*symsearcher)->address = dc;
        }
    } 

    else {
    int symbolType;
    int address;

    if (lineBuild->ast_type == ast_inst) {
        symbolType = symbolCode;
        address = ic;
    } else {
        symbolType = symbolData;
        address = dc;
    }

    add_symbol(symbolListHead, lineBuild->labelname, symbolType, address);
    }
}

/** 
 * encounter instruction then  we need to increase counter by one and also if both opearnds are registers we increase by another one
 * also if atleast one of the opearnds isnt register  we increase by 1 for each  (one / both of them may also be empty then we dont increase respectively)
 * @param lineBuild the summarized line we check
 * @param ic instruction counter
 * @return nothing we just initialize
 */
void handle_instruction(struct as_tree *lineBuild, int *ic) {
    (*ic)++;
    if (lineBuild->inst.operands[0].operand_type >= ast_register_ind && lineBuild->inst.operands[1].operand_type >= ast_register_ind) {
        (*ic)++;
    } else {
        (*ic) += (lineBuild->inst.operands[0].operand_type >= ast_immed) + (lineBuild->inst.operands[1].operand_type >= ast_immed);
    }
}

/** 
 *encode the data (numbers int) to to the array of data picture and update the data counter
 * @param lineBuild the summarized line we check
 * @param dc data counter
 * @param unit store the translated information and track ic dc also including externals struct 
 * @return nothing we just initialize
 */
void handle_data_directive(struct as_tree *lineBuild, struct compilation_part *unit, int *dc) {
    memcpy(&unit->dataPicture[unit->DC], lineBuild->dir.operand_type.number, lineBuild->dir.operand_type.number_count * sizeof(int));
    (*dc) += lineBuild->dir.operand_type.number_count;
    unit->DC = *dc;
}

/** 
 *encode the data (ascii values as int from string ) to to the array of data picture and update the data counter
 * @param lineBuild the summarized line we check
 * @param dc data counter
 * @param unit store the translated information and track ic dc also including externals struct 
 * @return nothing we just initialize
 */
void handle_string_directive(struct as_tree *lineBuild, struct compilation_part *unit, int *dc) {
    memcpy(&unit->dataPicture[unit->DC], lineBuild->dir.operand_type.number, lineBuild->dir.operand_type.number_count * sizeof(int));
    (*dc) += lineBuild->dir.operand_type.number_count + 1;
    unit->DC = *dc;
}

 
/** 
 * if we found an entry definition for symbol that already has been added but only with instruction / data attribute 
 * we update that symbol to also be type of entry symbolEntryData / symbolEntryCode.
 * if we didnt find the symbol in the list as entry we initialize its instruction / data attributes only.
 * if any scenario repeats itself like double entry value line nor symbol that already has been added that we are trying to add 
 * it considerd redefinition
 * @param lineBuild the summarized line 
 * @param symbolListHead symbol table list head
 * @param symsearcher pointer to the head helping us to search symbols
 * @param error error flag
 * @param lineCounter line counter
 * @return nothing we just initialize
 */
void handle_entry_extern(const char *fileNameAM, struct as_tree *lineBuild, struct symbol **symbolListHead, struct symbol **symsearcher, int *error, int lineCounter) {
     const char *label = lineBuild->dir.operand_type.label;
     int dirType = lineBuild->dir.dir_type;
    if (*symsearcher) {
        if (dirType == ast_entry) {
            if ((*symsearcher)->symbolType == symbolCode) {
                (*symsearcher)->symbolType = symbolEntryCode;
                return;
            }
            if ((*symsearcher)->symbolType == symbolData) {
                (*symsearcher)->symbolType = symbolEntryData; 
                return;
            }
            /*redef because symbol can be kind of entry  that was defined or entry declartion again */
            printf("Error in file %s: error in line %d: redefinition of symbol %s\n", fileNameAM, lineCounter, label);
            *error = 1;
            return;
        }
        /*external*/
        printf("Error in file %s: error in line %d: redefinition of symbol %s\n", fileNameAM, lineCounter, label);
        *error = 1;
        return;
        
    }
    
     add_symbol(symbolListHead, label, dirType, NoAddressYet);
}