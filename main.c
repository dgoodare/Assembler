#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assembler.h"

int main()
{
	SymbolEntry symbol;

	int instructionCount = 0;//the number of instructions
	int lineNumber = 0;//the number of lines that have been read

	//declaring pointers to files
	//the starting file being read from in pass 1
	FILE *inputf;
	//intermediate file, that has the same content as inputf, but with comments and whitespace removed
	//this file is written to in pass 1 and read from in pass 2
	FILE *interf;
	//the final binary output file, this is written to in pass 2
	FILE *outputf;

	char currentCommand[MAXCHAR];//string to store the command being processed, with buffer of MAXCHAR (80) characters
	char processedCommand[MAXCHAR];//processed version of currentCommand, with comments and whitespace removed

	//temporary variables to store symbols
	char tempSymbol1[MAXCHAR];
	char tempSymbol2[MAXCHAR];

	//initialise the symbol table with instructions
	initialiseInstructions();

	//open files 
	//WILL EVENTUALLY IMPLEMENT A MORE ROBUST METHOD FOR OPENING FILES
	inputf = fopen("assembly.txt", "r");//open the assembly language file for reading
	interf = fopen("intermediate.txt", "w");//open or create the intermediate file

	/*----PASS 1 ----*/
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
				c++;
			}
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


	}
}