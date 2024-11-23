#include "asttree.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "globals.h"

#define MAX_LINE 81



/*declartions */
void process_instruction(const char *fileNameAM,struct as_tree *lineBuild, struct compilation_part *unit, struct symbol **symbolListHead, int *error, int lineCounter);
void process_operands(const char *fileNameAM, struct as_tree *lineBuild, struct compilation_part *unit, struct symbol **symbolListHead, int *error, int lineCounter);
void process_both_register_operands(struct as_tree *lineBuild, struct compilation_part *unit);
void process_any_single_register_operand(struct as_tree *lineBuild, struct compilation_part *unit, int operand_index);
void process_label_operand(const char *fileNameAM, struct as_tree *lineBuild, struct compilation_part *unit, struct symbol **symbolListHead, int *error, int lineCounter, int operand_index);
void process_immediate_operand(struct as_tree *lineBuild, struct compilation_part *unit, int operand_index);
void handle_extern_symbol(struct compilation_part *unit, struct symbol *symsearcher);


/** 
 * add the extern symbols and their attributes (like address)to an externs struct 
 * and encode the whole code (not data done in first pass)found to the code picture. (labels with code / separate code)
 * fill the missing address numbers for all the symbols used as operands
 * @param fileNameAM, pointer to file to print the file reference  in case of errors for user
 * @param amFile  actual file we scan
 * @param symbolListHead symbol table list head
 * @param unit store the translated information and track ic dc also including externals struct 
 * @return return 1 if we have error in the line otherswise 0 
 */
int pass_two(const char *fileNameAM, FILE *amFile, struct symbol **symbolListHead, struct compilation_part *unit) {
    int error = 0;
    char line[MAX_LINE] = {0};
    int lineCounter = 1;
    struct as_tree *lineBuild = NULL;

    while (fgets(line, sizeof(line), amFile)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        lineBuild = summarize_ast_line(line, lineCounter);
        /*we already handled the directives in the first pass therfore we only handle the instructions*/
        if (lineBuild->ast_type == ast_inst) {
            process_instruction(fileNameAM, lineBuild ,unit, symbolListHead, &error, lineCounter);
        }

        if (lineBuild->error[0] != '\0') {
            error = 1;
        }

        lineCounter++;
        free_ast(lineBuild);
    }

    return error;
}


/** 
 * using bitwise operators to move the bits if instruction encounterd and if the operands are not empty move the bits by the defined rules
 * and the A.R.E bits also , we also have inner functions to handle the specific operands, first handles when we have both operands as registers
 * and the second one handles the other options
 *very important to notice in the second pass, we fill the missing address numbers for all the symbols used as operands (second time i mention it for clarity)
 * @param lineBuild the summarized line
 * @param unit store the translated information and track ic dc also including externals struct 
 * @param symbolListHead symbol table list head
 * @param error error flag 
 * @param lineCounter the line counter
 * @return return nothing
 */
void process_instruction(const char *fileNameAM, struct as_tree *lineBuild, struct compilation_part *unit, struct symbol **symbolListHead, int *error, int lineCounter) {
    unit->codePicture[unit->IC] = lineBuild->inst.inst_type << 11;
    /* shift operands type and encode */
    if (lineBuild->inst.operands[0].operand_type != ast_none) {
        unit->codePicture[unit->IC] |= (1 << (7 + lineBuild->inst.operands[0].operand_type));
    }
    if (lineBuild->inst.operands[1].operand_type != ast_none) {
        unit->codePicture[unit->IC] |= (1 << (3 + lineBuild->inst.operands[1].operand_type));
    }
    unit->codePicture[unit->IC] |= 4; /*A.R.E*/
    unit->IC++;

    if (lineBuild->inst.operands[0].operand_type >= ast_register_ind && lineBuild->inst.operands[1].operand_type >= ast_register_ind) {
        process_both_register_operands(lineBuild, unit);
    } else {
        process_operands(fileNameAM, lineBuild, unit, symbolListHead, error, lineCounter);
    }
}
/** 
 * this case of 2 registers we move the bits to their position and store the result in the codepicture array and increase the ic
 * @param lineBuild the summarized line
 * @param unit store the translated information and track ic dc also including externals struct 
 * @return return nothing
 */
void process_both_register_operands(struct as_tree *lineBuild, struct compilation_part *unit) {
    /*bit shift and A.R.E and encode*/
    unit->codePicture[unit->IC] = lineBuild->inst.operands[0].operand_option.reg << 6; 
    unit->codePicture[unit->IC] |= lineBuild->inst.operands[1].operand_option.reg << 3;
    unit->codePicture[unit->IC] |= 4;
    unit->IC++;
}


/** 
 * if one of the operands isnt register we use this function to move the bits accordingly for each type (label, immediate, register)
 * @param lineBuild the summarized line
 * @param unit store the translated information and track ic dc also including externals struct 
 * @param symbolListHead symbol table list head
 * @param error error flag 
 * @param lineCounter the line counter
 * @return return nothing
 */
void process_operands(const char *fileNameAM, struct as_tree *lineBuild, struct compilation_part *unit, struct symbol **symbolListHead, int *error, int lineCounter) {
    int i;
    for (i = 0; i < 2; i++) {
        switch (lineBuild->inst.operands[i].operand_type) {
            case ast_register:
            case ast_register_ind:
                process_any_single_register_operand(lineBuild, unit, i);
                break;
            case ast_label:
                process_label_operand( fileNameAM ,lineBuild, unit, symbolListHead, error, lineCounter, i);
                break;
            case ast_immed:
                process_immediate_operand(lineBuild, unit, i);
                break;
            default:
                break;
        }
    }
}

/** 
 * this case of single register we move the bits to their position and store the result in the codepicture array and increase the ic
 * @param lineBuild the summarized line
 * @param unit store the translated information and track ic dc also including externals struct 
 * @param operand_index the index of operand because it can be first or second
 * @return return nothing
 */
void process_any_single_register_operand(struct as_tree *lineBuild, struct compilation_part *unit, int operand_index) {
     /*bit shift and A.R.E and encode*/
    unit->codePicture[unit->IC] = lineBuild->inst.operands[operand_index].operand_option.reg << (6 - (3 * operand_index));
    unit->codePicture[unit->IC] |= 4;
    unit->IC++;
}
/** 
 * if the operand is label we use this function to move the bits also if we encounter in the line symbol that is already defined as extern
 * in the symbol table  we add it to the externs array and each time we encounter new one we do it, other wise we just add the new address.
 * also fill the address number for all  symbols used (as operands ofcourse)
 * @param lineBuild the summarized line
 * @param unit store the translated information and track ic dc also including externals struct 
 * @param symbolListHead symbol table list head
 * @param error error flag 
 * @param lineCounter the line counter
 * @return return nothing
 */
void process_label_operand(const char *fileNameAM, struct as_tree *lineBuild, struct compilation_part *unit, struct symbol **symbolListHead, int *error, int lineCounter, int operand_index) {
    const char *label = lineBuild->inst.operands[operand_index].operand_option.label;
    struct symbol *symsearcher = findSymbol(*symbolListHead, label);

    if (!symsearcher) {
        *error = 1;
        printf("Error in file: %s error in line %d: undefined label: %s\n", fileNameAM, lineCounter, label);
        return;
    }

    /* we add the missing address and shift*/
    unit->codePicture[unit->IC] = symsearcher->address << 3;

    if (symsearcher->symbolType == symbolExtern) {
        handle_extern_symbol(unit, symsearcher);
    } else {
        unit->codePicture[unit->IC] |= 2; /* label shift*/
    }

    unit->IC++;
}

/** 
 * if we encounter in the line symbol that is already defined as extern
 * in the symbol table  we add it to the externs array and each time we encounter new one  we do it, other wise we just add the new address.
 * @param unit store the translated information and track ic dc also including externals struct 
 * @param symseacrher symbol pointer
 * @return return nothing
 */
void handle_extern_symbol(struct compilation_part *unit, struct symbol *symsearcher) {
    struct exte *extersearch;
    unit->codePicture[unit->IC] = 1; /*extern bit*/
    extersearch = findext(unit->extrn, unit->extCount, symsearcher->symbolName);
    if (extersearch) {
        /* extern symbol found adding new extern address*/
        extersearch->addresses[extersearch->addressCount] = unit->IC + 100;
        extersearch->addressCount++;
    } else {
        /* first time extern symbol encounter */
        unit->extrn[unit->extCount].extername = strdup1(symsearcher->symbolName);
        unit->extrn[unit->extCount].addresses[unit->extrn[unit->extCount].addressCount] = unit->IC + 100;
        unit->extrn[unit->extCount].addressCount++;
        unit->extCount++;
    }
}


/** 
 * the case of number we move the bits to their position and store the result in the codepicture array and increase the ic
 * @param lineBuild the summarized line
 * @param unit store the translated information and track ic dc also including externals struct 
 * @param operand_index the index of operand because it can be first or second
 * @return return nothing
 */
void process_immediate_operand(struct as_tree *lineBuild, struct compilation_part *unit, int operand_index) {
   /*bit shift and A.R.E and encode*/
    unit->codePicture[unit->IC] = lineBuild->inst.operands[operand_index].operand_option.immed << 3;
    unit->codePicture[unit->IC] |= 4;
    unit->IC++;
}




