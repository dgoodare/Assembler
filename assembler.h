#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

/*----Constants----*/
#define MAXLENGTH 24//max number of characters in a symbol or variable name
#define MAXENTRIES 256//max number of entries in the symbol table
#define MAXCHAR 128//max number of character that will be read per line

/*----Data Structures----*/
typedef struct instruction
{
	char mnem[3];//mnemonic
	char binary[4];//binary equivalent
} Instruction;

typedef struct symbolEntry
{
	char symbol[MAXLENGTH];
	int address;
} SymbolEntry;

/*----Global Variables----*/
Instruction InstructionSet[8];//the instruction set is an array of instruction data structures
SymbolEntry SymbolTable[MAXENTRIES];//the symbol table is represented as an array of Symbol table entries

/*----Function declarations----*/
void initialiseInstructions();
void initialiseSymbolTable();
bool addSymbolEntry(SymbolEntry newSymbol);
int getAddress(char *symbol);
void printSymbol(SymbolEntry symbol);
void removeCommentsAndSpaces();
int findString(const char* substring);
void resolveSymbols();
void processCommand(char command[]);
bool isInstruction(const char* str);
