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

	///////////////////////////    Scoring the 27 Club     ///////////////////////////
	// There are 3x3x3 possible outcomes when a player performs his 3 Throws.
	// This forks off 27 possible new universes, but the sum of the 3 Throws
	// can only range from 3x1=3 (minimum) to 3x3=9 (maximum).
	//
	// These totals could be computed, but they are here enumerated by hand
	int Club27[10]={0,0,0,1,3,6,7,6,3,1};

	///////////////////////////    Discerning states     ///////////////////////////
	// At any given moment during the game, there are 4 variables that uniquely
	// definy the current situation : the Score and the Position of both players.
	// As more and more Universes keep forking off, there will be a higher amount
	// of occupants for each unique State defined by these 4 numbers.
	//
	// Note that the number of possible States will not expand indefinitely.
	// Both the Position variables will always remain restricted to 10x10 options.
	// The possibly evolution of Scores *does* increase the conceivable options,
	// by spreading out the Score of the current Player over the 7 possible values
	// of the 27 Club.
	//
	// Eventually, more and more of the States will be culled, as a Score number
	// reaches 21. That reduces its amount of occupants to 0, so it can be removed
	// from the exhaustive list of possibly reached States.
	typedef struct {
		unsigned long long Universes;
		unsigned char Score[2];
		unsigned char Position[2];
	} tState;
	tState *State = (tState*)malloc(sizeof(tState));
	int NrOfStates=1, StateNr, MaxNrOfStates=0;

	// Start out with a single possible Universe, with the given starting positions.
	State->Universes = 1;
	State->Score[0] = 0;
	State->Score[1] = 0;
	State->Position[0] = Position[0];
	State->Position[1] = Position[1];

	unsigned long long PlayerWins[2]={0};

	///////////////////////////    Play the game     ///////////////////////////
	PlayerNr=0;
	for (;;)
	{
		Turn++;

		printf("====== Turn %d ======\n", Turn);
		printf("---- Starting with %d States: ----\n", NrOfStates);
#if 0
		if (Turn < 6)
		{
			tState *StatePtr;
		for (StateNr=0,StatePtr=State; StateNr<NrOfStates; StateNr++,StatePtr++)
			printf("[%4d] Pos[%2d,%2d] Score[%2d,%2d] Universes=%llu\n", StateNr,
					StatePtr->Position[0], StatePtr->Position[1], StatePtr->Score[0], StatePtr->Score[1],
					StatePtr->Universes);
		}
#endif

		// A list of NewStates will need to be compiled upon every turn.
		tState *NewState = NULL;
		int NrOfNewStates=0;

		tState *StatePtr;
		for (StateNr=0,StatePtr=State; StateNr<NrOfStates; StateNr++,StatePtr++)
			// A player makes his 3 Throws, which forks of 27 new Universes
			// from all the currently counted Universes that have reached this State.
			// From each existing State, go over all members of the 27 Club
			for (int Advance=3; Advance<=9; Advance++)
			{
				// Create a NewState that initially has all the same properties as the original State
				tState thisNewState = *StatePtr;
				// Now adjust the fields of this NewState as needed.
				thisNewState.Universes *= Club27[Advance];
				thisNewState.Position[PlayerNr] += Advance;
				if (thisNewState.Position[PlayerNr] > 10)  thisNewState.Position[PlayerNr] -= 10;
				thisNewState.Score[PlayerNr] += thisNewState.Position[PlayerNr];

				// Does the Score that was reached now, make this Player win ?
				if (thisNewState.Score[PlayerNr] >= 21)
					PlayerWins[PlayerNr] += thisNewState.Universes;
				else
				{
					// If not, then store this NewState into the growing array of all NewStates
					NewState = (tState*)realloc(NewState, ++NrOfNewStates * sizeof(tState));
					memcpy(NewState + NrOfNewStates - 1, &thisNewState, sizeof(tState));
				}
			} /* for (Advance) */

		printf("---- Obtained %d NewStates: ----\n", NrOfNewStates);
#if 0
		if (Turn < 6)
		{
			int NewStateNr;
			tState *NewStatePtr;
		for (NewStateNr=0,NewStatePtr=NewState; NewStateNr<NrOfNewStates; NewStateNr++,NewStatePtr++)
			printf("[%4d] Pos[%2d,%2d] Score[%2d,%2d] Universes=%llu\n", NewStateNr,
					NewStatePtr->Position[0], NewStatePtr->Position[1], NewStatePtr->Score[0], NewStatePtr->Score[1],
					NewStatePtr->Universes);
		}
#endif

#if 1
		// There may be doubles within the array of NewStates that could be collapsed
		tState *First, *Second;
		int FirstNr, SecondNr;
		for (FirstNr=0; FirstNr<NrOfNewStates; FirstNr++)
		{
			First = NewState + FirstNr;
			for (SecondNr=FirstNr+1; SecondNr<NrOfNewStates; SecondNr++)
			{
				Second = NewState + SecondNr;
				if ((First->Position[0] == Second->Position[0]) &&
						(First->Position[1] == Second->Position[1]) &&
						(First->Score[0] == Second->Score[0]) &&
						(First->Score[1] == Second->Score[1]))
				{
//					printf("** Identical ** %d == %d : Pos[%2d,%2d] Score [%2d,%2d] Universes %llu + %llu\n",
//							FirstNr, SecondNr, First->Position[0], First->Position[1], First->Score[0], First->Score[1],
//							First->Universes, Second->Universes);
					// Exactly the same State variables : therefore these two NewStates can be combined
					First->Universes += Second->Universes;
					// Erase the Second NewState by moving all its neighbors one spot closer
					for (int MoveNr=SecondNr; MoveNr<NrOfNewStates-1; MoveNr++)
						memcpy(NewState + MoveNr, NewState + MoveNr + 1, sizeof(tState));
					// The array has gotten one element shorter now
					NrOfNewStates--;
					// Make sure that the next pass through the Second loop
					//   will consider the just-moved Second NewState *again*
					SecondNr--;
				} /* if (states identical) */
			} /* for (SecondNr) */
		} /* for (FirstNr) */
#endif

#if 0
		printf("---- Reduced to %d NewStates: ----\n", NrOfNewStates);
		if (Turn < 6)
		{
			int NewStateNr;
			tState *NewStatePtr;
		for (NewStateNr=0,NewStatePtr=NewState; NewStateNr<NrOfNewStates; NewStateNr++,NewStatePtr++)
			printf("[%4d] Pos[%2d,%2d] Score[%2d,%2d] Universes=%llu\n", NewStateNr,
					NewStatePtr->Position[0], NewStatePtr->Position[1], NewStatePtr->Score[0], NewStatePtr->Score[1],
					NewStatePtr->Universes);
		}
#endif


		// We can stop playing if all States have been pruned.
		if (!NrOfNewStates)  break;

		// After all the NewStates have been computed, this array can simply become the State array
		// in the next Turn. We don't need the old State array anymore.
		free(State);
		State = NewState;
		NrOfStates = NrOfNewStates;
		if (NrOfStates > MaxNrOfStates)  MaxNrOfStates = NrOfStates;

		// This Turn is completed
		PlayerNr = 1 - PlayerNr;
	}

	printf("After %d Turns, Player1 wins %llu universes and Player2 wins %llu\n", Turn, PlayerWins[0], PlayerWins[1]);
	printf("Dealt with MaxNrOfStates = %d\n", MaxNrOfStates);

	return 0;
}		
