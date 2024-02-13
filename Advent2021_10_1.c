#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Variations.h"

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[200];
	char* InputPtr;
	int InputPos, InputLen=0;
	int NrOfLines=0;

	if (Arguments == 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n", Argument[1]);
			exit(2);
		}
	}
	else if (Arguments > 2)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s [InputFile]\n", Argument[0]);
		fprintf(stderr, "\twith no [InputFile] argument, data will be read from stdin\n");
		exit(1);
	}

	char Stack[100];
	int StackPtr;
	int ErrorCount=0, ErrorScore=0;

	while (fgets(InputLine, 200, InputFile))
	{
	///////////////////////////    Get one full line of digits     ///////////////////////////
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		InputLen = strlen(InputLine);
		StackPtr=0;
		for (InputPos=0; InputPos<InputLen; InputPos++)
		{
			if (InputPos >= 200)
			{
				fprintf(stderr, "No storage for more than 100 chars on an InputLine\n");
				exit(3);
			}
			int NewOpening=0,MismatchFound=0,Underflow=0;
			// Parse this particular character
			switch (InputLine[InputPos])
			{
				case '<':
				case '[':
				case '(':
				case '{':
					NewOpening=1;
					break;
				case '>':
					if ((!StackPtr) || (Stack[StackPtr-1] != '<'))
					{ MismatchFound = 1;   ErrorScore += 25137; }
					break;
				case ']':
					if ((!StackPtr) || (Stack[StackPtr-1] != '['))
					{ MismatchFound = 1;   ErrorScore += 57; }
					break;
				case ')':
					if ((!StackPtr) || (Stack[StackPtr-1] != '('))
					{ MismatchFound = 1;   ErrorScore += 3; }
					break;
				case '}':
					if ((!StackPtr) || (Stack[StackPtr-1] != '{'))
					{ MismatchFound = 1;   ErrorScore += 1197; }
					break;
				default:
					printf("Unrecognized character %c at line %d pos %d\n", InputLine[InputPos], NrOfLines, InputPos);
					break;
			}
			if (NewOpening)
			{
				if (StackPtr<99)
					Stack[StackPtr++] = InputLine[InputPos];
				else
				{
					printf("Cannot extend Stack at line %d pos %d\n", NrOfLines, InputPos);
					break;
				}
			}
			else if (MismatchFound)
			{
				printf("Mismatched %c at line %d pos %d\n", InputLine[InputPos], NrOfLines, InputPos);
				ErrorCount++;
				// No further parsing on this InputLine
				break;
			}
			else
				// Correct closing
				StackPtr--;
		} /* for (InputPos) */
		NrOfLines++;
	} /* while (fgets) */
	printf("%d lines of length %d scanned\n", NrOfLines, InputLen);
	printf("Found %d error lines with total score of %d\n", ErrorCount, ErrorScore);


	return 0;
}		
