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
	
	/*----PASS 2----*/
	//generate the binary for the buffer entries
	for (int i = 1; i < 15; i++)
	{
		generateBinary(i);
	}

	writeBufferToFile();
	return 1;
}