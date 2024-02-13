#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[200];
	char* InputPtr;
	int InputPos, InputLen=0;

	int IterationNr,NrOfIterations=0;

	if (Arguments > 3)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s [InputFile [Iterations]]\n", Argument[0]);
		fprintf(stderr, "\twith no [InputFile] argument, data will be read from stdin\n");
		fprintf(stderr, "\tUp to [Iterations] will be run, or until the situation remains unchanged\n");
		exit(1);
	}
	if (Arguments >= 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n", Argument[1]);
			exit(2);
		}
	}
	if (Arguments >= 3)
	{
		if (1 != sscanf(Argument[2], "%d", &NrOfIterations))
		{
			fprintf(stderr, "Could not scan NrOfIterations from %s for reading\n", Argument[2]);
			exit(2);
		}
	}

	typedef enum {eEmpty, eEast, eSouth} tDir;
	tDir Cell[200][200]={eEmpty}, Prev[200][200]={eEmpty}, Move[200][200]={eEmpty};

	///////////////////////////    Get the Lookup table     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 200, InputFile))
	{
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
		for (InputPos=0; InputPos<InputLen; InputPos++)
			if (InputLine[InputPos] == 'v')
				Cell[InputPos][NrOfLines] = eSouth;
			else if (InputLine[InputPos] == '>')
				Cell[InputPos][NrOfLines] = eEast;
			else if (InputLine[InputPos] == '.')
				Cell[InputPos][NrOfLines] = eEmpty;
			else
			{
				fprintf(stderr, "Unexpected character %c for Cell [%d,%d]\n%s",
						InputLine[InputPos], InputPos, NrOfLines, InputLine);
				exit(3);
			}
		NrOfLines++;
	} /* while (fgets) */
	printf("Found input datasize to be %d Lines x %d chars\n", NrOfLines, InputLen);

	for (IterationNr=0; (!NrOfIterations) || (IterationNr<NrOfIterations); IterationNr++)
	{
#if 0
		// Display situation
		printf("==============  Iteration %2d  ==============\n", IterationNr);
		for (int Y=0; Y<NrOfLines; Y++)
		{
			for (int X=0; X<InputLen; X++)
				switch (Cell[X][Y])
				{
					case eEmpty: putchar('.');  break;
					case eSouth: putchar('v');  break;
					case eEast:  putchar('>');  break;
				}
			putchar('\n');
		}
#endif

		// Record the existing grid
		memcpy(Prev, Cell, 200 * 200 * sizeof(tDir));

		tDir theDir;
		for (theDir=eEast; theDir<=eSouth; theDir++)
		{
			for (int X=0; X<InputLen; X++)
				for (int Y=0; Y<NrOfLines; Y++)
				{
					Move[X][Y] = eEmpty;
					// Does the inhabitant of this Cell have the Direction being considered ?
					if (Cell[X][Y] != theDir)                         continue;
					// For this Direction, where is the Target cell ?
					int TargetX=X, TargetY=Y;
					switch (theDir)
					{
						case eEast:  if (++TargetX>=InputLen)  TargetX=0;  break;
						case eSouth: if (++TargetY>=NrOfLines) TargetY=0;  break;
					}
					// Is the Target cell clear ?
					if (Cell[TargetX][TargetY] != eEmpty)             continue;
					Move[X][Y] = theDir;
				} /* for (Y) */

			for (int X=0; X<InputLen; X++)
				for (int Y=0; Y<NrOfLines; Y++)
					if (Move[X][Y] == theDir)
					{
						// For this Direction, where is the Target cell ?
						int TargetX=X, TargetY=Y;
						switch (theDir)
						{
							case eEast:  if (++TargetX>=InputLen)  TargetX=0;  break;
							case eSouth: if (++TargetY>=NrOfLines) TargetY=0;  break;
						}
						// Make the move effective
						Cell[TargetX][TargetY] = theDir;
						Cell[X][Y] = eEmpty;
					} /* if (Movement == theDir) */
		} /* for (theDir) */

		// Did anything change ?
		if (!memcmp(Prev, Cell, 200 * 200 * sizeof(tDir)))         break;

	} /* for (IterationNr) */
	printf("IterationNr %d\n", IterationNr);

	return 0;
}		
