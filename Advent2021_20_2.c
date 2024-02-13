#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[1000];
	char* InputPtr;
	int InputPos, InputLen=0;

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

	unsigned char Lookup[512]={0};
	int LookupNr=0, LinesToSkip;
	int IterationNr,NrOfIterations=50;

	///////////////////////////    Get the Lookup table     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 1000, InputFile))
	{
		NrOfLines++;
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		for (InputPos=0; InputPos<strlen(InputLine); InputPos++, LookupNr++)
			if (InputLine[InputPos] == '.')
				Lookup[LookupNr] = 0;
			else if (InputLine[InputPos] == '#')
				Lookup[LookupNr] = 1;
			else
			{
				fprintf(stderr, "Unexpected character %c for LookupNr %d on InputLine #%d\n%s",
						InputLine[InputPos], LookupNr, NrOfLines, InputLine);
				exit(3);
			}
		if (!strlen(InputLine))
		{
			if (LookupNr != 512)
			{
				fprintf(stderr, "Expecting 512 lookup chars on InputLine #%d\n%s", NrOfLines, InputLine);
				exit(3);
			}
			LinesToSkip = NrOfLines;
			break;
		}
	} /* while (fgets) */
	printf("Parsed %d Lines of Lookup\n", NrOfLines);

	///////////////////////////    Determine data size     ///////////////////////////
	NrOfLines = 0;
	while (fgets(InputLine, 1000, InputFile))
	{
		NrOfLines++;
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!InputLen)
			InputLen = strlen(InputLine);
		else if (InputLen != strlen(InputLine))
		{
			fprintf(stderr, "InputLen was established as %d but InputLine #%d now has length %d\n%s",
					InputLen, strlen(InputLine), InputLine);
			exit(3);
		}
	}
	printf("Found input datasize to be %d Lines x %d chars\n", NrOfLines, InputLen);

	// Every Iteration can extend the field where '1' cells occur,
	//   by a rim of thickness 1 all around the current grid.
	// Therefore the total span of the field can increase by 2 per Iteration,
	//   both in horizontal and vertical direction.
	//
	// Moreover, it's easier to have an additional dummy outer rim of '0' cells,
	//   to facilitate construction of a Lookup index everywhere.
	int X,Y;
	int SizeX = InputLen + 2*(NrOfIterations + 1);
	int SizeY = NrOfLines + 2*(NrOfIterations + 1);

	// Array[X][Y] referencing, requires <InputLen> pointers to strips of <NrOfLines> values
	unsigned char* OldAlloc = (unsigned char*)malloc(SizeY * SizeX);
	unsigned char** Old = (unsigned char**)malloc(SizeX * sizeof(unsigned char*));
	Old[0] = OldAlloc;
	for (int i=1; i<SizeX; i++)  Old[i] = Old[i-1] + SizeY;
	memset(OldAlloc, 0, SizeX * SizeY);

	unsigned char* NewAlloc = (unsigned char*)malloc(SizeY * SizeX);
	unsigned char** New = (unsigned char**)malloc(SizeX * sizeof(unsigned char*));
	New[0] = NewAlloc;
	for (int i=1; i<SizeX; i++)  New[i] = New[i-1] + SizeY;

	///////////////////////////    Get the data     ///////////////////////////
	rewind(InputFile);
	// Skip the Lookup table
	for (NrOfLines=0; NrOfLines<LinesToSkip; NrOfLines++)
		fgets(InputLine, 1000, InputFile);
	NrOfLines=0;
	while (fgets(InputLine, 1000, InputFile))
	{
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		for (InputPos=0; InputPos<strlen(InputLine); InputPos++)
			// Make sure to center the initial data within the extended Grid
			if (InputLine[InputPos] == '.')
				Old[InputPos+NrOfIterations+1][NrOfLines+NrOfIterations+1] = 0;
			else if (InputLine[InputPos] == '#')
				Old[InputPos+NrOfIterations+1][NrOfLines+NrOfIterations+1] = 1;
			else
			{
				fprintf(stderr, "Unexpected character %c for Data [%d,%d]\n%s",
						InputLine[InputPos], InputPos, NrOfLines, InputLine);
				exit(3);
			}
		NrOfLines++;
	} /* while (fgets) */
	printf("Parsed %d Lines of length %d with Data\n", NrOfLines, InputLen);

	///////////////////////////    Iterate     ///////////////////////////
#if 0
		// Debug printout
		printf("----- Starting setup -----\n");
		for (Y=0; Y<SizeY; Y++)
		{
			for (X=0; X<SizeX; X++)
				putchar(Old[X][Y]?'#':'.');
			putchar('\n');
		}
#endif
	for (IterationNr=0; IterationNr<NrOfIterations; IterationNr++)
	{
		// The outer rim behaves like the infinite grid stretching out beyond the known data.
		// Its representative cell[0][0] will never be affected by the Lookup operation.
		// We need to consider the Lookup rule for the contents of cell[0][0] to decide
		// how the infinite surrounding grid will be filled after this Iteration.
		unsigned char OldDefaultChar = Old[0][0];
		unsigned char NewDefaultChar = Lookup[OldDefaultChar ? 511 : 0];
		memset(NewAlloc, NewDefaultChar, SizeX * SizeY);
		// Skip the outer rim as there are no surrounding cells available there, to construct Lookup
		for (X=1; X<SizeX-1; X++)
			for (Y=1; Y<SizeY-1; Y++)
			{
				// Construct a Lookup index
				LookupNr = 0;
				for (int LookupY = Y-1; LookupY<=Y+1; LookupY++)
					for (int LookupX = X-1; LookupX<=X+1; LookupX++)
					{
						LookupNr <<= 1;
						LookupNr += Old[LookupX][LookupY];
					}
				// Perform the Lookup and decide the New cell
				New[X][Y] = Lookup[LookupNr];
			} /* for (Y) */
		// After all the work is done, copy New back into the Old grid
		memcpy(OldAlloc, NewAlloc, SizeX * SizeY);
#if 0
		// Debug printout
		printf("----- Iteration %d -----\n", IterationNr+1);
		for (Y=0; Y<SizeY; Y++)
		{
			for (X=0; X<SizeX; X++)
				putchar(Old[X][Y]?'#':'.');
			putchar('\n');
		}
#endif
	} /* for (IterationNr) */

	//////////////////////    Count the '1' cells in the result     ///////////////////////
	int NrOfOnes=0;
	for (X=0; X<SizeX; X++)
		for (Y=0; Y<SizeY; Y++)
			if (Old[X][Y])  NrOfOnes++;
	printf("NrOfOnes %d\n", NrOfOnes);

	return 0;
}		
