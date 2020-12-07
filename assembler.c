#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "assembler.h"

//initialise the symbol table to include the Baby's pre-set instruction set. 
//These symbols are not defined by the user and shouldn't accidently be duplicated in the symbol table
void initialiseInstructions()
{
	//initialise keywords for the instruction set
	strcpy(SymbolTable[0].symbol, "JMP");
	strcpy(SymbolTable[1].symbol, "LDN");
	strcpy(SymbolTable[2].symbol, "STO");
	strcpy(SymbolTable[3].symbol, "SUB");
	strcpy(SymbolTable[4].symbol, "SUB");
	strcpy(SymbolTable[5].symbol, "CMP");
	strcpy(SymbolTable[6].symbol, "STP");

	//initialise addresses for the instruction set
	SymbolTable[0].address = 0;
	SymbolTable[1].address = 1;
	SymbolTable[2].address = 2;
	SymbolTable[3].address = 3;
	SymbolTable[4].address = 3;//duplicate values for SUB
	SymbolTable[5].address = 5;
	SymbolTable[6].address = 6;

	//initialise remaining entries in the table
	//NULL character for symbol
	//-1 for address
	for (int i = 7; i < MAXENTRIES; i++)
	{
		strcpy(SymbolTable[i].symbol, "\0");
		SymbolTable[i].address = -1;
	}
}

//adds a new symbol to the table, returns true is the symbol was successfully added, false if not
bool addSymbolEntry(SymbolEntry newSymbol)
{
	//first, check if the symbol already exists in the table
	int counter = 0;//counter variable to use in the while loop

	//while the two strings are not the same
	//AND
	//the symbol is not the null character
	//AND
	//counter is less than MAXENTRIES
	while ((strcmp(SymbolTable[counter].symbol, newSymbol.symbol) != 0) && (strcmp(SymbolTable[counter].symbol, "\0") != 0) && (counter < MAXENTRIES))
		counter++;

	//CASE 1: the symbol is not in the table yet
	if (strcmp(SymbolTable[counter].symbol, "\0") == 0)
	{
		strcpy(SymbolTable[counter].symbol, newSymbol.symbol);
		SymbolTable[counter].address = newSymbol.address;
		return true;
	}
	//CASE 2: the symbol is already in the table, but does not have an address (address is -1)
	if ((strcmp(SymbolTable[counter].symbol, newSymbol.symbol) == 0) && (SymbolTable[counter].address == -1))
	{
		SymbolTable[counter].address = newSymbol.address;
		return true;
	}
	//CASE 3: the symbol table is full
	if (counter >= MAXENTRIES)
	{
		//display error message
		printf("ERROR: the symbol table has reached its maximum capacity, unable to add symbol\n");
		return false;
	}
}

//called in the second pass to make sure all symbols have been assigned an address
int getAddress(char *symbol)
{
	int counter = 0;//counter variable to use in the while loop

	//if the symbol is found in the table, return its address
	while((counter < MAXENTRIES) && (SymbolTable[counter].address != -1))
	{
		if(strcmp(SymbolTable[counter].symbol, symbol) == 0)
			return SymbolTable[counter].address;
		counter++;
	}

	//if the symbol is not found, return -1
	return -1;
}

//prints a symbol in the symbol table
void printSymbol(SymbolEntry symbol)
{
	if (strcmp(symbol.symbol, "\0") != 0)
		printf("SYMBOL: %s ADDRESS: %d\n", symbol.symbol, symbol.address);	
}