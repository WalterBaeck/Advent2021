#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

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

	int Xmin,Xmax,Ymin,Ymax;
	int NrOfStrings=0, InputValue;

	///////////////////////////    Get the target area     ///////////////////////////
	fgets(InputLine, 100, InputFile);
	if (strncmp(InputLine, "target area: ", 13))
	{
		fprintf(stderr, "Expecting target area on InputLine\n%s", InputLine);
		exit(2);
	}
	while (InputPtr = strtok(NrOfStrings?NULL:InputLine, "=.,\n"))
	{
		// Nothing to parse within the opening chunk, nor in the middle
		if ((NrOfStrings) && (NrOfStrings != 3))
			if (1 != sscanf(InputPtr, "%d", &InputValue))
			{
				fprintf(stderr, "Cannot interpret string %s as a number\n", InputPtr);
				exit(2);
			}
		switch (NrOfStrings)
		{
			case 1: Xmin = InputValue;  break;
			case 2: Xmax = InputValue;  break;
			case 4: Ymin = InputValue;  break;
			case 5: Ymax = InputValue;  break;
		}
		NrOfStrings++;
	}

	///////////////////////////    Bounds for the search     ///////////////////////////

	// Terminal X coordinate is given as Xspeed * (Xspeed + 1) / 2
	int XspeedBoundLow = (int)sqrt(2 * Xmin);
	int XspeedBoundHigh = Xmax;

	int YspeedBoundHigh = (-Ymin)-1;
	int YspeedBoundLow = Ymin;

	///////////////////////////    Simulations     ///////////////////////////
	int NrOfHits=0;
	int XspeedStart, YspeedStart;
	for (XspeedStart=XspeedBoundLow; XspeedStart<=XspeedBoundHigh; XspeedStart++)
		for (YspeedStart=YspeedBoundLow; YspeedStart<=YspeedBoundHigh; YspeedStart++)
		{
			int X=0,Y=0,Xspeed=XspeedStart,Yspeed=YspeedStart;
			for (;;)
			{
				// Compute new position and speed
				X += Xspeed;
				Y += Yspeed;
				if (Xspeed) Xspeed--;
				Yspeed--;
				// Look for a hit
				if ((X>=Xmin) && (X<=Xmax) && (Y>=Ymin) && (Y<=Ymax))
				{
					NrOfHits++;
					break;
				}
				// Decide that this trajectory is hopeless, once Y is below target window
				if (Y < Ymin)  break;
			} /* for (;;) */
		} /* for (YspeedStart) */

	printf("Found %d hitting trajectories\n", NrOfHits);
	return 0;
}		
