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

/**
* This function is the first to be called in PASS 1 
* and will remove unnecessary characters and spacing from the assembly language file
*/
void removeCommentsAndSpaces()
{
	//declaring pointers to files
	//the starting file being read from in pass 1
	FILE *inputf;
	//intermediate file, that has the same content as inputf, but with comments and whitespace removed
	//this file is written to in pass 1 and read from in pass 2
	FILE *interf;

	char currentCommand[MAXCHAR];//string to store the command being processed, with buffer of MAXCHAR (80) characters
	char processedCommand[MAXCHAR];//processed version of currentCommand, with comments and whitespace removed

	//open files 
	//WILL EVENTUALLY IMPLEMENT A MORE ROBUST METHOD FOR OPENING FILES
	inputf = fopen("assembly.txt", "r");//open the assembly language file for reading
	interf = fopen("intermediate.txt", "w");//open or create the intermediate file

	if((!inputf) || (!interf))
	{
		printf("Error opening file\n");
		return;
	}

	//Reads in one line at a time and removes tabs, spaces and comments
	while (fgets(currentCommand, MAXCHAR, inputf) != NULL)
	{
		//process the command stored in currentCommand
		//counter variables to refer to each character in the two command variables
		int c = 0;//counter for currentCommand
		int p = 0;//counter for processedCommand

		//initialise processedCommand to NULL
		processedCommand[0] = '\0';

		//remove tabs and spaces
		while (currentCommand[c] != '\n')
		{
			if ((currentCommand[c] != '\t') && (currentCommand[c] != ' '))
			{
				processedCommand[p++] = currentCommand[c];
			}
			c++;
		}

		//add newline character and end-of-string character (null character) onto the end
		processedCommand[p] = '\n';
		processedCommand[p+1] = '\0';

		//remove comments
		p = 0;//reset counter
		while (processedCommand[p] != '\n')
		{
			if (processedCommand[p] == ';')
			{
				//replace comment with newline and null characters
				processedCommand[p] = '\n';
				processedCommand[p+1] = '\0';
				break;
			}
			p++;
		}
		//write the processed command into the intermediate file
		fputs(processedCommand, interf);
	}

	fclose(inputf);
	fclose(interf);
}

/**
* function to find the line containing the substring
* used to find START and END labels in PASS 1
*/
int findString(const char* substring)
{
	FILE *fp = fopen("intermediate.txt", "r");
	if (!fp)
	{
		printf("Error opening file\n");
		return -1;
	}

	int lineNumber = 0;
	char currentLine[MAXCHAR];

	while (fgets(currentLine, MAXCHAR, fp) != NULL)
	{
		//printf("Searching at line %d\n", lineNumber);
		char *p;//pointer to the first char of substring if it is found
		p = strstr(currentLine, substring);//returns a null pointer if the substring isn't found

		if (p)
		{
			//printf("String found at line %d\n", lineNumber);
			fclose(fp);
			return lineNumber;
		}
		else
		{
			lineNumber++;
		}
	}

	fclose(fp);
	return -1;
}

/**
* resovles user-defined symbols in the assembly code
* since variable declarations happen outside of the START-END section there is no need to look there
* hence, the positions of START and END are passed into the function.
*
* 
*/
void resolveSymbols(int startPos, int endPos)
{
	FILE *fp = fopen("intermediate.txt", "r");
	if (!fp)
	{
		printf("Error opening file\n");
		return;
	}

	int lineNumber = 0;
	char currentLine [MAXCHAR];

	while (fgets(currentLine, MAXCHAR, fp) != NULL)
	{
		if ((lineNumber >= startPos) && (lineNumber <= endPos))
		{
			lineNumber++;
			continue;
		}

		char *p;//pointer to the first char of substring if it is found
		p = strchr(currentLine, ':');//looks for : in the current line

		if (p)
		{
			//splits the line by ':' text before it will be the variable's name
			char *symbolName = strtok(currentLine, ":");
			//create a new symbol and assign its name
			SymbolEntry newSymbol;
			strcpy(newSymbol.symbol, symbolName);
			newSymbol.address = -1;
			//add it to the symbol table
			bool added = addSymbolEntry(newSymbol);
			if (added)
			{
				printf("Symbol added\n");
			}
			else
			{
				printf("Symbol not added\n");
			}
		}
		
		lineNumber++;
	}
}