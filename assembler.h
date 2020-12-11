#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

/*----Constants----*/
#define MAXLENGTH 24//max number of characters in a symbol or variable name
#define MAXENTRIES 256//max number of entries in the symbol table
#define MAXCHAR 128//max number of character that will be read per line
#define MAXBUFFER 256//max number of lines in the output code buffer

/*----Data Structures----*/
typedef struct instruction
{
	char mnem[4];//mnemonic for an instruction
	int binaryInt;//binary for the instruction represented as an integer
} Instruction;

/*
typedef struct binary
{
	char binary[6];//binary equivalent for an instruction
} Binary;
*/
typedef struct symbolEntry
{
	char symbol[MAXLENGTH];//symbol's name
	int address;//symbol's address
} SymbolEntry;

typedef struct bufferEntry
{
	int address;//The index for an address
	char binary[MAXCHAR];//the binary stored at the address
	bool complete;//used in Pass 2 to determine which entries need to be filled in
	char command[MAXCHAR];//stores the command to be processed in Pass 2
} BufferEntry;

/*----Global Variables----*/
Instruction InstructionSet[8];//the instruction set is an array of instruction data structures
//Binary InstructionBinary[8];//an array of the binary equivalents of the instruction mnemonics
SymbolEntry SymbolTable[MAXENTRIES];//the symbol table is represented as an array of Symbol table entries
BufferEntry OutputCodeBuffer[MAXBUFFER];//the output code buffer
int addressCounter;//this will increment if a symbol is discovered

/*----Function declarations----*/
void initialiseInstructions();
void initialiseBinary();
char* getMnem(Instruction ins);
void initialiseSymbolTable();
void initialiseOutputCodeBuffer();
bool addSymbolEntry(SymbolEntry newSymbol);
int getAddress(char *symbol);
void printSymbol(SymbolEntry symbol);
void removeCommentsAndSpaces();
int findString(const char* substring);
void resolveSymbols();
void processCommand(char command[]);
bool isInstruction(const char* str);
int getInstruction(const char* str);
bool isSymbol(const char* str);
void addToBuffer(const char* str, bool complete);
void printBuffer();
void writeBufferToFile();
unsigned int convertToBinary(unsigned i);
char* itoa(int value, char* result, int base);
char* chopN(char *str, int n);
void generateBinary(int index);
char* generateVariableBinary(char * cmd);
