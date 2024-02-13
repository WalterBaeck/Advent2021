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

	// We discern 7 Parking Spots within the Hall, and 2 spots in each Room :
	//   #############
	//   #01.2.3.4.56#
	//   ###1#1#1#1###
	//     #0#0#0#0#
	//     #########
	unsigned char Parking[7]={0},Room[4][2];
	int ParkingNr,RoomNr;

	///////////////////////////    Input starting situation     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 100, InputFile))
	{
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		NrOfLines++;
		switch (NrOfLines)
		{
			case 1:
				if (strcmp(InputLine, "#############"))
				{
					fprintf(stderr, "Expecting ############# on InputLine #%d\n%s\n", NrOfLines, InputLine);
					exit(3);
				}
				break;
			case 2:
				if (strcmp(InputLine, "#...........#"))
				{
					fprintf(stderr, "Expecting #...........# on InputLine #%d\n%s\n", NrOfLines, InputLine);
					exit(3);
				}
				break;
			case 3:
				InputPos = 0;
				if (strncmp(InputLine, "###", 3))
				{
					fprintf(stderr, "Expecting ### on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
					exit(3);
				}
				InputPos += 3;
				for (RoomNr=0; RoomNr<4; RoomNr++)
				{
					if ((InputLine[InputPos] < 'A') || (InputLine[InputPos] > 'D'))
					{
						fprintf(stderr, "Expecting [A..D] on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
						exit(3);
					}
					Room[RoomNr][1] = InputLine[InputPos] - 'A' + 1;
					InputPos++;
					if (InputLine[InputPos] != '#')
					{
						fprintf(stderr, "Expecting # on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
						exit(3);
					}
					InputPos++;
				}
				if (strncmp(InputLine+InputPos, "##", 2))
				{
					fprintf(stderr, "Expecting ## on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
					exit(3);
				}
				break;
			case 4:
				InputPos = 0;
				if (strncmp(InputLine, "  #", 3))
				{
					fprintf(stderr, "Expecting   # on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
					exit(3);
				}
				InputPos += 3;
				for (RoomNr=0; RoomNr<4; RoomNr++)
				{
					if ((InputLine[InputPos] < 'A') || (InputLine[InputPos] > 'D'))
					{
						fprintf(stderr, "Expecting [A..D] on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
						exit(3);
					}
					Room[RoomNr][0] = InputLine[InputPos] - 'A' + 1;
					InputPos++;
					if (InputLine[InputPos] != '#')
					{
						fprintf(stderr, "Expecting # on InputLine #%d at pos %d\n%s\n", NrOfLines, InputPos, InputLine);
						exit(3);
					}
					InputPos++;
				}
				break;
			case 5:
				if (strcmp(InputLine, "  #########"))
				{
					fprintf(stderr, "Expecting   ######### on InputLine #%d\n%s\n", NrOfLines, InputLine);
					exit(3);
				}
				break;
		} /* switch (NrOfLines) */
	} /* while (fgets) */

	int Cost=0,MinCost=0;

	// Due to the constraints of the game, only certain move types are possible :
	// - the topmost occupant of any Room moves to any Parking spot : 4 x 7 possibilities in total
	// - the same movement in the opposite direction                : 4 x 7 possibilities in total

	///////////////////////////    Helper functions     ///////////////////////////
	void DecodeChoice(int Choice, int *whichRoom, int *whichParking, int *fromRoom)
	{
		if (Choice >= 28)
		{
			Choice -= 28;
			*fromRoom = 0;
		}
		else
			*fromRoom = 1;
		*whichRoom = Choice / 7;
		*whichParking = Choice % 7;
	} /* DecodeChoice() */

	int isChoicePossible(int whichRoom, int whichParking, int fromRoom)
	{
		if (fromRoom)
		{
			// Not possible if the Room is empty, there's nobody to make this move
			if ((!Room[whichRoom][1]) && (!Room[whichRoom][0]))  return 0;
			// Not possible if the Parking spot is already occupied
			if (Parking[whichParking])  return 0;
			// Sanity : never move the bottom occupant out of a Rom, when it is at home
			if (Room[whichRoom][0] == whichRoom + 1)
			{
				if (!Room[whichRoom][1])
					// No top occupant : the bottom occupant would have moved, refuse
					return 0;
				else if (Room[whichRoom][1] == whichRoom + 1)
					// The top occupant is also home, refuse to move it out
					return 0;
			}
		}
		else // !fromRoom
		{
			// Only allowed to go from Parking into home Room
			if (Parking[whichParking] != whichRoom + 1)  return 0;
			// Not possible if the Room is already full
			if ((Room[whichRoom][1]) && (Room[whichRoom][0]))  return 0;
			// Extra condition : not allowed if another type is still in that Room
			if ((Room[whichRoom][0]) && (Room[whichRoom][0] != whichRoom + 1))  return 0;
		}
		// Are there any obstacles on this route ?
		// Handle the outer Parking spots as their neighbor, once that neighbor has proven empty
		if (whichParking == 0)
		{ if (Parking[1])  return 0;  else whichParking = 1; }
		if (whichParking == 6)
		{ if (Parking[5])  return 0;  else whichParking = 5; }
		// There's always free movement between Room #0 and Parking #1,#2 / Room #1 and Parking #2,#3 etc.
		for (int ParkNr=whichParking+1; ParkNr<=whichRoom+1; ParkNr++)  // Park is left of Room
			if (Parking[ParkNr])  return 0;
		for (int ParkNr=whichRoom+2; ParkNr<whichParking; ParkNr++)     // Park is right of Room
			if (Parking[ParkNr])  return 0;
		// Arriving here means success
		return 1;
	} /* isChoicePossible() */

	int ComputeCost(int whichRoom, int whichParking, int fromRoom)
	{
		// Basic movement between Room #x and Parking #x+1,#x+2 has distance 2
		int theCost = 2;

		// We need to know who is doing the moving
		unsigned char mover;
		if (!fromRoom)
		{
			mover = Parking[whichParking];
			if (!Room[whichRoom][0])
				theCost++;
		}
		else  // fromRoom
		{
			mover = Room[whichRoom][1];
			if (!mover)
			{
				mover = Room[whichRoom][0];
				theCost++;
			}
		}

		// Handle the outer Parking spots as their neighbor, with an extra step
		if (whichParking == 0) { theCost++; whichParking = 1; }
		if (whichParking == 6) { theCost++; whichParking = 5; }
		// Account for further displacements
		for (int ParkNr=whichParking+1; ParkNr<=whichRoom+1; ParkNr++)  // Park is left of Room
			theCost += 2;
		for (int ParkNr=whichRoom+2; ParkNr<whichParking; ParkNr++)     // Park is right of Room
			theCost += 2;

		// Both the distance and the mover are known now
		switch (mover)
		{
			case 2:  theCost *= 10;     break;
			case 3:  theCost *= 100;    break;
			case 4:  theCost *= 1000;   break;
		}

		return theCost;
	} /* ComputeCost() */

	void PerformStep(int whichRoom, int whichParking, int fromRoom)
	{
		unsigned char mover;
		if (!fromRoom)
		{
			mover = Parking[whichParking];
			Parking[whichParking] = 0;
			if (Room[whichRoom][0])
				Room[whichRoom][1] = mover;
			else
				Room[whichRoom][0] = mover;
		}
		else
		{
			mover = Room[whichRoom][1];
			if (mover)  Room[whichRoom][1]=0;
			else
			{
				mover = Room[whichRoom][0];
				Room[whichRoom][0] = 0;
			}
			Parking[whichParking] = mover;
		}
	} /* PerformStep() */

	void PrintSituation()
	{
		for (int ParkNr=0; ParkNr<7; ParkNr++)
		{
			if (!Parking[ParkNr])  putchar('.');  else  putchar('A' + Parking[ParkNr] - 1);
			if ((ParkNr) && (ParkNr<5)) putchar('.');
		}
		putchar('@');
		for (int theRoom=0; theRoom<4; theRoom++)
		{
			for (int Spot=1; Spot>=0; Spot--)
				if (!Room[theRoom][Spot]) putchar('.'); else putchar('A'+Room[theRoom][Spot]-1);
			putchar('|');
		}
		putchar(' ');
	}

	///////////////////////////    Construct all Movesets     ///////////////////////////
	int NrOfSteps=0, StepNr;
#define cMaxSteps 100
	int Step[cMaxSteps]={-1};
	int OutOfRoom;

	// Forever keep trying to add one more Step to the current Moveset
	for (;;)
	{
		// From the current situation, is it possible to pick a further Choice ?
		int ChoiceNr = Step[NrOfSteps];
PrintSituation();
//printf("Iteration starts Step[%d] beyond Choice %d\n", NrOfSteps, ChoiceNr);
		for (ChoiceNr++; ChoiceNr<56; ChoiceNr++)
		{
			DecodeChoice(ChoiceNr, &RoomNr, &ParkingNr, &OutOfRoom);
			// It has to be a possible move
			if (!isChoicePossible(RoomNr, ParkingNr, OutOfRoom))  continue;
			// Reject this move when it brings the TotalCost over the best end-to-end Cost yet
			int ExtraCost = ComputeCost(RoomNr, ParkingNr, OutOfRoom);
			if ((MinCost) && (Cost + ExtraCost >= MinCost))       continue;
			// We found a new viable Step
			Cost += ExtraCost;
			break;
		}
		if (ChoiceNr >= 56)
		{
			// At the current Node, we have exhausted all Choices to make another Step.
			// Instead, we need to undo the last taken Step, and reconsider from there.
			if (!NrOfSteps)
				// Cannot backtrack any further : the search ends
				break;
			// Backtrack : break up the last taken Step
			ChoiceNr = Step[--NrOfSteps];
			DecodeChoice(ChoiceNr, &RoomNr, &ParkingNr, &OutOfRoom);
			// Undo this step by performing the move in the opposite direction
			PerformStep(RoomNr, ParkingNr, 1-OutOfRoom);
			// Then we can compute what this step has cost, from this starting situation
			Cost -= ComputeCost(RoomNr, ParkingNr, OutOfRoom);
printf("Have undone Step[%2d]=%2d : Parking %d %s Room %d, now TotalCost %d\n",
				NrOfSteps, ChoiceNr, ParkingNr, OutOfRoom ? "from" : "to", RoomNr, Cost);
			continue;
		}
		// Getting here means that a viable new Step was found to extend the Path
printf("Will perform Step[%2d]=%2d : Parking %d %s Room %d with TotalCost %d\n",
				NrOfSteps, ChoiceNr, ParkingNr, OutOfRoom ? "from" : "to", RoomNr, Cost);
		PerformStep(RoomNr, ParkingNr, OutOfRoom);
		Step[NrOfSteps++] = ChoiceNr;
		if (NrOfSteps >= cMaxSteps)
		{
			fprintf(stderr, "No room for more than %d Steps\n", cMaxSteps);
			exit(4);
		}
		// Not any choice has been made further in the Moveset
		Step[NrOfSteps] = -1;
		// If we have arrived at the desired situation, register this, and extend no further
		static const unsigned char EmptyParking[7] = {0};
		if (!memcmp(Parking, EmptyParking, 7))
		{
printf("+++++ Reached solution with Cost %d\n", Cost);
			if ((!MinCost) || (Cost < MinCost))  MinCost = Cost;
			// Ensure backtracking instead of extending the Moveset
			Step[NrOfSteps] = 56;
		}
	} /* for (;;) */
	
	printf("MinCost is %d\n", MinCost);

	return 0;
}		
