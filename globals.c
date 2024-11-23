#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "globals.h"


const char *reserved_words[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", 
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
    ".data", ".string", ".entry", ".extern",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};
const int reserved_words_count = 28;

/** 
 * we check if the word is a reserved word.
 * @param word The word to check.
 * @return 1 if the word is reserved, 0 otherwise.
 */
int is_reserved_word(const char *word) {
    int i;
    for ( i = 0; i < reserved_words_count; i++) {
        if (strcmp(word, reserved_words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

const char *instructions_requiring_operands[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", 
    "jmp", "bne", "red", "prn", "jsr"
};

const int instructions_count = sizeof(instructions_requiring_operands) / sizeof(instructions_requiring_operands[0]);

/** 
 * we check if instruction is requiring operands
 * @param word The instruction  to check.
 * @return 1 if the instruction requiring, 0 otherwise.
 */
int is_instruction_requiring_operands(const char *word) {
    int i;
    for (i = 0; i < instructions_count; i++) {
        if (strcmp(word, instructions_requiring_operands[i]) == 0) {
            return 1; 
        }
    }
    return 0; 
}

/** 
 * strdup alter duplictae the string
 * @param s The string duplicate
 * @return 1 if  duplicated otherwise NULL
 */
char *strdup1(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = (char *)malloc(len);  
    if (dup == NULL) {
        return NULL;  
    }
    memcpy(dup, s, len);  
    return dup;
}

/** 
 * snprintf alter, copy with max range and string formatting
 * @param str  destination buffer that the formatted string will be copied to
 * @param size  maximum number of characters to write to the buffer (including the null terminator) 
 * @param format the format string that specifies how to format the input 
 * @return  the number of characters that would be written
 */
int my_snprintf(char *str, size_t size, const char *format, ...) {
    int n;
    va_list args;
    char buffer[1024];

    va_start(args, format);
    n = vsprintf(buffer, format, args);
    va_end(args);

    if (n < 0) {
        return n; 
    }

    if ((size_t)n < size) {
        strncpy(str, buffer, n);
        str[n] = '\0';
    } else if (size > 0) {
        strncpy(str, buffer, size - 1);
        str[size - 1] = '\0';
    }

    return n; 
}