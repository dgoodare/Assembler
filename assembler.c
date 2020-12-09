#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "assembler.h"

//initialise the symbol table to include the Baby's pre-set instruction set. 
//These symbols are not defined by the user and shouldn't accidently be duplicated in the symbol table
void initialiseInstructions()
{
	//initialise keywords for the instruction set
	strcpy(InstructionSet[0].mnem, "JMP");
	strcpy(InstructionSet[1].mnem, "JRP");
	strcpy(InstructionSet[2].mnem, "LDN");
	strcpy(InstructionSet[3].mnem, "STO");
	strcpy(InstructionSet[4].mnem, "SUB");
	strcpy(InstructionSet[5].mnem, "SUB");
	strcpy(InstructionSet[6].mnem, "CMP");
	strcpy(InstructionSet[7].mnem, "STP");

	//initialise binary for the instruction set
	/*
	strcpy(InstructionSet[0].binary, "000");
	strcpy(InstructionSet[1].binary, "100");
	strcpy(InstructionSet[2].binary, "010");
	strcpy(InstructionSet[3].binary, "110");
	strcpy(InstructionSet[4].binary, "001");
	strcpy(InstructionSet[5].binary, "101");
	strcpy(InstructionSet[6].binary, "011");
	strcpy(InstructionSet[7].binary, "111");
*/
}

char* getMnem(Instruction ins)
{
	char * str;
	str	= malloc(sizeof(char)*3);

	strcpy(str, ins.mnem);
	return str;
}

//initialise entries in the table
//NULL character for symbol
//-1 for address
void initialiseSymbolTable()
{
	/**
	* the Manchester Baby program starts at 1, the 0th address needs to be filled
	* this is what the 1st line (VAR 0) is doing in the assembly code
	* I've opted to do this automatically to avoid potential errors
	*/
	strcpy(SymbolTable[0].symbol, "0");
	SymbolTable[0].address = 0;

	for (int i = 1; i < MAXENTRIES; i++)
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
		char *p;//pointer to the first char of substring if it is found
		p = strstr(currentLine, substring);//returns a null pointer if the substring isn't found

		if (p)
		{
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
* since a label is always the first thing in a line and will always be followed by a ':'
* they can be easily identified and extracted from the assembly code
*/
void resolveSymbols()
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
		char *p;//pointer to the first char of substring if it is found
		p = strchr(currentLine, ':');//looks for : in the current line

		if (p)
		{
			//splits the line by ':' text before it will be the symbol's name
			char *str = strtok(currentLine, ":");

			if (!isdigit(str[0]))
			{
				SymbolEntry newSymbol;
				strcpy(newSymbol.symbol, str);
				newSymbol.address = -1;
				//add it to the symbol table
				addSymbolEntry(newSymbol);
			}
			
			//send the rest of the current line to be processed and added to the output code buffer (if necessary)
			str = strtok(NULL, ":");
			processCommand(str);
		}
		else if ((currentLine[1] != '\0') && (currentLine[0] != '\n'))
		{
			processCommand(currentLine);
		}
		
		lineNumber++;
	}
	fclose(fp);
}

void processCommand(char command[])
{
	//printf("Command being processed: %s", command);
	char tempStr1[3];//temp variable to store the first 3 characters in the command
	tempStr1[0] = command[0];
	tempStr1[1] = command[1];
	tempStr1[2] = command[2];

	int length = strlen(command) - 3;

	char tempStr2[length];//temp variabel to store the rest of the command

	for (int i = 0; i < length; i++)
	{
		if ((command[i+3] != '\n') && (command[i+3] != '\0'))
			tempStr2[i] = command[i+3];
	}

	//printf("start of command: %s\n", tempStr1);
	//printf("rest of command: %s\n", tempStr2);
	
	//check if the command is a variable declaration, if it is, ignore it for now
	if (strcmp(tempStr1, "VAR") == 0)
	{
		printf("variable declaration, moving to next line\n");
		return;
	}

	//check if the start of the command is an instruction
	if (!isInstruction(tempStr1))
	{
		printf("ERROR: command is not in the instruction set\n");
		exit(0);
	}
	
	//check if the rest of the command is a number
	if (!isdigit(tempStr2[0]))
	{
		printf("%s\n", tempStr2);
		printf("rest of command is a symbol, adding to table\n");
		//create a new symbol and assign its name
		SymbolEntry newSymbol;
		strcpy(newSymbol.symbol, tempStr2);
		newSymbol.address = -1;
		//add it to the symbol table
		addSymbolEntry(newSymbol);
	}
}

//checks if a string is in the instruction set
bool isInstruction(const char* str)
{
	bool isInstruction = false;

	//printf("%s\n", str);

	for (int i = 0; i < 8; i++)
	{
		if (strcmp(str, getMnem(InstructionSet[i])) == 0)
			isInstruction = true;
	}

	return isInstruction;
}