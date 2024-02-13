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

	// A slow Xspeed is preferred, because it allows of ample time to reach high altitude.
	// However there is a lower bound: by firing _too_ slow, the probe will never reach target-X.
	// Terminal X coordinate is given as Xspeed * (Xspeed + 1) / 2
	int XspeedStart = (int)sqrt(2 * Xmin);

	// A high positive Yspeed is preferred, because it will reach the highest altitude.
	// However, it will reach the same speed in negative sense, when the probe falls through Y=0
	// and from there it should not "miss" the target Y bounds. Notice how the arc in positive Y
	// range marks equal Y values for the rising and falling part !

	// That means that the _next_ Y position will be reached with the negative of YspeedStart,
	// minus 1 extra. This position should not overshoot the target Y range. Optimizing means
	// that simply, the maximum allowed Yspeed is the lower bound of the Y range, minus 1 ...
	int YspeedStart = Ymin-1;

	// Again, the top altitude reached will be given as Yspeed * (Yspeed + 1) / 2

	printf("Top possible altitude is %d\n", Ymin * (Ymin + 1) / 2);

	return 0;
}		
