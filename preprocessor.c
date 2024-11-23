
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"



#define MAX_MACRO_LENGTH 32
#define MAX_WORD_LENGTH  32
#define MAX_LINE_LENGTH 80



/*macro value and name attributes and next pointer */
struct MacroNode
{
  char * macroName;
  char * macroValue;
  struct MacroNode* next;
  
};
/*head of the list*/
struct MacroList{
    struct MacroNode* head;
};

/**
 * function to check if the word given is alone in the line
 * @param line line to scan 
 * @param keywordp key word to check 
 * @return 0 is not alone otherwise 1
 */
int is_word_alone(const char* line , char* keyword) {
    const char* start = line;

    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    if (strncmp(start, keyword, strlen(keyword)) == 0) {
        start += strlen(keyword);

        while (*start != '\0' && isspace((unsigned char)*start)) {
            start++;
        }

        return *start == '\0';
    }

    return 0;
}



/**
 * create macro node
 * @param macroName maco name 
 * @param macroValue the value 
 * @return node otherwise NULL
 */
struct MacroNode* create_macro_node(const char* macroName, const char* macroValue) {
    struct MacroNode* newMacroNode = (struct MacroNode*)malloc(sizeof(struct MacroNode));
    if (newMacroNode != NULL) {
        newMacroNode->macroName = (char*)malloc(strlen(macroName) + 1);
        newMacroNode->macroValue = (char*)malloc(strlen(macroValue) + 1);

        if (newMacroNode->macroName == NULL || newMacroNode->macroValue == NULL) {
            free(newMacroNode->macroName);
            free(newMacroNode->macroValue);
            free(newMacroNode);
            return NULL;
        }

        strcpy(newMacroNode->macroName, macroName);
        strcpy(newMacroNode->macroValue, macroValue);
        newMacroNode->next = NULL;
    }
    return newMacroNode;
}


/**
 * free the macro lisr
 * @param list macro list 
 * @return nothing
 */
void freeMacroList(struct MacroList* list) {
    struct MacroNode* current = list->head;
    struct MacroNode* next;
    while (current != NULL) {
        next = current->next;
        free(current->macroName);
        free(current->macroValue);
        free(current);
        current = next;
    }
    list->head = NULL;
}


/**
 * add the macro to the list
 * @param list macro list we add to
 * @param macroName maco name to add
 * @param macroValue the value to add
 * @return node otherwise NULL
 */
struct MacroNode* add_macro(struct MacroList* list, const char* macroName, const char* macroValue) {
    struct MacroNode* newNode = create_macro_node(macroName, macroValue); /* create the macro */
    if (newNode != NULL) {
        if (list->head == NULL) {
            list->head = newNode;
        } else {
            struct MacroNode* current = list->head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
        return newNode; 
    } else {
        return NULL;
    }
}


 
/**
 * check if there is a macro name in the line that  exist in the list 
 * @param list macro list we scan 
 * @param line The line to check.
 * @return node otherwise NULL
 */
struct MacroNode* search_macro(struct MacroList* list, char* line) {
    char macro_name[MAX_MACRO_LENGTH];
    size_t name_length;
    char *start = line;
    char *end;
    struct MacroNode* current;

    while (isspace((unsigned char)*start)) {
        start++;
    }

    end = start;
    while (*end != '\0' && !isspace((unsigned char)*end)) {
        end++;
    }

     name_length = end - start;
    if (name_length >= MAX_MACRO_LENGTH) {
        name_length = MAX_MACRO_LENGTH - 1;
    }

    strncpy(macro_name, start, name_length);
    macro_name[name_length] = '\0';

    current = list->head;
    while (current != NULL) {
        if (strcmp(macro_name, current->macroName) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}





/**
 * check if the given macro word is valid 
 * @param macro The macro word to check.
 * @param lineNumber follow lines for line reference if we have an error (later in the file scan)
 * @return 1 if the macro is valid, 0 otherwise.
 */
int is_valid_macro(const char *macro , int lineNumebr) {
   
    int i;
    int len = strlen(macro);


    if (!isalpha(macro[0])) {
        fprintf(stderr, "error: Macro name '%s' starts with ilegel char\n", macro);
        fprintf(stderr, " line of error %d \n", lineNumebr);

        return 0;
    }

    for ( i = 1; i < len; i++) {
        if (!isalnum(macro[i])) {
        fprintf(stderr, "error: Macro name '%s'starting from the second character, is not alphanumeric\n", macro);
        fprintf(stderr, " line of error %d \n", lineNumebr);

         return 0;
        }
    }

    if (is_reserved_word(macro)) {
       fprintf(stderr, "error: Macro name '%s' is also an saved word\n", macro);
       fprintf(stderr, " line of error %d \n", lineNumebr);

        return 0;
    }

    return 1;
}

/**
 * extract macro name from line 
 * @param line line to check macro def from
 * @param lineNumber line counter
 * @return 1 if there is a def othetwise 0
 */
int is_macro_definition(const char *line, int lineNumber) {
    const char *keyword = "macr";
    char macro_name[MAX_WORD_LENGTH + 1];
    const char *macro_name_start;
    size_t length;
    const char *start = line;

    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    if (strncmp(start, keyword, strlen(keyword)) == 0) {
        start += strlen(keyword);

        while (*start != '\0' && isspace((unsigned char)*start)) {
            start++;
        }

        if (*start == '\0' || isspace((unsigned char)*start)) {
            return 0;
        }

        macro_name_start = start;
        while (*start != '\0' && !isspace((unsigned char)*start)) {
            start++;
        }

        length = start - macro_name_start;

        if (length > MAX_WORD_LENGTH) {
             fprintf(stderr, "Error: Macro name too long by length of %d characters at line %d\n", MAX_WORD_LENGTH, lineNumber);
            return 0;
        }

        strncpy(macro_name, macro_name_start, length);
        macro_name[length] = '\0';

        if (is_valid_macro(macro_name, lineNumber)==0) {
            return 0;
        }

        while (*start != '\0' && isspace((unsigned char)*start)) {
            start++;
        }

        if (*start != '\0') {
            fprintf(stderr, "Error: Unexpected text after macro name '%s' at line %d\n", macro_name, lineNumber);

            return 0;
        }
         return 1;
    }

    return 0;
}
/**
 * extract macro name from line 
 * @param line line to extract from 
 * @param macro_name the string we initialize the macro name to
 * @return nothing void type
 */
void extract_macro_name(const char *line, char *macro_name) {
    size_t length;
    const char *end;
    const char *start = strstr(line, "macr");

    if (start != NULL) {
        start += 4;

        while (*start == ' ') {
            start++;
        }

        end = start;
        while (*end != ' ' && *end != '\0') {
            end++;
        }

        length = end - start;

        strncpy(macro_name, start, length);
        macro_name[length] = '\0';
    }
}


/**
 * we get a file size
 * @param file the file to size up
 * @return file size otherwise -1
 */
long get_file_size(FILE* file) {
    long size;

    if (file == NULL) {
        fprintf(stderr, "Error: File is not open\n");
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: Cannot move file pointer to the end\n");
        return -1;
    }

    size = ftell(file);
    if (size == -1L) {
        fprintf(stderr, "Error: Cannot determine file size\n");
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Error: Cannot reset file pointer to the beginning\n");
        return -1;
    }

    return size;
}

/**
 * process the file and open the macros max macro value can be the whole value in the file
 * @param inputFileName file name to scan 
 * @param outputFileName am file name after file processed
 * @return nothing void
 */
void preprocessor(const char* inputFileName, const char* outputFileName, int *error) {
    struct MacroList list = {NULL};
    char line[MAX_LINE_LENGTH];
    size_t current_length;
    size_t additional_length;
    char* linesvalue;
    long file_size;
    char* trimmed_line;
    int Linecounter;
    int is_macro_def;
    FILE* outputFile;
    FILE* inputFile;
    size_t len;
    char macro_name[MAX_MACRO_LENGTH];
    struct MacroNode* macro = NULL;

    Linecounter = 0;
    macro_name[0] = '\0';

    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return;
    }

    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return;
    }

    file_size = get_file_size(inputFile);
    linesvalue = (char*)malloc(file_size + 1); 
    if (linesvalue == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for linesvalue.\n");
        fclose(inputFile);
        fclose(outputFile);
        return;
    }
    linesvalue[0] = '\0';

    while (fgets(line, sizeof(line), inputFile)) {
        Linecounter++;
        len = strlen(line);
        
        /*clean last char of line to prevent tabs and ect*/
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        trimmed_line = line;
        while (isspace((unsigned char)*trimmed_line)) {
            trimmed_line++;
        }
        /* line starts with ';' as  first char, skip*/ 
        if (*line == ';') {
            continue;
        }
        /*empty line skip*/
        if (*trimmed_line == '\0') {
            continue;
        }
       
        is_macro_def = is_macro_definition(line, Linecounter);
        /*if the line has macro definition*/
        if (is_macro_def == 1) {
            /*get the macro word from the line*/
            extract_macro_name(line, macro_name);
            /*start adding the content of the macro*/
            while (fgets(line, sizeof(line), inputFile)) {
                Linecounter++;
                /*reached final endmacr and its alone*/
                if (is_word_alone(line, "endmacr") == 1 && strstr(line, "endmacr") != NULL) {
                    break;
                }
                /*endmacr isnt alone meaning its invalid closing */
                if (is_word_alone(line, "endmacr") == 0 && strstr(line, "endmacr") != NULL) {
                    fprintf(stderr, "Error: '%s' macro closer 'endmacr' is not stand alone word at line %d \n", macro_name, Linecounter);
                    *error = 0;
                    macro_name[0] = '\0';
                    break;
                }
                /*content strcat*/
                 current_length = strlen(linesvalue);
                 additional_length = strlen(line) + 1; 

                if (current_length + additional_length < file_size) {
                    strncat(linesvalue, line, file_size - current_length - 1);
                } else {
                    fprintf(stderr, "Error: Insufficient memory allocated for macro content\n");
                    *error = 0;
                    break;
                }
            
            }
            /*add macro with macro content*/
            add_macro(&list, macro_name, linesvalue);
            linesvalue[0] = '\0';
            macro_name[0] = '\0';
            continue;
            }
         
        /*if macr is not alone but macr  string encounterd and we check  its not endmacr*/
        if (is_word_alone(line, "macr") == 0 && strstr(line, "macr") != NULL && strstr(line, "endmacr") == NULL) {
            fprintf(stderr, "Error: macr is not stand alone macro word at line %d\n", Linecounter);
            *error = 0;
            macro_name[0] = '\0';
            continue;
        }

        macro = search_macro(&list, line);
        /*if we found a macro but its not a stand alone word in the line that means we cant open the macro for am file and its invalid*/
        /*macro name cant be substring or with other chars in the line*/
        /*otherwise open the macro to the output file*/
        if (macro != NULL) {  
            if (!is_word_alone(line, macro->macroName)) {
                printf("Error: Macro '%s' found but not alone in the line %d\n", macro->macroName, Linecounter);
                *error = 0;
                continue;   
            } else { /*alone so we print*/
                fprintf(outputFile, "%s", macro->macroValue);
                continue;
            }
        }
        
        /*any other line that is not endmacr we just print to the file*/
        if (strstr(line, "endmacr") == NULL) {
            fprintf(outputFile, "%s\n", line);
        }
     }
    free(linesvalue);
    freeMacroList(&list);
    fclose(inputFile);
    fclose(outputFile);
        
    }

    
  