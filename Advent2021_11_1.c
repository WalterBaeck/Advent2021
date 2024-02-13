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

	int InputPos, InputLen=0;
	int NrOfLines=0;

	int Num[10][10];
	while (fgets(InputLine, 100, InputFile))
	{
	///////////////////////////    Get one full line of digits     ///////////////////////////
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!InputLen)
			InputLen = strlen(InputLine);
		else if (InputLen != strlen(InputLine))
		{
			fprintf(stderr, "InputLen was determined at %d but InputLine #%d has len %d\n",
					InputLen, NrOfLines, strlen(InputLine));
			exit(3);
		}
		for (InputPos=0; InputPos<InputLen; InputPos++)
		{
			if ((InputLine[InputPos] < '0') || (InputLine[InputPos] > '9'))
			{
				fprintf(stderr, "Cannot interpret line %d character %d: %c as a digit\n",
						NrOfLines+1, InputPos, InputLine[InputPos]);
				exit(2);
			}
			if (NrOfLines >= 10)
			{
				fprintf(stderr, "No storage for more than 10 InputLines\n");
				exit(3);
			}
			if (InputPos >= 10)
			{
				fprintf(stderr, "No storage for more than 10 chars on an InputLine\n");
				exit(3);
			}
			Num[NrOfLines][InputPos] = InputLine[InputPos] - '0';
		}
		NrOfLines++;
	} /* while (fgets) */
	printf("%d lines of length %d scanned\n", NrOfLines, InputLen);

	///////////////////////////    Simulate : reaching a Flash    ///////////////////////////
	int Iteration;
	int NrOfFlashes=0;
	for (Iteration=0; Iteration<100; Iteration++)
	{
		unsigned char HasFlashedThisIteration[10][10];
		memset(HasFlashedThisIteration, 0, 10 * 10);
		int X, Y;
		// First step of an Iteration is to increment every cell
		for (X=0; X<NrOfLines; X++)
			for (Y=0; Y<InputLen; Y++)
				Num[X][Y]++;

#if 0
		printf("+++++++++Increment+++++++++\n");
		for (X=0; X<NrOfLines; X++)
		{
			for (Y=0; Y<InputLen; Y++)
				putchar('0' + Num[X][Y]);
			putchar('\n');
		}
#endif

		// Within each Iteration, expanding waves of flashes need to be computed
		//   until the latest wave ignites no further subsequent flashes
		for (;;)
		{
			int FlashesThisPass=0;
			int X, Y;
			for (X=0; X<NrOfLines; X++)
				for (Y=0; Y<InputLen; Y++)
					if ((Num[X][Y]>9) && (!HasFlashedThisIteration[X][Y]))
					{
						// OK it's a new Flash
						HasFlashedThisIteration[X][Y] = 1;
						FlashesThisPass++;
#if 0
						printf("Because [%d][%d]=%d flashes during this Pass ..\n", X, Y, Num[X][Y]);
#endif
						// Make Waves
						int AdjX, AdjY;
						for (AdjX=X-1; AdjX<=X+1; AdjX++)
						{
							if ((AdjX < 0) || (AdjX >= NrOfLines))  continue;
							for (AdjY=Y-1; AdjY<=Y+1; AdjY++)
							{
								if ((AdjY < 0) || (AdjY >= InputLen))  continue;
								if ((AdjX == X) && (AdjY == Y))        continue;
								Num[AdjX][AdjY]++;
#if 0
						printf(".. increase [%d][%d] to %d\n", AdjX, AdjY, Num[AdjX][AdjY]);
#endif
							}
						} /* for (AdjX) */
#if 0
		for (X=0; X<NrOfLines; X++)
		{
			for (Y=0; Y<InputLen; Y++)
				putchar(HasFlashedThisIteration[X][Y] ? '0' : '.');
			putchar('\n');
		}
#endif
					} /* if (FlashReached) */
			if (!FlashesThisPass)  break;
			NrOfFlashes += FlashesThisPass;

#if 0
		printf("---------------Wave------------\n");
		for (X=0; X<NrOfLines; X++)
		{
			for (Y=0; Y<InputLen; Y++)
				putchar('0' + Num[X][Y]);
			putchar('\n');
		}
#endif

		} /* for (;;) */
		// Last step of an Iteration is to reset all cells that have flashed
		for (X=0; X<NrOfLines; X++)
			for (Y=0; Y<InputLen; Y++)
				if (HasFlashedThisIteration[X][Y])  Num[X][Y] = 0;

		printf("============= After Iteration %2d =============\n", Iteration+1);
		for (X=0; X<NrOfLines; X++)
		{
			for (Y=0; Y<InputLen; Y++)
				putchar('0' + Num[X][Y]);
			putchar('\n');
		}

	} /* for (Iteration) */

	printf("%d Flashes found after 100 Iterations\n", NrOfFlashes);

	return 0;
}		
