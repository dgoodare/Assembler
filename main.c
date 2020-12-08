#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assembler.h"

int main()
{
	//initialise the symbol table with instructions
	initialiseInstructions();

	/*----PASS 1----*/
	//remove unnecessary characters from the assembly language file
	removeCommentsAndSpaces();
	//find START and END in the intermediate file
	int startPos = findString("START");
	int endPos = findString("END");
	printf("START: %d END: %d\n", startPos, endPos);

	if (startPos == -1)
	{
		printf("an error occurred, or the string could not be found\n");
	}
	else
	{
		printf("START found in line %d\n", startPos);
	}

	if (endPos == -1)
	{
		printf("an error occurred, or the string could not be found\n");
	}
	else
	{
		printf("END found in line %d\n", endPos);
	}

	resolveSymbols(startPos, endPos);

	for (int i = 0; i < MAXENTRIES; i++)
	{
		printSymbol(SymbolTable[i]);
	}
}