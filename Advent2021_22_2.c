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

	// We store a list of cubes: each one is bounded by 2 coordinates, in 3 dimensions
	// We'll only store the cubes for the 'On' state - all other cubicles are default 'Off'.
	//
	// Note that these coordinates are INCLUSIVE - the initial example 10..12 refers
	// to THREE (not two) cubicles in a row. It's best to think of these coordinates
	// as the center point of a cubicle, stretching from -1/2 to +1/2 around it.
	// This explains how the "initial region" from Part 1 [-50..+50] actually has 101 cubicles.
	int NrOfExisting=0,ExistingNr;
	int X,Y,Z,SizeX=101, SizeY=101, SizeZ=101;
	int* BoundAlloc = (int*)malloc(50000 * 3 * 2 * sizeof(int));
	memset(BoundAlloc, 0, 50000 * 3 * 2 * sizeof(int));
	// Array[X][Y][Z] referencing, requires a 2-D array of pointers to strips of <2> values
	int** BoundStrip = (int**)malloc(50000 * 3 * sizeof(int*));
	BoundStrip[0] = BoundAlloc;
	for (int i=1; i<50000*3; i++)  BoundStrip[i] = BoundStrip[i-1] + 2;
	// These strips are then pointed to by <50000> pointers, each pointing to <3> BoundStrips
	int*** Bound = (int***)malloc(50000 * sizeof(int**));
	Bound[0] = BoundStrip;
	for (int i=1; i<50000; i++)  Bound[i] = Bound[i-1] + 3;

	int NrOfLines=0, InputFieldNr;
	while (fgets(InputLine, 100, InputFile))
	{
		NrOfLines++;
	///////////////////////////    Input one cube     ///////////////////////////
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
		int NewBound[3][2];
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
			int Dim = (InputFieldNr-1)/3;
			NewBound[Dim][InputFieldNr-2-3*Dim] = Value;
		} /* while (strtok) */
printf("============== Putting [%d..%d][%d..%d][%d..%d] to %hhd\n", NewBound[0][0], NewBound[0][1],
				NewBound[1][0],NewBound[1][1],NewBound[2][0],NewBound[2][1], OnNotOff);

		// We will be intersecting this new cubes with all previous cubes known to be in 'On'
    // Each intersection may split off pieces of an existing cube, so the existing list
    // gets extended, but with smaller cubes.
    // More importantly, the incoming Action cube also gets fragmented upon each intersection,
    // and these fragments then need to ALL be considered for further intersections 
    // further down the existing list of cubes.
    // Whereas the existing cubes are only stored in the 'On' state, the Action list needs
    // to be registered in either state. When an Off action gets fragmented into pieces,
    // each of those pieces still has the final say in switching off cubicles within further
    // existing cubes down the list. But for a single InputLine, the Action is always constant.

    // Static allocation of this smaller 3-D array - do this only once
		static int Action[1000][3][2];
		int NrOfActions, ActionNr;
		// Put that single Action cube just read from input, into the first element of this list.
		int Dim, Side;
		for (Dim=0; Dim<3; Dim++)
			for (Side=0; Side<2; Side++)
				Action[0][Dim][Side] = NewBound[Dim][Side];
		NrOfActions = 1;

	///////////////////////////    Helper functions     ///////////////////////////
		void AddToExisting(int Nr, int Bounds[3][2])
		{
printf("+++++ Adding Existing #%d [%d..%d][%d..%d][%d..%d]\n", Nr,
		Bounds[0][0], Bounds[0][1], Bounds[1][0], Bounds[1][1], Bounds[2][0], Bounds[2][1]);
			// Shift out all Existing elements
			for (int ExistNr=NrOfExisting; ExistNr>Nr; ExistNr--)
				for (Dim=0; Dim<3; Dim++)
					for (Side=0; Side<2; Side++)
						Bound[ExistNr][Dim][Side] = Bound[ExistNr-1][Dim][Side];
			if (++NrOfExisting >= 50000)
			{
				fprintf(stderr, "No room for more than 50000 Existing\n");
				exit(4);
			}
			// Insert at this position of the array
			for (Dim=0; Dim<3; Dim++)
				for (Side=0; Side<2; Side++)
					Bound[Nr][Dim][Side] = Bounds[Dim][Side];
		} /* AddToExisting() */

		void AddToAction(int Nr, int Bounds[3][2])
		{
printf("      Adding Action #%d [%d..%d][%d..%d][%d..%d]\n", Nr,
		Bounds[0][0], Bounds[0][1], Bounds[1][0], Bounds[1][1], Bounds[2][0], Bounds[2][1]);
			// Shift out all Action elements
			for (int ActNr=NrOfActions; ActNr>Nr; ActNr--)
				for (Dim=0; Dim<3; Dim++)
					for (Side=0; Side<2; Side++)
						Action[ActNr][Dim][Side] = Action[ActNr-1][Dim][Side];
			if (++NrOfActions >= 1000)
			{
				fprintf(stderr, "No room for more than 1000 Actions\n");
				exit(4);
			}
			// Insert at this position of the array
			for (Dim=0; Dim<3; Dim++)
				for (Side=0; Side<2; Side++)
					Action[Nr][Dim][Side] = Bounds[Dim][Side];
		} /* AddToAction() */

		void RemoveFromExisting(int Nr)
		{
			NrOfExisting--;
printf("----- Remove Existing #%d [%d..%d][%d..%d][%d..%d]\n", Nr,
		Bound[Nr][0][0], Bound[Nr][0][1], Bound[Nr][1][0], Bound[Nr][1][1], Bound[Nr][2][0], Bound[Nr][2][1]);
			// Shift back all Existing elements
			for (int ExistNr=Nr; ExistNr<NrOfExisting; ExistNr++)
				for (Dim=0; Dim<3; Dim++)
					for (Side=0; Side<2; Side++)
						Bound[ExistNr][Dim][Side] = Bound[ExistNr+1][Dim][Side];
		} /* RemoveFromExisting() */

		void RemoveFromAction(int Nr)
		{
printf("      Remove Action #%d [%d..%d][%d..%d][%d..%d]\n", Nr,
		Action[Nr][0][0], Action[Nr][0][1], Action[Nr][1][0], Action[Nr][1][1], Action[Nr][2][0], Action[Nr][2][1]);
			NrOfActions--;
			// Shift back all Action elements
			for (int ActNr=Nr; ActNr<NrOfActions; ActNr++)
				for (Dim=0; Dim<3; Dim++)
					for (Side=0; Side<2; Side++)
						Action[ActNr][Dim][Side] = Action[ActNr+1][Dim][Side];
		} /* RemoveFromAction() */

	///////////////////////////    Interact with Existing cubes     ///////////////////////////

		for (ExistingNr=0; ExistingNr<NrOfExisting; ExistingNr++)
		{
			// Use local variable for ease of reference
			int ExistBound[3][2];
			for (Dim=0; Dim<3; Dim++)
				for (Side=0; Side<2; Side++)
					ExistBound[Dim][Side] = Bound[ExistingNr][Dim][Side];
			for (ActionNr=0; ActionNr<NrOfActions; ActionNr++)
			{
			// Use local variable for ease of reference
				int ActionBound[3][2];
				for (Dim=0; Dim<3; Dim++)
					for (Side=0; Side<2; Side++)
						ActionBound[Dim][Side] = Action[ActionNr][Dim][Side];

#if 0
printf("  %%   Exist #%d [%d..%d][%d..%d][%d..%d] Action #%d [%d..%d][%d..%d][%d..%d] ",
		ExistingNr, ExistBound[0][0], ExistBound[0][1], ExistBound[1][0], ExistBound[1][1],
		ExistBound[2][0], ExistBound[2][1],
		ActionNr, ActionBound[0][0], ActionBound[0][1], ActionBound[1][0], ActionBound[1][1],
		ActionBound[2][0], ActionBound[2][1]);
#endif

				// Determine whether any intersection happens between Existing and Action cube
				for (Dim=0; Dim<3; Dim++)
				{
					// No intersection when Max from one is lower than Min of the other
					if (ExistBound[Dim][1] < ActionBound[Dim][0])  break;
					if (ExistBound[Dim][0] > ActionBound[Dim][1])  break;
				}
				// Staying clear of each other in *any* of the 3 Dimensions, means no intersection
				if (Dim < 3)
				{
#if 0
printf(".. no\n");
#endif
					continue;
				}
#if 0
printf("INTERSECT\n");
#endif

	///////////////////////////    Intersection     ///////////////////////////
				// In 1-D, when line sections A and B intersect, 4 situations can arise.
				// The intersection always leads to 3 contiguous zones, delineated by the
				// 4 boundaries Amin, Bmin, Amax, Bmax (in whatever order they may occur)
				//
				//     Setup        Amin<Bmin  Amax>Bmax    Zone content (C=common)
				// A   ========
				// B     --------       v                      A C B
				// B     ----           v          v           A C A
				// B --------                      v           B C A
				// B ------------                              B C B
				//
				// Note how the middle zones always contains the common part (overlap between A and B),
				// and the outer zones cover either one of the original sections A or B, but not both.
				//
				// We simply repeat this process in all 3 dimensions.
				// As 3 contiguous zones arise in each of the 3 dimensions,
				// the space enveloping the Existing and the Action cube gets fragmented into
				// 3^3 = 27 smaller cubic spaces.
				unsigned char HasExist[3][3];   // Use only [0] and [2] for the outer zones
				int Boundary[3][4];   // The boundary points in between the 3 contiguous zones
				for (Dim=0; Dim<3; Dim++)
				{
					// Establish the 4 boundaries along this Dimension, and the contents of the 3 zones
					if (ExistBound[Dim][0] < ActionBound[Dim][0])
					{
						Boundary[Dim][0] = ExistBound[Dim][0];
						Boundary[Dim][1] = ActionBound[Dim][0];
						HasExist[Dim][0] = 1;
					}
					else  // ExistBound[Dim][0] >= ActionBound[Dim][0]
					{
						Boundary[Dim][0] = ActionBound[Dim][0];
						Boundary[Dim][1] = ExistBound[Dim][0];
						HasExist[Dim][0] = 0;
					}
					if (ExistBound[Dim][1] > ActionBound[Dim][1])
					{
						Boundary[Dim][2] = ActionBound[Dim][1];
						Boundary[Dim][3] = ExistBound[Dim][1];
						HasExist[Dim][2] = 1;
					}
					else  // ExistBound[Dim][1] <= ActionBound[Dim][1]
					{
						Boundary[Dim][2] = ExistBound[Dim][1];
						Boundary[Dim][3] = ActionBound[Dim][1];
						HasExist[Dim][2] = 0;
					}
				} /* for (Dim) */

				// When the Action is 'on', the entire Existing cube can remain.
				// Do not fragment it; just add the fragments from Action cube
				// that fall outside of Existing cube.
				// However if Action is 'off', then destroy the original cube,
				// and store the fragments that are not touched by Action cube
				// back into the existing list.
				if (!OnNotOff)
					RemoveFromExisting(ExistingNr--);
				else // OnNotOff
					RemoveFromAction(ActionNr--);

				// The only actual overlap within 3 dimensions, is the center cube of the 3^3=27 Club
				// that has now been fragmented. All other pieces of this 27 Club cover at most one
				// of the intersecting cubes (only when the 3 indicators HasExist[][] agree),
				// or empty space. Go over all pieces of the 27 Club.
				int Select[3]={0,0,-1};
				for (;;)
				{
					// Attempt to increment the Selection
					int SelectNr;
					for (SelectNr=3; SelectNr; SelectNr--)
						if (++Select[SelectNr-1] < 3)
							break;
						else
							Select[SelectNr-1] = 0;
					if (!SelectNr)  break;  // No more members in 27 Club

	///////////////////////////    27 Club     ///////////////////////////
					// Determine whether this cube fragment belongs to Existing or Action cube
					enum {eCenter, eExisting, eAction, eNone} CubeType = eCenter;
					// Being Center in any Dimension is 'neutral' : it still allows this fragment
					// to be determined either way
					for (Dim=0; Dim<3; Dim++)
					{
						if (Select[Dim] == 1)  continue;
						if (HasExist[Dim][Select[Dim]])
							switch (CubeType)
							{
								case eCenter:
								case eExisting:
									CubeType = eExisting;
									break;
								case eAction: // Conflict
									CubeType = eNone;
									break;
							}
						else // !HasExist[Dim][Select[Dim]]
							switch (CubeType)
							{
								case eCenter:
								case eAction:
									CubeType = eAction;
									break;
								case eExisting: // Conflict
									CubeType = eNone;
									break;
							}
						// No point in continuing the determination after conflict found
						if (CubeType == eNone)  break;
					} /* for (Dim) */
					if (CubeType == eNone)  continue;  // Empty space

					// Determine what the Bounds of this new Cube will be
					int SelectBound[3][2];					
					for (Dim=0; Dim<3; Dim++)
					{
						switch (Select[Dim])
						{
							case 0:
								SelectBound[Dim][0] = Boundary[Dim][0];
								SelectBound[Dim][1] = Boundary[Dim][1] - 1;
								break;
							case 1:
								SelectBound[Dim][0] = Boundary[Dim][1];
								SelectBound[Dim][1] = Boundary[Dim][2];
								break;
							case 2:
								SelectBound[Dim][0] = Boundary[Dim][2] + 1;
								SelectBound[Dim][1] = Boundary[Dim][3];
								break;
						}
						if (SelectBound[Dim][1] < SelectBound[Dim][0])  break;
					}
					if (Dim < 3)  continue;  // Non-existing fragment due to coinciding edge
					
	///////////////////////////    Taking Action     ///////////////////////////
					// Note that the Center fragment requires no further processing.
					// If the Action is 'On', the Existing cube still remains, and contains this.
					// Action 'Off' leaves the whole Action cube unfragmented, and contains this.
					switch (CubeType)
					{
						case eExisting:
							if (!OnNotOff)
								// When the whole Existing cube has been deleted,
								// some fragments are not affected by the 'Off' Action.
								// These surviving fragments should be added.
								AddToExisting(++ExistingNr, SelectBound);
							break;
						case eAction:
							if (OnNotOff)
								// The whole Existing cube remains, but those fragments of the Action
								// cube that fall outside it, should now be added as further Actions.
								AddToAction(++ActionNr, SelectBound);
							break;
					} /* switch (CubeType) */
				} /* over 27 Club */
			} /* for (ActionNr) */

		} /* for (ExistingNr) */

printf("..... NrOfExisting=%d  NrOfActions=%d\n", NrOfExisting, NrOfActions);

		// Once all the intersections for this InputLine have been handled,
		// all that's left to do, is to remember this Action list in case of 'On'
		if (OnNotOff)
			for (ActionNr=0; ActionNr<NrOfActions; ActionNr++)
			{
				for (Dim=0; Dim<3; Dim++)
					for (Side=0; Side<2; Side++)
						Bound[NrOfExisting][Dim][Side] = Action[ActionNr][Dim][Side];
printf("+++++ Convert Action #%d->Exist#%d [%d..%d][%d..%d][%d..%d]\n", ActionNr, NrOfExisting,
		Action[ActionNr][0][0], Action[ActionNr][0][1], Action[ActionNr][1][0], Action[ActionNr][1][1],
		Action[ActionNr][2][0], Action[ActionNr][2][1]);
				if (++NrOfExisting >= 50000)
				{
					fprintf(stderr, "No room for more than 50000 Existing\n");
					exit(4);
				}
			} /* for (ActionNr) */
	} /* while (fgets) */
	printf("Parsed %d Lines\n", NrOfLines);

	///////////////////////////    Counting Cubicles     ///////////////////////////
	unsigned long long NumCubicles = 0;
	for (ExistingNr=0; ExistingNr<NrOfExisting; ExistingNr++)
	{
		unsigned long long ExtraCubicles = 1;
		for (int Dim=0; Dim<3; Dim++)  ExtraCubicles *= (Bound[ExistingNr][Dim][1] - Bound[ExistingNr][Dim][0] + 1);
		NumCubicles += ExtraCubicles;
printf("***** Counted Existing #%d [%d..%d][%d..%d][%d..%d] : + %llu = %llu\n", ExistingNr, 
		Bound[ExistingNr][0][0], Bound[ExistingNr][0][1], Bound[ExistingNr][1][0], Bound[ExistingNr][1][1], Bound[ExistingNr][2][0], Bound[ExistingNr][2][1],
ExtraCubicles, NumCubicles);
	}
	printf("%llu cubicles are on\n", NumCubicles);
	return 0;
}		
