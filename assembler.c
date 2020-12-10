#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "assembler.h"

/**
* This function will initialise the mnemonics for the instruction set.
* I originally had the mnemonics and their binary equivalents stored in the same data structure, but for some reason
* when initialising the binary, it would change the mnemonic as well, so "JMP" would become "JMP000". 
* So as a workaround, i split them up, which isn't ideal.
* One possible solution is to store the binary as an integer instead and only convert it to binary later.
*/
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
}

/**
* Initialise Binary Codes for Instructions
* As mentioned above, this is how I've initialised the binary code equivalents for the instructions. It's 
* obviously not ideal, but it works for now.
*/
void initialiseBinary()
{
	strcpy(InstructionBinary[0].binary, "00000");
	strcpy(InstructionBinary[1].binary, "00100");
	strcpy(InstructionBinary[2].binary, "00010");
	strcpy(InstructionBinary[3].binary, "00110");
	strcpy(InstructionBinary[4].binary, "00001");
	strcpy(InstructionBinary[5].binary, "00101");
	strcpy(InstructionBinary[6].binary, "00011");
	strcpy(InstructionBinary[7].binary, "00111");
}

/**
* This function will return the mnemonic for the intruction passed into it
*/
char* getMnem(Instruction ins)
{
	char * str;
	str	= malloc(sizeof(char)*3);

	strcpy(str, ins.mnem);
	return str;
}

/**
* Initialise the entries in the symbol table with "\0" as the symbol name
* I have been using -1 for symbols that do not have an address
*/
void initialiseSymbolTable()
{
	for (int i = 0; i < MAXENTRIES; i++)
	{
		strcpy(SymbolTable[i].symbol, "\0");
		SymbolTable[i].address = -1;
	}
}

/**
* This will set up the output code buffer with the addresses, which as far as I can tell, are just linear
* and in ascending order from 0 - MAXBUFFER.
* And If I understand it correctly, since the Manchester Baby increments its instruction counter before 
* fetching the instruction, the code in the first address will be ignored.
*/
void initialiseOutputCodeBuffer()
{
	//the first entry must be initialised differently
	OutputCodeBuffer[0].address = 0;
	strcpy(OutputCodeBuffer[0].binary, "00000");

	for (int i = 1; i < MAXBUFFER; i++)
	{
		OutputCodeBuffer[i].address = i;
		strcpy(OutputCodeBuffer[i].binary, "");
	}

	//set address counter to 1
	addressCounter = 1;
}

/**
* This adds a new symbol to the Symbol Table
*/
bool addSymbolEntry(SymbolEntry newSymbol)
{
	//first, check if the symbol already exists in the table
	if (isSymbol(newSymbol.symbol))
	{
		return false;
	}

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

/**
* Returns the address of a symbol
* As it stands, this function doesn't get used anywhere in Pass 1, but might be useful in Pass 2
* so I've left it in. This is quite a messy way of returning a value, so could probably be changed. 
*/
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

/**
* Prints a symbol's information
*/
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
* Function to find the line containing the substring passed into it
* This isn't being used at the moment, but might be useful in Pass 2 so I've left it in for now.
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
* Resovles user-defined symbols in the assembly code
*
* For labels like 'START' or 'END' it will always be the first thing in a line and will always be 
* followed by a ':', so they can be easily identified and extracted from the assembly code.
* These labels' addresses will be known and can be added to the symbol table in their completed form
*
* For other symbols (variables) like 'NUM01', their addresses are unknown until Pass 2, so they will
* be added to the Symbol Table with an address of -1. This doesn't happen in this function though, 
* they will be added when a command e.g "LDNNUM01" is processed when processCommand() is called.
*/
void resolveSymbols()
{
	FILE *fp = fopen("intermediate.txt", "r");
	if (!fp)
	{
		printf("Error opening file\n");
		return;
	}

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
				//Symbols like 'START' and 'END' get added to the table here
				SymbolEntry newSymbol;
				strcpy(newSymbol.symbol, str);
				newSymbol.address = addressCounter;
				//add it to the symbol table
				addSymbolEntry(newSymbol);
				//increment address counter
				addressCounter++;
			}
			
			//send the rest of the current line to be processed and added to the output code buffer (if necessary)
			str = strtok(NULL, ":");
			processCommand(str);
		}
		else if ((currentLine[1] != '\0') && (currentLine[0] != '\n'))
		{
			processCommand(currentLine);
		}
		
	}
	fclose(fp);
}

/**
* This function processes commands like 'LDNNUM01' and 'SUBNUM02'
* It will check if the instruction matches an instruction from the instruction set, if it doesn't exit(0) is called.
* 
*/
void processCommand(char command[])
{
	printf("Command being processed: %s", command);
	char tempStr1[3];//temp variable to store the first 3 characters in the command
	tempStr1[0] = command[0];
	tempStr1[1] = command[1];
	tempStr1[2] = command[2];

	int length = strlen(command) - 3;

	char tempStr2[length];//temp variable to store the rest of the command

	for (int i = 0; i < length; i++)
	{
		if ((command[i+3] != '\n') && (command[i+3] != '\0'))
			tempStr2[i] = command[i+3];
	}
	
	//check if the command is a variable declaration, if it is, ignore it for now
	if (strcmp(tempStr1, "VAR") == 0)
	{
		return;
	}

	//check if the start of the command is an instruction
	if (!isInstruction(tempStr1))
	{
		printf("ERROR: command is not in the instruction set\n");
		exit(0);
	}

	//put command binary into the output code buffer
	addToBuffer(tempStr1);
	
	//check if the rest of the command is a number
	if (!isdigit(tempStr2[0]))
	{
		//Symbols like 'NUM01' get added here
		//create a new symbol and assign its name
		SymbolEntry newSymbol;
		strcpy(newSymbol.symbol, tempStr2);
		newSymbol.address = -1;
		//add it to the symbol table
		addSymbolEntry(newSymbol);
		addressCounter++;
	}
	/*
	* There is a case where a command is called directly onto a number e.g. 'LDN5', but I haven't implemented this
	* yet since it does not appear in the assembly code file we've been given.
	*/
}

/**
* This function will check if a command matches a one of the pre-deifined instructions
*/
bool isInstruction(const char* str)
{
	bool isInstruction = false;

	for (int i = 0; i < 8; i++)
	{
		if (strcmp(str, getMnem(InstructionSet[i])) == 0)
			isInstruction = true;
	}

	return isInstruction;
}

/**
* Checks if a symbol already exists in the Symbol Table
*/
bool isSymbol(const char* str)
{
	bool isSymbol = false;

	for (int i = 0; i < MAXENTRIES; i++)
	{
		if(strcmp(str, SymbolTable[i].symbol) == 0)
		{
			isSymbol = true;
			break;
		}
	}

	return isSymbol;
}

/**
* This function is called binary is being added to the Output Buffer
* This is definitely going to need some work. I initially thought it would be better to store 
* the binary codes as strings, but this has caused some issues, so it might be better storing
* them as integers to be converted into binary later.
*/
void addToBuffer(const char* str)
{
	char tempbinary[5];
	printf("Instruction %s received\n", str);

	for(int i = 0; i < 8; i++)
	{
		if (strcmp(str, getMnem(InstructionSet[i])) == 0)
		{

			printf("Match found\n");
			strcpy(tempbinary, InstructionBinary[i].binary);
			printf("binary code: %s\n", tempbinary);
			break;
		}
	}

	strcpy(OutputCodeBuffer[addressCounter].binary, tempbinary);
}

/**
* Prints the first 10 entries in the buffer
* It only prints the first 10 for now since it has 256 spaces and only a handful will be used
*/
void printBuffer()
{
	printf("First 10 entries in Output buffer:\n");
	for (int i = 0; i < 10; i++)
	{
		printf("Binary stored in address %d is %s\n", i, OutputCodeBuffer[i].binary);
	}
}