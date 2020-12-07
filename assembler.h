#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/*----Constants----*/
#define MAXLENGTH 24//max number of characters in a symbol or variable name
#define MAXENTRIES 256//max number of entries in the symbol table
#define MAXCHAR 128//max number of character that will be read per line

/*----Data Structures----*/
typedef struct symbolEntry
{
	char symbol[MAXLENGTH];
	int address;
} SymbolEntry;

/*----Global Variables----*/
SymbolEntry SymbolTable[MAXENTRIES];//the symbol table is represented as an array of Symbol table entries

/*----Function declarations----*/
void initialiseInstructions();
bool addSymbolEntry(SymbolEntry newSymbol);
int getAddress(char *symbol);
void printSymbol(SymbolEntry symbol);