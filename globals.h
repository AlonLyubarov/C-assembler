extern const char *reserved_words[];
extern const char *instructions_requiring_operands[];
extern const int reserved_words_count;
extern const int instructions_count;
int is_reserved_word(const char *word);
int is_instruction_requiring_operands(const char *word);


int my_snprintf(char *str, size_t size, const char *format, ...);

char *strdup1(const char *s);
