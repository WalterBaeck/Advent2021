#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;

	if (Arguments == 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n");
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

	typedef enum {
		eUp,
		eDown,
		eForward,
	} tMove;
	char InputLine[100];
	tMove InputMove;
	int InputPos, InputDist;
	int NrOfMoves=0, HorPos=0, VerPos=0;


	while (fgets(InputLine, 100, InputFile))
	{
		///////////////////////////    Get input move    ///////////////////////////
		if (feof(InputFile))  break;
		if (!strncmp(InputLine, "up ", 3))
		{  InputMove = eUp;       InputPos = 3; }
		else if (!strncmp(InputLine, "down ", 5))
		{  InputMove = eDown;     InputPos = 5; }
		else if (!strncmp(InputLine, "forward ", 8))
		{  InputMove = eForward;  InputPos = 8; }
		else
		{
			fprintf(stderr, "Cannot interpret Move type from line %d: %s", NrOfMoves, InputLine);
			exit(4);
		}
		if (1 != sscanf(InputLine+InputPos, "%d", &InputDist))
		{
			fprintf(stderr, "Could not scan Move Distance from %s\n", InputLine);
			exit(3);
		}

		/////////////////////////////   Bookkeeping    /////////////////////////////
		NrOfMoves++;

		////////////////////////   Processing this move   ////////////////////////
		switch (InputMove)
		{
			case eUp:        VerPos -= InputDist;  break;
			case eDown:      VerPos += InputDist;  break;
			case eForward:   HorPos += InputDist;  break;
		}

		///////////////////////////   Prepare for next   ///////////////////////////
	}

	printf("%d moves scanned - HorPos=%d  VerPos=%d\n", NrOfMoves, HorPos, VerPos);
	printf("Multiplication result: %d\n", HorPos * abs(VerPos));
	return 0;
}		
