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

	int Score[2]={0};
	int Position[2];
	int PlayerNr, Turn=0;
	int Dice=0;

	///////////////////////////    Get the starting positions     ///////////////////////////
	int NrOfLines=0;
	for (PlayerNr=0; PlayerNr<2; PlayerNr++)
	{
		fgets(InputLine, 100, InputFile);
		NrOfLines++;
		char Expected[100]="Player 1 starting position: ";
		Expected[7] += PlayerNr;
		if (strncmp(InputLine, Expected, strlen(Expected)))
		{
			fprintf(stderr, "Unexpected InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}
		if (1 != sscanf(InputLine + strlen(Expected), "%d", &(Position[PlayerNr])))
		{
			fprintf(stderr, "Could not read startpos at pos %d of InputLine #%d\n%s",
					strlen(Expected), NrOfLines, InputLine);
			exit(3);
		}
	} /* for (PlayerNr) */
	printf("Parsed %d Lines\n", NrOfLines);

	///////////////////////////    Play the game     ///////////////////////////
	PlayerNr=0;
	for (;;)
	{
		Turn++;
		int Advance = 0;
		for (int Throw=0; Throw<3; Throw++)
		{
			if (++Dice > 100)  Dice = 1;
			Advance += Dice;
		} /* for (Throw) */
		// Now combine the total distance to advance from these 3 Throws
		// Easier modulo calculus by subtracting 1
		Position[PlayerNr] = ((Position[PlayerNr]-1 + Advance) % 10) + 1;
		Score[PlayerNr] += Position[PlayerNr];
		if (Score[PlayerNr] >= 1000)  break;
		// This Turn is completed
		if (++PlayerNr >= 2)  PlayerNr = 0;
	}

	printf("At Turn %d, Player %d reached score %d\n", Turn, PlayerNr+1, Score[PlayerNr]);
	printf("Losing Player %d has score %d and the die has been rolled %d times\n",
			(1-PlayerNr)+1, Score[1-PlayerNr], 3*Turn);
	printf("so %d x %d = %d\n", Score[1-PlayerNr], 3*Turn, Score[1-PlayerNr] * 3*Turn);

	return 0;
}		
