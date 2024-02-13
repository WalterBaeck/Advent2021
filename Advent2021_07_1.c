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
	int GroupNr, NrOfGroups=0;
	for (GroupNr=MinCrab; NrOfGroups < NrOfCrabs/2; GroupNr++)
		NrOfGroups += Group[GroupNr];
	// According to https://en.wikipedia.org/wiki/Geometric_median#Properties
	//   the geometric median of a 1-dimensional set is equal to 'the' median
	printf("After including points at X=%d, reached %d points out of %d total\n",
			--GroupNr, NrOfGroups, NrOfCrabs);

	///////////////////////////    Put that to the test    ///////////////////////////
	int TargetNr=GroupNr-5;
	if (TargetNr<MinCrab)  TargetNr=MinCrab;
	int Attempt, SumDistance;
	for (Attempt=0; Attempt<=10; Attempt++)
	{
		SumDistance = 0;
		for (GroupNr=MinCrab; GroupNr<=MaxCrab; GroupNr++)
			SumDistance += Group[GroupNr] * abs(GroupNr - TargetNr);
		printf("Target %3d has SumDistance %d\n", TargetNr, SumDistance);
		if (++TargetNr > MaxCrab)  break;
	}

	return 0;
}		
