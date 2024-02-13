#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	int MaxIndex, BasinMax[3]={0};

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
	int NrOfLocalLows=0;
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
printf("#%2d: LocalLow[%2d][%2d] = %d", NrOfLocalLows, LineNr, InputPos, Num[LineNr][InputPos]);

	//////////////    Look for the Basin around this LocalLow     ////////////////
				unsigned char InBasin[100][100];
				memset(InBasin, 0, 100 * 100 * sizeof(unsigned char));
				// Begin by marking the LocalLow itself as the first Basin dweller
				int BasinSize=1;
				InBasin[LineNr][InputPos] = 1;
				// Now keep scanning the whole grid, looking to extend the Basin
				int BasinExtended;
				do {
					BasinExtended = 0;
					int GridLine, GridPos;
					int AdjacentLine, AdjacentPos;
					for (GridLine=0; GridLine<NrOfLines; GridLine++)
						for (GridPos=0; GridPos<InputLen; GridPos++)
							// For any Basin element, look for a non-Basin adjacent higher number, but exclude 9
							if (InBasin[GridLine][GridPos])
							{
								int Direction;
								for (Direction=0; Direction<4; Direction++)
								{
									AdjacentLine = GridLine;  AdjacentPos = GridPos;
									switch (Direction)
									{
										case 0: AdjacentPos--; break;
										case 1: AdjacentPos++; break;
										case 2: AdjacentLine--; break;
										case 3: AdjacentLine++; break;
									}
									// Viable Adjacent coordinates ?
									if (AdjacentPos < 0)  continue;
									if (AdjacentPos >= InputLen)  continue;
									if (AdjacentLine < 0)  continue;
									if (AdjacentLine >= NrOfLines)  continue;
									// Adjacent should not be member of Basin yet
									if (InBasin[AdjacentLine][AdjacentPos])  continue;
									// Adjacent should be strictly higher
									if (Num[AdjacentLine][AdjacentPos] <= Num[GridLine][GridPos])  continue;
									// Adjacent must not have level 9
									if (Num[AdjacentLine][AdjacentPos] == 9)  continue;
									// When all the conditions have been met, extend the Basin with this Adjacent
									InBasin[AdjacentLine][AdjacentPos] = 1;
									BasinExtended = 1;
									BasinSize++;
								} /* for (Direction) */
							} /* if (InBasin) */
				} while (BasinExtended);

				printf("  with Basin size %d\n", BasinSize);
				// Can this BasinSize compete with the 3 Maxima registered so far ?
				for (MaxIndex=0; MaxIndex<3; MaxIndex++)
					if (BasinSize > BasinMax[MaxIndex])
					{
						int temp = BasinMax[MaxIndex];
						BasinMax[MaxIndex] = BasinSize;
						BasinSize = temp;
					}
			} /* if (!AdjacentLowerFound) */
		} /* for (InputPos) */

	int MaxMult=1;
	printf("%d LocalLows found, with maximum basin sizes", NrOfLocalLows);
	for (MaxIndex=0; MaxIndex<3; MaxIndex++)
	{
		printf("%s%d", MaxIndex?" * ":" ", BasinMax[MaxIndex]);
		MaxMult *= BasinMax[MaxIndex];
	}
	printf(" = %d\n", MaxMult);

	return 0;
}		
