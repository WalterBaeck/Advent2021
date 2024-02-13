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

	///////////////////////////    Consider all paths     ///////////////////////////
	int X=0,Y=0;
	int DestX, DestY, PrevX, PrevY;
	int Score=0, MinScore=0;
	char Step[10000], Visited[100][100];
	memset(Step, 0, 10000);
	memset(Visited, 0, 100 * 100);
	Visited[0][0]=1;
	int NrOfSteps=0, StepNr;
	int Cheapest[100][100];
	memset(Cheapest, 0, 100 * 100 * sizeof(int));

	// ++++++ Forever keep trying to add one more Step to the current Path ++++++
	for (long long Iteration=0;; Iteration++)
	{
		// ------ From the current position, is it possible to pick a further Choice ? ------
		int ChoiceNr = Step[NrOfSteps];
if (!(Iteration&0xFFFFFF))
printf("Iteration %lld starts Step[%d] from pos[%d,%d] beyond Choice %d - MinScore %d\n", Iteration, NrOfSteps, X, Y, ChoiceNr, MinScore);
		for (ChoiceNr++; ChoiceNr<=4; ChoiceNr++)
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
			// ------ Consider all possible reasons to reject this Choice : ------
			// - leaving the grid
			if ((DestX < 0) || (DestX >= InputLen))   continue;
			if ((DestY < 0) || (DestY >= NrOfLines))  continue;
			// - revisiting a cell
			if (Visited[DestX][DestY])                continue;
			// - a cheaper route to this cell was found earlier
			if ((Cheapest[DestX][DestY]) && (Cheapest[DestX][DestY] < Score+Num[DestX][DestY]))
				                                        continue;

			// ------ All obstacles have been passed : so we found a new viable Step ------
			Step[NrOfSteps++] = ChoiceNr;
			X = DestX;  Y = DestY;
			Visited[X][Y] = 1;
			Score += Num[X][Y];
			Cheapest[X][Y] = Score;
			// If we have arrived at the End, register this, and extend no further
			if ((DestX == InputLen-1) && (DestY == NrOfLines-1))
			{
				if ((!MinScore) || (Score < MinScore))  MinScore = Score;
				// Ensure backtracking instead of extending the Path
				Step[NrOfSteps] = 4;
			}
			else if (NrOfSteps > 9998)
			{
				fprintf(stderr, "No room for more than 10000 steps\n");
				exit(3);
			}
			break;
		} /* for (ChoiceNr) */
		if (ChoiceNr > 4)
		{
			// At the current Node, we have exhausted all Choices to make another Step.
			// Instead, we need to undo the last taken Step, and reconsider from there.
			if (!NrOfSteps)
				// Cannot backtrack any further : the search ends
				break;
			// ------ Backtrack : break up the last taken Step ------
			Step[NrOfSteps] = 0;
			Visited[X][Y] = 0;
			Score -= Num[X][Y];
			NrOfSteps--;
			ChoiceNr = Step[NrOfSteps];
			switch (ChoiceNr)
			{
				case 1: Y++; break;
				case 2: X--; break;
				case 3: Y--; break;
				case 4: X++; break;
			}
		}
	} /* for (;;) */

	printf("Found MinScore %d\n", MinScore);


	return 0;
}		
