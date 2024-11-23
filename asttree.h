#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct as_tree {
     char error[80];
     char* labelname;
    enum {
        ast_inst,
        ast_dir,
        ast_empty
    } ast_type;
    struct {
        enum {
            ast_extern,
            ast_entry,
            ast_string,
            ast_data
        } dir_type;
        struct {
            char *label;
            int number[80];
            int number_count;
        } operand_type;
    } dir;
    struct {
        enum {
            ast_mov,
            ast_cmp,
            ast_add,
            ast_sub,
            ast_lea,
            ast_clr,
            ast_not,
            ast_inc,
            ast_dec,
            ast_jmp,
            ast_bne,
            ast_red,
            ast_prn,
            ast_jsr,
            ast_rts,
            ast_stop
        } inst_type;
        struct {
            enum {
                ast_none = -1,
                ast_immed,
                ast_label,
                ast_register_ind,
                ast_register
            } operand_type;
            union {
                char *label;
                int immed;
                int reg;
            } operand_option;
        } operands[2];
    } inst;
};



struct as_tree* summarize_ast_line(char *line, int linecounter);

void free_ast(struct as_tree *node);

