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

	///////////////////////////    Find the cheapest cost of every cell     ///////////////////////////
	int X=0,Y=0;
	int DestX, DestY;
	int Cost[100][100];
	memset(Cost, 0, 100 * 100 * sizeof(int));
	// Fake cost for starting cell
	Cost[0][0] = 1;
	int NewX[2000],NewY[2000], NrOfNew,NewNr;
	// Start out with just 1 cell being 'newly discovered'
	NewX[0]=0; NewY[0]=0; NrOfNew=1;

	// ++++++ Forever keep trying to find more New cells, starting from the current series of New cells ++++++
	for (; NrOfNew>0;)
	{
		int ExtraX[2000],ExtraY[2000], NrOfExtra=0;
		for (NewNr=0; NewNr<NrOfNew; NewNr++)
		{
			X=NewX[NewNr]; Y=NewY[NewNr];
			// ------ From the current position, reach out to all adjacent cells ------
			int ChoiceNr;
			for (ChoiceNr=1; ChoiceNr<=4; ChoiceNr++)
			{
				// Determine the Dest coordinates for this Choice
				DestX=X; DestY=Y;
				switch (ChoiceNr)
				{
					case 1: DestY--; break;
					case 2: DestX++; break;
					case 3: DestY++; break;
					case 4: DestX--; break;
				}
				// ------ Consider all possible reasons to reject this ChoiceNr : ------
				// - leaving the grid
				if ((DestX < 0) || (DestX >= InputLen))   continue;
				if ((DestY < 0) || (DestY >= NrOfLines))  continue;
				// - a cheaper route to this cell was found earlier
				if ((Cost[DestX][DestY]) && (Cost[DestX][DestY] <= Cost[X][Y]+Num[DestX][DestY]))
					                                        continue;
				// ------ All conditions have been met : so we found a viable Extra cell ------
				ExtraX[NrOfExtra]=DestX; ExtraY[NrOfExtra]=DestY;
				NrOfExtra++;
				// Register this as the new Cost for the adjacent cell
				//   (whether it is newly discovered, or the Cost is cheaper than before, regardless)
				Cost[DestX][DestY] = Cost[X][Y]+Num[DestX][DestY];
			} /* for (ChoiceNr) */
		} /* for (NewNr) */
		// After having gone through the whole batch of New cells, generatin Extra cells,
		//   we have no further use for the current batch of New cells.
		// The Extra cells now take their place.
		memcpy(NewX, ExtraX, NrOfExtra*sizeof(int));
		memcpy(NewY, ExtraY, NrOfExtra*sizeof(int));
		NrOfNew = NrOfExtra;
	} /* for (;NrOfExtra;) */

	// To know the cost of the bottom-right cell, just look it up -
	//   but don't forget to subtract the fake cost=1 of the top-left cell.
	printf("Found Cost[%d,%d] = %d\n", InputLen-1, NrOfLines-1, Cost[InputLen-1][NrOfLines-1]-1);

	return 0;
}		
