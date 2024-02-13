#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Variations.h"

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[105];
	char* InputPtr;

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

	int InputPos, InputLen=0;
	int NrOfLines=0, LineNr;

	int Num[100][100];
	while (fgets(InputLine, 105, InputFile))
	{
	///////////////////////////    Get one full line of digits     ///////////////////////////
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!InputLen)
			InputLen = strlen(InputLine);
		for (InputPos=0; InputPos<InputLen; InputPos++)
		{
			if ((InputLine[InputPos] < '0') || (InputLine[InputPos] > '9'))
			{
				fprintf(stderr, "Cannot interpret line %d character %d: %c as a digit\n",
						NrOfLines+1, InputPos, InputLine[InputPos]);
				exit(2);
			}
			if (NrOfLines >= 100)
			{
				fprintf(stderr, "No storage for more than 100 InputLines\n");
				exit(3);
			}
			if (InputPos >= 100)
			{
				fprintf(stderr, "No storage for more than 100 chars on an InputLine\n");
				exit(3);
			}
			Num[NrOfLines][InputPos] = InputLine[InputPos] - '0';
		}
		NrOfLines++;
	} /* while (fgets) */
	printf("%d lines of length %d scanned\n", NrOfLines, InputLen);

	///////////////////////////    Look for the local lows     ///////////////////////////
	int NrOfLocalLows=0, SumOfLocalLows=0;
	for (LineNr=0; LineNr<NrOfLines; LineNr++)
		for (InputPos=0; InputPos<InputLen; InputPos++)
		{
			int AdjacentLowerFound=0;
			// Try to look for an adjacent lower number.
			// This can be done in 4 directions, not diagonal, and not outside the grid of Nums.
			if (InputPos)
			{ if (Num[LineNr][InputPos-1] <= Num[LineNr][InputPos])  AdjacentLowerFound = 1;	}
			if (InputPos+1 < InputLen)
			{ if (Num[LineNr][InputPos+1] <= Num[LineNr][InputPos])  AdjacentLowerFound = 1;	}
			if (LineNr)
			{ if (Num[LineNr-1][InputPos] <= Num[LineNr][InputPos])  AdjacentLowerFound = 1;	}
			if (LineNr+1 < NrOfLines)
			{ if (Num[LineNr+1][InputPos] <= Num[LineNr][InputPos])  AdjacentLowerFound = 1;	}
			// When no adjacent lower numbers can be found, it's a LocalLow
			if (!AdjacentLowerFound)
			{
				NrOfLocalLows++;
				SumOfLocalLows += Num[LineNr][InputPos];
printf("#%2d: LocalLow[%2d][%2d] = %d\n", NrOfLocalLows, LineNr, InputPos, Num[LineNr][InputPos]);
			}
		}

	printf("%d LocalLows found, with a sum of %d - together: %d\n",
			NrOfLocalLows, SumOfLocalLows, NrOfLocalLows + SumOfLocalLows);

	return 0;
}		
