#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assembler.h"

int main()
{
	SymbolEntry symbol;

	int instructionCount = 0;//the number of instructions
	int lineNumber = 0;//the number of lines that have been read

	//temporary variables to store symbols
	char tempSymbol1[MAXCHAR];
	char tempSymbol2[MAXCHAR];

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