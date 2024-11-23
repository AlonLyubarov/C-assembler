#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "symbolTable.h"



/** 
 *search the extern
 * @param extArray externs array
 * @param extCount number of externs
 * @param symbolName the symbol name
 * @return return the extern otherwise NULL
 */
struct exte* findext(struct exte *extArray, int extCount, const char* symbolName) {
    int i;
    for ( i = 0; i < extCount; i++) {
        if (strcmp(extArray[i].extername, symbolName) == 0) {
            return &extArray[i];
        }
    }
    return NULL;
}

/** 
 *create symbol
 * @param name symbol name
 * @param type symbol type
 * @param address symbol address
 * @return symbol otherwise error
 */
struct symbol* create_symbol(const char *name, int type, int address) {
    struct symbol* new_symbol = (struct symbol*)malloc(sizeof(struct symbol));
    if (!new_symbol) {
        perror("Failed to allocate memory for symbol");
        exit(EXIT_FAILURE);
    }

    if (name) {
        strncpy(new_symbol->symbolName, name, 31);
        new_symbol->symbolName[31] = '\0';
    } else {
        new_symbol->symbolName[0] = '\0';
    }

    new_symbol->symbolType = type;
    new_symbol->address = address;
    new_symbol->next = NULL;
    return new_symbol;
}
/** 
 *find symbol in the list
 * @param head_ref  list head
 * @param name symbol name
 * @param type symbol type
 * @param address symbol address
 * @return nothing we just add
 */
void add_symbol(struct symbol** head_ref, const char *name, int type, int address) {
    struct symbol* new_symbol = create_symbol(name, type, address);
    struct symbol* last = *head_ref;
    if (*head_ref == NULL) {
        *head_ref = new_symbol;
        return;
    }
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = new_symbol;
}
/** 
 *find symbol in the list
 * @param Head  list head
 * @param name symbol name
 * @return return symbol otherwise NULL
 */
struct symbol* findSymbol(struct symbol* head, const char* name) {
    struct symbol* current;
    if (name == NULL) {
        return NULL;
    }
     current = head;

    while (current != NULL) {
        if (strcmp(current->symbolName, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;  
}

/** 
 *we initialize the entries to the entries array and push foward data of all kind thats why we add ic to the dc address (data count)
 *so it will keep its chronology but after the ic
 * @param symbolListHead symbol table list head
 * @param entries[] entries array for entry initialization
 * @param entriesCount counter of entries
 * @param error error flag
 * @param ic instruction counter
 * @return return 1 if we have error in the line otherswise 0 
 */
void processSymbols(struct symbol *symbolListHead, struct symbol *entries[], int *entriesCount, int *error, int ic ) {
    struct symbol* current = symbolListHead;

    while (current != NULL) {
        if (current->symbolType == symbolEntry) {
            printf("Error: entry '%s' with no value  \n", current->symbolName ); /*entry with no value*/
            *error = 1;
        }
        /*push data foward*/
        if (current->symbolType == symbolData) {
            current->address += ic;
        }

        if (current->symbolType == symbolEntryData) {
            current->address += ic;
        }
        /*entries add*/
        if (current->symbolType == symbolEntryCode || current->symbolType == symbolEntryData) {

            if (*entriesCount < MAX_ENTRIES) {
                entries[*entriesCount] = current;
                (*entriesCount)++;
            } else {
                printf("Error: too many entries\n");
                *error = 1;
            }
        }

        current = current->next;
    }

   

}

