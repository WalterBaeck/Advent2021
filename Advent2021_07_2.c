#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[5000];
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

	///////////////////////////    Get the set of all X-pos     ///////////////////////////
	int NrOfCrabs=0;
	int Group[2000]={0};
	int InputCrab, MinCrab=2000, MaxCrab=0;

	fgets(InputLine, 5000, InputFile);
	while (InputPtr = strtok(NrOfCrabs?NULL:InputLine, ",\n"))
	{
		if (1 != sscanf(InputPtr, "%d", &InputCrab))
		{
			fprintf(stderr, "Could not scan InputCrab from InputPtr #%d:\n%s", NrOfCrabs, InputPtr);
			exit(2);
		}
		if ((InputCrab < 0) || (InputCrab >= 2000))
		{
			fprintf(stderr, "Weird InputCrab #%d: %d from InputPtr %s\n", NrOfCrabs, InputCrab, InputPtr);
			exit(2);
		}
		// Bookkeeping the edge values
		if (InputCrab < MinCrab)
			MinCrab = InputCrab;
		if (InputCrab > MaxCrab)
			MaxCrab = InputCrab;
		Group[InputCrab]++;
		NrOfCrabs++;
	} /* while (strtok) */
	printf("%d crabs scanned, min=%d max=%d\n", NrOfCrabs, MinCrab, MaxCrab);

	///////////////////////////   Count up to half of the X-pos  ///////////////////////////
	int GroupNr, SumTotal=0;
	for (GroupNr=MinCrab; GroupNr<=MaxCrab; GroupNr++)
		SumTotal += GroupNr * Group[GroupNr];
	// Perhaps here, the average value is the best starting point ?
	printf("Found average %f\n", ((float)SumTotal)/NrOfCrabs); 
	GroupNr = (int)(((float)SumTotal)/NrOfCrabs + .5);

	///////////////////////////    Put that to the test    ///////////////////////////
	int TargetNr=GroupNr-5;
	if (TargetNr<MinCrab)  TargetNr=MinCrab;
	int Attempt, SumQuadDistance;
	for (Attempt=0; Attempt<=10; Attempt++)
	{
		SumQuadDistance = 0;
		for (GroupNr=MinCrab; GroupNr<=MaxCrab; GroupNr++)
		{
			int Distance = abs(GroupNr - TargetNr);
			SumQuadDistance += Group[GroupNr] * Distance * (Distance+1) / 2;
		}
		printf("Target %3d has SumQuadDistance %d\n", TargetNr, SumQuadDistance);
		if (++TargetNr > MaxCrab)  break;
	}

	return 0;
}		
