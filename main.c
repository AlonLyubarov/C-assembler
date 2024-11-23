#include "TranslationFunctions.h"
#include "Mainfunctions.h"
#define MAX_FILE_NAME 80

/**
 *the function is preprocessing given file if it goes well (no errors) we continue to the first and second passes , otherwise we stop after preprocessing
 * when we reach the passes we find out the errors in the first pass and the second pass, if we dont have errors at the second pass and only in the first pass
 * then we show only the errors in the first one , also if we only have errors in the second pass and not in the first one we only show the errors of the second
 * ofcourse if we have errors in both passes we show them both
 * if the preprocess went well and BOTH of the passes went well we create ob file and if we have externs / entrys we create  individual file for each/
 * @param inputFileName file name to work on
 */
void assembler(const char *inputFileName) {
    char amFileName[MAX_FILE_NAME];
    char asFileName[MAX_FILE_NAME];
    FILE *amFile;
    struct symbol *symbolListHead = NULL;
    struct symbol *entries[100] = {0};
    struct compilation_part unit = {0};
    int entriesCount = 0;
    int preERROR = 1;
    int firstPassSuccess = 1; 
    int secondPassSuccess = 1; 

    sprintf(asFileName, "%s.as", inputFileName);
    sprintf(amFileName, "%s.am", inputFileName);

    preprocessor(asFileName, amFileName , &preERROR);
    if (preERROR == 0)
    {
     fprintf(stderr, "PREPROCESSING FAILED CANT CONTINUE TO PASSES.\n");
     exit(1);

    }
    
    amFile = fopen(amFileName, "r");
    if (!amFile) {
        perror("Failed to open preprocessed file");
        return;
    }

    if (pass_one(amFileName, amFile, &symbolListHead, entries, &entriesCount, &unit) != 0) {
        firstPassSuccess = 0;
        fprintf(stderr, "FRIST PASS FAILED.\n");
       
    }

    rewind(amFile);

    if (pass_two(amFileName, amFile, &symbolListHead, &unit) != 0) {
        secondPassSuccess = 0;
        fprintf(stderr, "SECOND PASS FAILED.\n");
        
    }

    if (firstPassSuccess && secondPassSuccess ) {
        if (entriesCount > 0) {
            print_Entries(entries, entriesCount, inputFileName);
        }

        if (unit.extCount > 0) {
            print_Externs(&unit, inputFileName);
        }

        print_ob_file(&unit, inputFileName);
    } 
    

    fclose(amFile);
    free_externs(&unit);
    free_symbol_list(symbolListHead);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    assembler(argv[1]);

    return 0;
}
