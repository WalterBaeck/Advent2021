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
	int CompletionCount=0;
	long long SortedScore[100], CompletionScore;
	int NrOfScores=0, ScoreNr;

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
					if ((!StackPtr) || (Stack[StackPtr-1] != '<'))  MismatchFound = 1;
					break;
				case ']':
					if ((!StackPtr) || (Stack[StackPtr-1] != '['))  MismatchFound = 1;
					break;
				case ')':
					if ((!StackPtr) || (Stack[StackPtr-1] != '('))  MismatchFound = 1;
					break;
				case '}':
					if ((!StackPtr) || (Stack[StackPtr-1] != '{'))  MismatchFound = 1;
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
				// No further parsing on this InputLine
				break;
			else
				// Correct closing
				StackPtr--;
		} /* for (InputPos) */

		// Was the line parsed until the end ? That means, no corruption
		// If additionally, the StackPtr hasn't reached 0 again, then we need completion
		if ((InputPos >= InputLen) && (StackPtr))
		{
	///////////////////////////    Compute Completion     ///////////////////////////
			CompletionCount++;
			CompletionScore=0;
			while (--StackPtr >= 0)
			{
				CompletionScore *= 5;
				switch (Stack[StackPtr])
				{
					case '<':  CompletionScore+=4;  break;
					case '{':  CompletionScore+=3;  break;
					case '[':  CompletionScore+=2;  break;
					case '(':  CompletionScore+=1;  break;
				}
			}
			printf("Completion score %lld for InputLine %d\n", CompletionScore, NrOfLines+1);
	/////////////////////    Insert Score into Sorted array     /////////////////////
			for (ScoreNr=0; ScoreNr<NrOfScores; ScoreNr++)
				if (CompletionScore <= SortedScore[ScoreNr])  break;
			int ScorePos;
			for (ScorePos=NrOfScores; ScorePos>ScoreNr; ScorePos--)
				SortedScore[ScorePos] = SortedScore[ScorePos-1];
			SortedScore[ScoreNr] = CompletionScore;
			NrOfScores++;
		} /* if (Completion) */
		NrOfLines++;
	} /* while (fgets) */
	printf("%d lines of length %d scanned\n", NrOfLines, InputLen);
	printf("Found %d incomplete lines with median score of %lld\n", CompletionCount, SortedScore[NrOfScores>>1]);

	for (ScoreNr=0; ScoreNr<NrOfScores; ScoreNr++)
		printf("%lld  ", SortedScore[ScoreNr]);
	printf("\n");

	return 0;
}		
