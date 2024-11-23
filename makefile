CC = gcc
CFLAGS = -ansi -pedantic -Wall -g
TARGET = a.out
SOURCES = languagepraser.c symbolTable.c globals.c passnumber1.c passnumber2.c TranslationFunctions.c preprocessor.c main.c
HEADERS = asttree.h globals.h TranslationFunctions.h symbolTable.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
