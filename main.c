#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "assembler.h"

int main()
{
	/*----INITIALISATION----*/
	initialiseInstructions();

	//initialiseBinary();

	initialiseSymbolTable();
	
	initialiseOutputCodeBuffer();

	/*----PASS 1----*/
	//remove unnecessary characters from the assembly language file
	removeCommentsAndSpaces();
	//look for symbols in the assembly code and add them to the Symbol table with its address (if it is known)
	resolveSymbols();
	//prints the symbol table
	for (int i = 0; i < MAXENTRIES; i++)
	{
		printSymbol(SymbolTable[i]);
	}
	//prints the buffer
	//printBuffer();

	//generate the binary for the buffer entries
	for (int i = 0; i < 10; i++)
	{
		generateBinary(i);
	}

	//printBuffer();

	writeBufferToFile();
	return 1;
}