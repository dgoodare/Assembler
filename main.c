#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "assembler.h"

int main()
{
	//initialise the instruction set
	initialiseInstructions();
	//initialise the symbol table
	initialiseSymbolTable();

	/*----PASS 1----*/
	//remove unnecessary characters from the assembly language file
	removeCommentsAndSpaces();
	
	resolveSymbols();

	for (int i = 0; i < MAXENTRIES; i++)
	{
		printSymbol(SymbolTable[i]);
	}
}