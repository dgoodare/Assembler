#include <string.h>
#include <stdio.h>
#include "assembler.h"

int main()
{
	initialiseInstructions();

	SymbolEntry TEST1, TEST2, TEST3, TEST4;

	strcpy(TEST1.symbol, "TEST1");
	TEST1.address = 7;

	strcpy(TEST2.symbol, "TEST2");
	TEST2.address = 8;

	strcpy(TEST3.symbol, "TEST3");
	TEST3.address = 9;

	strcpy(TEST4.symbol, "TEST1");

	addSymbolEntry(TEST1);
	addSymbolEntry(TEST2);
	addSymbolEntry(TEST3);

	for (int i = 0; i < MAXENTRIES; i++)
	{
		printSymbol(SymbolTable[i]);
	}
}