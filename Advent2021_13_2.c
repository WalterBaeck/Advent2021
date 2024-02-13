#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Variations.h"

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[100];
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

	int X[1000],Y[1000];
	int NrOfDots=0, DotNr;

	///////////////////////////    Input the dots     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 100, InputFile))
	{
		NrOfLines++;
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!strlen(InputLine))  break;

		int NrOfStrings=0;
		int InputNumber, InputX=-1, InputY=-1;
		while (InputPtr = strtok(NrOfStrings?NULL:InputLine, ","))
		{
			if (NrOfStrings >= 2)
			{
				fprintf(stderr, "Expect no more than 2 strings on InputLine #%d\n%s", NrOfLines, InputLine);
				exit(3);
			}
			if (1 != sscanf(InputPtr, "%d", &InputNumber))
			{
				fprintf(stderr, "Cannot interpret numeric InputString #%d on InputLine #%d\n%s",
						NrOfStrings, NrOfLines, InputLine);
				exit(3);
			}
			// Remember the InputNumber that was just scanned in
			if (!NrOfStrings)
				InputX = InputNumber;
			else if (NrOfStrings == 1)
				InputY = InputNumber;
			NrOfStrings++;
		} /* while (strtok) */
		if (NrOfStrings != 2)
		{
			fprintf(stderr, "Supposed to scan exactly 2 Strings on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}
		X[NrOfDots] = InputX;
		Y[NrOfDots] = InputY;
		NrOfDots++;
	} /* while (fgets) */
	printf("%d lines scanned - NrOfDots is %d\n", NrOfLines, NrOfDots);

	///////////////////////////    Folding instructions     ///////////////////////////
	while (fgets(InputLine, 100, InputFile))
	{
		NrOfLines++;
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (strncmp(InputLine, "fold along ", 11))
		{
			fprintf(stderr, "Expect a folding instruction on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}

		int Horizontal=0, FoldCoord=-1;
		if (InputLine[11]=='y')
			Horizontal = 1;
		else if (InputLine[11]!='x')
		{
			fprintf(stderr, "Expect a folding instruction on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}

		if (1 != sscanf(InputLine+13, "%d", &FoldCoord))
		{
			fprintf(stderr, "No FoldCoord found on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}

printf("-------- Folding over %s -----------\n", InputLine+11);

		// Now go over all known Dots, to fold them into another position if needed
		for (DotNr=0; DotNr<NrOfDots; DotNr++)
		{
			int DestX,DestY,DestNr;
			DestX = X[DotNr];
			DestY = Y[DotNr];
			if (Horizontal)
			{
				if (Y[DotNr] < FoldCoord)
					// Nothing happens to this dot, and the NrOfDots remains unchanged
					continue;
				else
					DestY = FoldCoord - (Y[DotNr] - FoldCoord);
			}
			else // Vertical folding
			{
				if (X[DotNr] < FoldCoord)
					// Nothing happens
					continue;
				else
					DestX = FoldCoord - (X[DotNr] - FoldCoord);
			}
			// The question is, whether the Destination was already occupied by a Dot or not
			for (DestNr=0; DestNr<NrOfDots; DestNr++)
				if ((DestX==X[DestNr]) && (DestY==Y[DestNr]))
					break;
			if (DestNr >= NrOfDots)
			{
				// Dest was not known before as a Dot
				// This mean that the NrOfDots remains the same;
				// we just change the original Dot's coordinates into Dest's coordinates
printf("[%d,%d] changes to the empty location [%d,%d]\n", X[DotNr], Y[DotNr], DestX, DestY);
				X[DotNr] = DestX;
				Y[DotNr] = DestY;
				continue;
			}
			// Arriving here, means that the Dest was already occupied by a Dot.
			// We merely need to remove the original Dot that was folded into Dest;
			// it disappears from the set of Dots.
printf("[%d,%d] folds onto existing Dot #%d [%d,%d]\n", X[DotNr], Y[DotNr], DestNr, DestX, DestY);
			for (DestNr=DotNr; DestNr<NrOfDots-1; DestNr++)
			{
				X[DestNr] = X[DestNr+1];
				Y[DestNr] = Y[DestNr+1];
			}
			NrOfDots--;
			// This means that we have to consider DotNr yet again - we have erased what was there
			DotNr--;
		} /* for (DotNr) */

	printf("+++++++ NrOfDots is now %d +++++++\n", NrOfDots);

	} /* while (fgets) */
	printf("%d lines scanned - NrOfDots is %d\n", NrOfLines, NrOfDots);

	///////////////////////////    Visualizing     ///////////////////////////
	int MaxX=0, MaxY=0;
	char Grid[100][20];
	memset(Grid, '.', 100 * 20);
	for (DotNr=0; DotNr<NrOfDots; DotNr++)
	{
		if (X[DotNr] > MaxX)  MaxX = X[DotNr];
		if (Y[DotNr] > MaxY)  MaxY = Y[DotNr];
		if ((X[DotNr] >= 100) || (Y[DotNr] >= 20))
		{
			fprintf(stderr, "Could not visualize Dot #%d [%d,%d]\n", DotNr, X[DotNr], Y[DotNr]);
			exit(4);
		}
		Grid[X[DotNr]][Y[DotNr]] = '#';
	}
	int GridX, GridY;
	for (GridY=0; GridY<=MaxY; GridY++)
	{
		for (GridX=0; GridX<=MaxX; GridX++)
			putchar(Grid[GridX][GridY]);
		putchar('\n');
	}

	return 0;
}		
