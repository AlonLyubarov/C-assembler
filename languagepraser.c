/*represent the data of each*/
#include "asttree.h"
#include <ctype.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"


struct string_sep_result {
    char *strings[80];
    int strings_count;
};
/** 
 *  we take a string and break into substrings based on , \t \r \n \v \f
 * @param str the string to break down
 * @return struct that contains the parts of the string (substrings) each stored separately
 * and the number of substrings found
 */
struct string_sep_result split_string(const char *str) {
    struct string_sep_result ssr = {0};
    char *temp = strdup1(str);
    char *token;
    if (!temp) {
        perror("Memory allocation failed");
        return ssr;
    }

    token = strtok(temp, ", \t\r\n\v\f");
    while (token) {
        ssr.strings[ssr.strings_count] = strdup1(token);
        if (!ssr.strings[ssr.strings_count]) {
            perror("Memory allocation failed");
            free(temp);
            return ssr;
        }
        ssr.strings_count++;
        token = strtok(NULL, ", \t\r\n\v\f");
    }

    free(temp);
    return ssr;
}




/** 
 * Check if the is string is a valid number
 * @param str The number to check.
 * @param linecounter line counter
 * @return 1 if the number is valid, 0 otherwise.
 */
int is_valid_number(const char *str , int lincounter) {

    if (*str == '\0') {
        printf("Invalid: empty string at line %d  \n", lincounter);
        return 0;
    }

    if (*str == '-' || *str == '+') {
        str++;
    }

    if (!isdigit((unsigned char)*str)) {
        printf("Invalid: no digit after sign at line %d \n" , lincounter);
        return 0;
    }

    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            printf("Invalid: non-digit character found: %c at line %d \n", *str, lincounter);
            return 0;
        }
        str++;
    }

    
    return 1;
}

/**
 * Check if the string is valid 
 * @param label  label  check.
 * @param linecounter line counter
 * @return 1 if the its vaild otherwise 0
 */
int is_valid_label(const char *label , int linecounter) {
    int i;
    int len = strlen(label);

    
    if (len < 1 || len > 31) {
        printf("Invalid label: Length of '%s' is %d, but should be between 1 and 31 characters at line %d \n", label, len, linecounter);
        return 0;
    }

    if (!isalpha(label[0])) {
        printf("Invalid label: '%s' does not start with an alphabetic character at line %d \n", label ,linecounter);
        return 0;
    }
    
    for (i = 1; i < len; i++) {
        if (!isalnum(label[i])) {
            printf("Invalid label: '%s' contains non-alphanumeric character '%c' at line %d \n", label, label[i], linecounter);
            return 0;
        }
    }

    if (is_reserved_word(label)==1) {
        printf("Invalid label: '%s' is a reserved word at line  %d .\n", label, linecounter);
        return 0;
    }

    return 1;
}

/** 
 * label extrection from line
 * @param line The line we extract the label from.
 * @param linecounter line counter
 * @return The extracted label if found, NULL otherwise.
 */
char* label_defnition_extrector(char *line , int linecounter) {
   const char *colon;
   char *label ;


    while (isspace((unsigned char)*line)) {
        line++;
    }
    colon = strchr(line, ':');

    if (colon != NULL && (colon - line) > 0) {
        int labelLength = colon - line;
        if (isspace((unsigned char)*(colon - 1))) {
            printf("Invalid label: '%s' space before ':' at line  %d .\n", line, linecounter);
            return NULL;
        }

        label = (char*) malloc(labelLength + 1);
        if (label == NULL) {
            perror("Memory allocation failed");
            return NULL;
        }

        strncpy(label, line, labelLength);
        label[labelLength] = '\0'; 

        if ( is_valid_label(label , linecounter) == 1){
            return label;
        }
        free(label);

    }

    return NULL; 
}


/** 
 * label directive extrection from line after using label_defnition_extrector
 * @param line The line in which we  preform the actions.
 * @param linecounter line counter
 * @return The extracted directive if found, NULL otherwise.
 */
char* extract_directive_after_label(char *line, int linecounter) {
    char *label = label_defnition_extrector(line, linecounter);
    const char* directive_position;
    size_t directive_length;
    const char *directive_start;
    const char *directive_end;
    char *directive;

    if (label == NULL) {
        free(label);
        return NULL; 
    }
    directive_position = strchr(line, ':') + 1;
    while (isspace((unsigned char)*directive_position)) {
        directive_position++;
    }

    if (*directive_position == '.') {
        directive_start = directive_position;
        directive_end = directive_position;

        while (*directive_end != '\0' && !isspace((unsigned char)*directive_end)) {
            directive_end++;
        }

         directive_length = directive_end - directive_start;
        directive = (char*) malloc(directive_length + 1);
        if (directive == NULL) {
            perror("Memory allocation failed");
            free(label);
            return NULL;
        }
        strncpy(directive, directive_start, directive_length);
        directive[directive_length] = '\0';

        if (strcmp(directive, ".string") == 0 || strcmp(directive, ".data") == 0) {
            free(label);
            return directive; 
        } else {
            free(directive);
            free(label);
            return NULL; 
        }
    }

    free(label);
    return NULL;
}


/** 
 * label directive extrection from line after using label_defnition_extrector
 * @param line The line in which we  preform the actions.
 * @param linecounter line counter
 * @return The extracted directive if found, NULL otherwise.
 */
char* extract_label_after_directive(char *line, const char *directive, int linecounter) {
    const char *directive_position = strstr(line, directive);
    const char *extra_check;
    size_t label_length;
    char *label;
    if (directive_position != NULL) {
        if ((directive_position == line || isspace((unsigned char)*(directive_position - 1))) &&
            (isspace((unsigned char)*(directive_position + strlen(directive))) || *(directive_position + strlen(directive)) == '\0')) {
            
            directive_position += strlen(directive);

            while (isspace((unsigned char)*directive_position)) {
                directive_position++;
            }

            if (*directive_position != '\0') {
                const char *label_start = directive_position;
                const char *label_end = directive_position;

                while (*label_end != '\0' && !isspace((unsigned char)*label_end)) {
                    label_end++;
                }

                extra_check = label_end;
                while (isspace((unsigned char)*extra_check)) {
                    extra_check++;
                }
                if (*extra_check != '\0') {
                    return NULL; 
                }

                 label_length = label_end - label_start;
                label = (char*) malloc(label_length + 1);
                if (label == NULL) {
                    perror("Memory allocation failed");
                    return NULL;
                }
                strncpy(label, label_start, label_length);
                label[label_length] = '\0';

                if (is_valid_label(label, linecounter)) {
                    return label;
                } else {
                    free(label);
                }
            }
        }
    }

    return NULL; 
}



/** 
 * value extrection from line after the given directive  
 * @param line The line in which we  preform the actions.
 * @param directive The directive we given 
 * @return The extracted directive value if found, NULL otherwise.
 */
char* extract_value_after_directive(char *line, const char *directive) {
    const char *directive_position;
    char *value;
    const char *value_start;
    const char *value_end;
    size_t value_length;
    directive_position = strstr(line, directive);
    
    if (directive_position != NULL) {
        if ((directive_position == line || isspace((unsigned char)*(directive_position - 1))) &&
            (isspace((unsigned char)*(directive_position + strlen(directive))) || *(directive_position + strlen(directive)) == '\0')) {
            
            directive_position += strlen(directive);

            while (isspace((unsigned char)*directive_position)) {
                directive_position++;
            }

            if (*directive_position != '\0') {
                value_start = directive_position;
                value_end = directive_position;

                while (*value_end != '\0' && *value_end != '\n') {
                    value_end++;
                }

                 value_length = value_end - value_start;
                value = (char*) malloc(value_length + 1);
                if (value == NULL) {
                    perror("Memory allocation failed");
                    return NULL;
                }
                strncpy(value, value_start, value_length);
                value[value_length] = '\0';

                return value;
            }
        }
    }

    return NULL; 
}



/** 
 * value extrection of .string from line after the given .string directive (we use extract_value_after_directive)
 * @param line The line in which we  preform the actions.
 * @param error initialize error if found to that string
 * @return The extracted .string value if found, NULL otherwise.
 */
char* get_dir_string_val(char *line  , char *error) {
    char *directive = ".string";
    char *extracted_value;
    char *result;
    size_t len;
    size_t new_len;
   


    extracted_value = extract_value_after_directive(line, directive);
    
    if (!extracted_value && strstr(line, directive)){
       my_snprintf(error, 256, "Error: empty value for .string directive.");
        return NULL; 
    }
   
    if (extracted_value == NULL) {
        return NULL;
    }
    
     len = strlen(extracted_value);
     
    if (len < 2 || extracted_value[0] != '\"'  || extracted_value[len - 1] != '\"') {
        my_snprintf(error, 256, "Error: Invalid .string directive, missing quotes: %s", extracted_value);
        free(extracted_value);
        return NULL;
    }


     new_len = len - 2;
    result = (char *)malloc(new_len + 1);
    if (result == NULL) {
        printf("Error: Memory allocation failed\n");
        free(extracted_value);
        return NULL;
    }

    strncpy(result, extracted_value + 1, new_len);
    result[new_len] = '\0';

    free(extracted_value);
    return result; 
}

/** 
 * convert string values to ascii values in int
 * @param input The string  received to convert
 * @param output The int we return when we finish converting the input
 * @return The converted value, if one of the values given are NULL we return nothing
 */
void string_to_ascii(const char *input, int *output) {
    int i;
    if (input == NULL || output == NULL) {
        return;
    }

     i = 0;
    while (*input) {
        output[i] = (int) (*input);
        input++;
        i++;
    }

    if (i < 80) {
        output[i] = 0;
    }
}

/** 
 * get the .string directive value , initialize the relevent fields in the ast (string data initialized as int ascii value in the ast field)
 * therfore we will initialize count of the chars and their values as int ascii to the ast.
 * @param node the ast tree node  in which we initialize the info to.
 * @param line given line we use.
 * @return nothing because we just initialize as said above.
 */
void process_string_directive(struct as_tree *node, char *line) {
    char *DirString;
    int len;
    DirString = get_dir_string_val(line,node->error);
    if (DirString != NULL) {
        node->ast_type = ast_dir;
        node->dir.dir_type = ast_string;
         len = strlen(DirString);
        node->dir.operand_type.number_count = len;

        string_to_ascii(DirString, node->dir.operand_type.number);

       

        free(DirString);
    } 
}
  /** 
 * get the .data directive value and count the number of data (numbers) 
 *@param line given line we use.
 *@param ssr struct we use to process the data by spliting the data and validating the data chars
 *@param error initialize error if found to that string 
 *@param linecounter line counter
 * @return  the data counter  if the data is valid otherwise  return -1
 */
  int get_dir_data_count(char *line, struct string_sep_result *ssr , char *error , int linecounter) {
    char *extracted_value;
    char *directive;
    char *start;
    char *end;
    int comma_count;
    char *temp;
    int count;
    int i;
    int j;
    directive = ".data";




    extracted_value = extract_value_after_directive(line, directive);
    
    if (!extracted_value && strstr(line, directive)){
       my_snprintf(error, 256, "Error: empty value for .data directive.");
        return -1; 
    }
   


    if (!extracted_value) {
       free(extracted_value);
        return -1;
    }
    
    

    start = extracted_value;
    while (isspace((unsigned char)*start)) start++;
    if (*start == ',') {
        my_snprintf(error, 256, "Error: Comma at the start of .data directive.");
        free(extracted_value);
        return -1; 
    }

    end = extracted_value + strlen(extracted_value) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    if (*end == ',') {
        my_snprintf(error, 256, "Error: Comma at the end of .data directive.");
        free(extracted_value);
        return -1; 
    }

     comma_count = 0;
    temp = start;
    while (*temp != '\0') {
        if (*temp == ',') {
            comma_count++;
            if (comma_count > 1) {
               my_snprintf(error, 256, "Error: Multiple consecutive commas in .data directive.");
                free(extracted_value);
                return -1; 
            }
        } else if (!isspace((unsigned char)*temp)) {
            comma_count = 0;
        }
        temp++;
    }

    *ssr = split_string(extracted_value);

    count = 0;
    for ( i = 0; i < ssr->strings_count; i++) {
        if (!is_valid_number(ssr->strings[i],linecounter)) {
           my_snprintf(error, 256, "Error: Invalid number in .data directive: %s", ssr->strings[i]);
            for ( j = 0; j < ssr->strings_count; j++) {
                free(ssr->strings[j]);
            }
            free(extracted_value);
            return -1; 
        }
        count++;
    }

    free(extracted_value);
    return count;
}


/** 
 * get the .data directive value and initialize the relevent fields in the ast ( convert using strtol every char to int)
 * therfore we will initialize count of the numbers and their values as int to the ast.
 * @param node the ast tree node  in which we initialize the info to.
 * @param line given line we use.
 * @param linecounter line counter
 * @return nothing because we just initialize as said above.
 */
void process_data_directive(struct as_tree *node, char *line ,int linecounter) {
    struct string_sep_result ssr = {0}; 
    int count;
    int num_index;
    char *endptr;
    long value;
    int i;
    int j;

    count = get_dir_data_count(line, &ssr, node->error, linecounter);
    if (count == -1) {
        return;
    }
   

    memset(node->dir.operand_type.number, 0, sizeof(node->dir.operand_type.number));
     num_index = 0;
    for ( i = 0; i < ssr.strings_count; i++) {
        value = strtol(ssr.strings[i], &endptr, 10);
        if (*endptr != '\0') {
            strncpy(node->error, "Error: Invalid number in .data directive.", sizeof(node->error) - 1);
            node->error[sizeof(node->error) - 1] = '\0'; 
            for ( j = 0; j < ssr.strings_count; j++) {
                free(ssr.strings[j]);
            }
            return;
        }

        node->dir.operand_type.number[num_index] = (int)value;
        num_index++;
    }

    node->dir.operand_type.number_count = num_index;
    node->ast_type = ast_dir;
    node->dir.dir_type = ast_data;

    for ( i = 0; i < ssr.strings_count; i++) {
        free(ssr.strings[i]);
    }

    
}
/** 
 * Check if the instruction  requires operands and if its alone (using is_instruction_requiring_operands to check the word from line )
 * @param line The line  to check
 * @param ast the ast node we use to initialize the error field if we encounter an error
 * @return 0 if the instruction is not alone otherwise return 1
 */
int is_instruction_alone(const char *line , struct as_tree *ast ) {
    const char *delimiters = " \t\n";
    char *token;
    char *line_copy;

    line_copy = strdup1(line);
    if (line_copy == NULL) {
        perror("Memory allocation failed");
        return 0;
    }

    token = strtok(line_copy, delimiters);

    if (token != NULL && is_instruction_requiring_operands(token)) {
        token = strtok(NULL, delimiters);

        if (token == NULL) {
           my_snprintf(ast->error, sizeof(ast->error), "Error: Instruction '%s' requires operands but is alone .", line_copy );
            free(line_copy);
            return 1;
        }
    }

    free(line_copy);
    return 0;
}

/** 
 * check if the word given as argument is an instruction 
 * @param token The word  to check
 * @return return the number of the instruction (following the maman 14 instruction table numbering) otherwise -1
 */
int get_instruction_type(char* token) {
    if (strcmp(token, "mov") == 0) return ast_mov;
    if (strcmp(token, "cmp") == 0) return ast_cmp;
    if (strcmp(token, "add") == 0) return ast_add;
    if (strcmp(token, "sub") == 0) return ast_sub;
    if (strcmp(token, "lea") == 0) return ast_lea;
    if (strcmp(token, "clr") == 0) return ast_clr;
    if (strcmp(token, "not") == 0) return ast_not;
    if (strcmp(token, "inc") == 0) return ast_inc;
    if (strcmp(token, "dec") == 0) return ast_dec;
    if (strcmp(token, "jmp") == 0) return ast_jmp;
    if (strcmp(token, "bne") == 0) return ast_bne;
    if (strcmp(token, "red") == 0) return ast_red;
    if (strcmp(token, "prn") == 0) return ast_prn;
    if (strcmp(token, "jsr") == 0) return ast_jsr;
    if (strcmp(token, "rts") == 0) return ast_rts;
    if (strcmp(token, "stop") == 0) return ast_stop;
    return -1;
}


/** 
 * check if the word given starts with operand attribute
 * @param operand The word  to check
 * @return return the number of the operand type (ast field index )
 */
int get_operand_type(char* operand) {
    if (operand[0] == '#') return ast_immed;
    if (operand[0] == '*') return ast_register_ind;
    if (operand[0] == 'r' ) return ast_register;
    return ast_label;
}


/** 
 * check if the instruction type is allowed to reccive operands by defined  rules for each instruction
 * @param inst_type, represnets what instruction type it can be (as we saw ast indexing)
 * @param operand_type  The operand to check (we know it by ast indexing thats why int)
 * @param operand_position, The operand postion (first after instruction / second  )
 * @return return the number of the instruction (following the maman 14 instruction table numbering) otherwise -1
 */
int is_operand_allowed(int inst_type, int operand_type, int operand_position) {
    switch (inst_type) {
        case ast_mov:
        case ast_add:
        case ast_sub:
            if (operand_position == 0 && (operand_type == ast_immed || operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            if (operand_position == 1 && (operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            break;
        case ast_cmp:
            if (operand_position == 0 && (operand_type == ast_immed || operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            if (operand_position == 1 && (operand_type == ast_immed || operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            break;
        case ast_lea:
            if (operand_position == 0 && operand_type == ast_label) return 1;
            if (operand_position == 1 && (operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            break;
        case ast_clr:
        case ast_not:
        case ast_inc:
        case ast_dec:
            if (operand_position == 0 && (operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            break;
        case ast_jmp:
        case ast_bne:
        case ast_jsr:
            if (operand_position == 0 && (operand_type == ast_label || operand_type == ast_register_ind)) return 1;
            break;
        case ast_red:
            if (operand_position == 0 && (operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            break;
        case ast_prn:
            if (operand_position == 0 && (operand_type == ast_immed || operand_type == ast_label || operand_type == ast_register || operand_type == ast_register_ind)) return 1;
            break;
        case ast_rts:
        case ast_stop:
            return operand_type == ast_none;
        default:
            return 0;
    }
    return 0;
}



/** 
 * check if the operand type is allowed and then check if indeed it has no syntx issues inside the operand defenition itself if so initialize the wanted data to ast
 * @param operand, the actual operand word
 * @param ast  the tree node we initialize and get the inst type from
 * @param operand_index, The index number of the instruction from the table 
 * @param linecounter line counter
 * @return return 1 if the operand initialized well with no syntx error in the operand defenition otherwise return 0 and initialize error to ast 
 */
int parse_operand(char* operand, struct as_tree* ast, int operand_index, int linecounter) {
    int type;
    type = get_operand_type(operand);
    if (!is_operand_allowed(ast->inst.inst_type, type, operand_index)) {
         my_snprintf(ast->error, sizeof(ast->error), "Invalid operand type for instruction number %d at position %d", ast->inst.inst_type, operand_index);
        return 0;
    }
    ast->inst.operands[operand_index].operand_type = type;
    switch (type) {
           case ast_immed:
            if ((operand[1] == '-' && operand[2] >= '0' && operand[2] <= '9') || (operand[1] >= '0' && operand[1] <= '9')) {
                ast->inst.operands[operand_index].operand_option.immed = atoi(operand + 1); 
            } else {
                 my_snprintf(ast->error, sizeof(ast->error), "Invalid immediate value %s", operand);
                return 0;
            }
            break;
        case ast_register_ind:
            if (operand[0] == '*' && operand[1] == 'r' && operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0') {
                ast->inst.operands[operand_index].operand_option.reg = atoi(&operand[2]); 
            } else {
                 my_snprintf(ast->error, sizeof(ast->error), "Invalid indirect register %s", operand);
                return 0;
            }
            break;
        case ast_register:
            if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') { 
                ast->inst.operands[operand_index].operand_option.reg = atoi(&operand[1]); 
            } else {
                 my_snprintf(ast->error, sizeof(ast->error), "Invalid register %s", operand);
                return 0;
            }
            break;
        case ast_label:
            if (!is_valid_label(operand,linecounter)) { 
                 my_snprintf(ast->error, sizeof(ast->error), "Invalid label %s", operand);
                return 0;
            }
            ast->inst.operands[operand_index].operand_option.label = strdup1(operand);
            break;
        default:
             my_snprintf(ast->error, sizeof(ast->error), "Unknown operand type %d", type);
            return 0;
    }
    return 1;
}


int contains_reserved_word(const char *line) {
    int i;
    for ( i = 0; i < reserved_words_count; ++i) {
        if (strstr(line, reserved_words[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

void check_for_no_reserved_words(char *line, struct as_tree *node) {
    if (!contains_reserved_word(line)) {
        my_snprintf(node->error, sizeof(node->error), "Error: Line does not contain any reserved words.");
    }
}

/**
 *  check the line and initialize all relevent fields using the  functions we created so far
 * @param line  line  check.
 * @param linecounter line counter
 * @return ast node if no syntx issues found overall otherwise NULL
 */
struct as_tree* summarize_ast_line(char* line, int linecounter) {
    char* line_copy;
    size_t len;
    char* entryLabel;
    char* externLabel;
    char* label;
    char* label_end;
    char* token;
    int inst_type;
    int operand_count;
    int i;
    struct as_tree* node = (struct as_tree*)malloc(sizeof(struct as_tree));
    if (node == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }
    memset(node, 0, sizeof(struct as_tree));
    node->ast_type = ast_empty;

    line_copy = strdup1(line);
    if (line_copy == NULL) {
        perror("Memory allocation failed");
        free(node);
        return NULL;
    }
     len = strlen(line_copy);
    while (len > 0 && (line_copy[len - 1] == '\n' || line_copy[len - 1] == ' ' || line_copy[len - 1] == '\t')) {
        line_copy[--len] = '\0';
    }

    /*not even one reserved word in the line means stright away the line is invalid  */
    check_for_no_reserved_words(line_copy,node);
    /*if the instruction demands at least one operand and it found alone the line is invalid */
    is_instruction_alone(line_copy, node);

    /*extrect the label after .entry*/
    entryLabel = extract_label_after_directive(line_copy, ".entry", linecounter);
    if (entryLabel != NULL) {
        node->ast_type = ast_dir;
        node->dir.dir_type = ast_entry;
        node->dir.operand_type.label = entryLabel;
    }

    strcpy(line_copy, line);
     while (len > 0 && (line_copy[len - 1] == '\n' || line_copy[len - 1] == ' ' || line_copy[len - 1] == '\t')) {
        line_copy[--len] = '\0';
    }
    /*extrect the label after .extern*/
     externLabel = extract_label_after_directive(line_copy, ".extern", linecounter);
    if (externLabel != NULL) {
        node->ast_type = ast_dir;
        node->dir.dir_type = ast_extern;
        node->dir.operand_type.label = externLabel;
    }

    strcpy(line_copy, line);
    while (len > 0 && (line_copy[len - 1] == '\n' || line_copy[len - 1] == ' ' || line_copy[len - 1] == '\t')) {
        line_copy[--len] = '\0';
    }
    /*extrect  label definition and move past ':'*/
    label = label_defnition_extrector(line_copy, linecounter);

    if (label != NULL) {
        node->labelname = label;

        label_end = strchr(line_copy, ':');
        if (label_end != NULL) {
            label_end++; 
            while (*label_end == ' ' || *label_end == '\t') label_end++; 
            memmove(line_copy, label_end, strlen(label_end) + 1); 
        }
     
    }
    
    
   /*if both label and entry label apper we ignore the label and put to NULL (label definition before entry label)*/
    if (label != NULL && entryLabel != NULL) {
        node->labelname = NULL;
        node->ast_type = ast_dir;
        node->dir.dir_type = ast_entry;
        node->dir.operand_type.label = entryLabel;
        free(label);
    }
    /*if both label and extern label apper we ignore the label and put to NULL (label definition before extern label)*/
    if (label != NULL && externLabel != NULL) {
        node->labelname = NULL;
        node->ast_type = ast_dir;
        node->dir.dir_type = ast_extern;
        node->dir.operand_type.label = externLabel;
        free(label);
    }
    /*if its a string definition initialize otherwise continue */
    process_string_directive(node, line_copy);
    /*if its a data definition initialize otherwise continue */
    process_data_directive(node, line_copy, linecounter);
     
   
    
     token = strtok(line_copy, " ");
    
     len = strlen(token);
    if (len > 0 && token[len - 1] == '\n') {
        token[len - 1] = '\0';
    }
    

    /*get the instruction type if exist from the token we prased*/
    inst_type = get_instruction_type(token);

    if (inst_type == -1) {
        free(line_copy);
        return node;
    }
    /*if so initialize */
    node->ast_type = ast_inst;
    node->inst.inst_type = inst_type;
    /*count the operands to later know how many we initialize in the tree */
    operand_count = 0;
    for ( i = 0; i < 2; ++i) {
        /*skip , to differentiate betwen operands*/
        token = strtok(NULL, ", ");
        /*found token  otherwise token dosent exist  and its initialized to  none*/
        if (token) {
            /*if operand is not valid return node and check second*/
            if (!parse_operand(token, node, i,linecounter)) {
                free(line_copy);
                return node;
            }
            /*found operand increase counter*/
            operand_count++;
        } else {
            node->inst.operands[i].operand_type = ast_none;
        }
    }
      /*if its solo operand it must be destination and index 1  considerd destination*/
      if (operand_count == 1) {
        node->inst.operands[1] = node->inst.operands[0];
        node->inst.operands[0].operand_type = ast_none;
    }


    free(line_copy);
    return node;
}


/**
 *  freeing the ast tree
 * @param node the node to free (tree content)
 * @return nothing we just free
 */
void free_ast(struct as_tree *node) {
    int i;
    if (node == NULL) {
        return;
    }

    if (node->labelname != NULL) {
        free(node->labelname);
    }

    if (node->ast_type == ast_inst) {
        for ( i = 0; i < 2; ++i) {
            if (node->inst.operands[i].operand_type == ast_label) {
                if (node->inst.operands[i].operand_option.label != NULL) {
                    free(node->inst.operands[i].operand_option.label);
                }
            }
        }
    } else if (node->ast_type == ast_dir) {
        if (node->dir.dir_type == ast_extern || node->dir.dir_type == ast_entry) {
            if (node->dir.operand_type.label != NULL) {
                free(node->dir.operand_type.label);
            }
        }
    }
   

    free(node);
}





