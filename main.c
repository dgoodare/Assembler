#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "assembler.h"

int main()
{
	initialiseInstructions();

	initialiseBinary();

	initialiseSymbolTable();
	
	initialiseOutputCodeBuffer();

	/*----PASS 1----*/
	//remove unnecessary characters from the assembly language file
	removeCommentsAndSpaces();
	
	resolveSymbols();

	for (int i = 0; i < MAXENTRIES; i++)
	{
		printSymbol(SymbolTable[i]);
	}

	printBuffer();
	return 1;
}