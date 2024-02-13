#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[100];
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

	// Naive implementation records everything that happens within a 101 x 101 x 101 cube
	int X,Y,Z,SizeX=101, SizeY=101, SizeZ=101;
	unsigned char* CubeAlloc = (unsigned char*)malloc(SizeX * SizeY * SizeZ * sizeof(unsigned char));
	memset(CubeAlloc, 0, SizeX * SizeY * SizeZ * sizeof(unsigned char));
	// Array[X][Y][Z] referencing, requires a 2-D array of pointers to strips of <SizeZ> values
	unsigned char** CubeStrip = (unsigned char**)malloc(SizeX * SizeY * sizeof(unsigned char*));
	CubeStrip[0] = CubeAlloc;
	for (int i=1; i<SizeX*SizeY; i++)  CubeStrip[i] = CubeStrip[i-1] + SizeZ;
	// These strips are then pointed to by <SizeX> pointers, each pointing to <SizeY> CubeStrips
	unsigned char*** Cube = (unsigned char***)malloc(SizeX * sizeof(unsigned char**));
	Cube[0] = CubeStrip;
	for (int i=1; i<SizeX; i++)  Cube[i] = Cube[i-1] + SizeY;

	///////////////////////////    Get the starting positions     ///////////////////////////
	int NrOfLines=0, InputFieldNr;
	while (fgets(InputLine, 100, InputFile))
	{
		NrOfLines++;
		// First parse the on/off string
		unsigned char OnNotOff=0;
		if (!strncmp(InputLine, "on ", 3))
		{
			OnNotOff = 1;
			InputPos = 3;
		}
		else if (!strncmp(InputLine, "off ", 4))
		{
			OnNotOff = 0;
			InputPos = 4;
		}
		else
		{
			fprintf(stderr, "Expecting on/off at beginning of InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}
		// Then parse the 6 numerical fields
		InputFieldNr = 0;
		int Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;
		while (InputPtr = strtok(InputFieldNr ? NULL : InputLine + InputPos, "=.,\n"))
		{
			InputFieldNr++;
			// Don't parse the x-y-z indicators
			if ((InputFieldNr == 1) || (InputFieldNr == 4) || (InputFieldNr == 7))  continue;
			int Value;
			if (1 != sscanf(InputPtr, "%d", &Value))
			{
				fprintf(stderr, "Cannot parse numeric field #%d : %s on InputLine #%d\n", InputFieldNr, InputPtr, NrOfLines);
				exit(3);
			}
			switch (InputFieldNr)
			{
				case 2: Xmin = Value;  break;
				case 3: Xmax = Value;  break;
				case 5: Ymin = Value;  break;
				case 6: Ymax = Value;  break;
				case 8: Zmin = Value;  break;
				case 9: Zmax = Value;  break;
			}
		} /* while (strtok) */
		// Now apply this action to the Cube, but impose the size constraints first
		if (Xmin < -50)  Xmin = -50;
		if (Xmax > 50)   Xmax =  50;
		if (Ymin < -50)  Ymin = -50;
		if (Ymax > 50)   Ymax =  50;
		if (Zmin < -50)  Zmin = -50;
		if (Zmax > 50)   Zmax =  50;
printf("Putting [%d..%d][%d..%d][%d..%d] to %hhd\n", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, OnNotOff);
		for (X=Xmin; X<=Xmax; X++)
		for (Y=Ymin; Y<=Ymax; Y++)
		for (Z=Zmin; Z<=Zmax; Z++)
			// Reposition to a SizeX x SizeY x SizeZ cube starting from [0,0,0]
			Cube[X+50][Y+50][Z+50] = OnNotOff;
	} /* while (fgets) */
	printf("Parsed %d Lines\n", NrOfLines);

	///////////////////////////    Counting Cubicles     ///////////////////////////
	int NumCubicles = 0;
	for (X=0; X<SizeX; X++)
	for (Y=0; Y<SizeY; Y++)
	for (Z=0; Z<SizeZ; Z++)
		if (Cube[X][Y][Z])  NumCubicles++;

	printf("%d cubicles are on\n", NumCubicles);
	return 0;
}		
