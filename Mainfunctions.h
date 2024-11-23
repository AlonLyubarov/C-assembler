



 int pass_one(const char *fileNameAM, FILE *amFile, struct symbol **symbolListHead, struct symbol *entries[], int *entriesCount, struct compilation_part *unit);
 int pass_two(const char *fileNameAM, FILE *amFile, struct symbol **symbolListHead, struct compilation_part *unit);
 void preprocessor(const char* inputFileName, const char* outputFileName ,int *error);
