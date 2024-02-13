#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[1000];
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

	///////////////////////////   How much is the Fish ?   ///////////////////////////
	int NrOfFish=0;
	long long Group[9]={0};
	int GroupNr, InputFish;

	fgets(InputLine, 1000, InputFile);
	while (InputPtr = strtok(NrOfFish?NULL:InputLine, ",\n"))
	{
		if (1 != sscanf(InputPtr, "%d", &InputFish))
		{
			fprintf(stderr, "Could not scan InputFish from InputPtr #%d:\n%s", NrOfFish, InputPtr);
			exit(2);
		}
		if ((InputFish < 0) || (InputFish > 8))
		{
			fprintf(stderr, "Weird InputFish #%d: %d from InputPtr %s\n", NrOfFish, InputFish, InputPtr);
			exit(2);
		}
		Group[InputFish]++;
		NrOfFish++;
	} /* while (strtok) */
	printf("%d fish scanned\n", NrOfFish);

	///////////////////////////   Let the days go by   ///////////////////////////
	int NrOfDays=0;
	long long NewGroup[9];
	long long SumFish;
	for (NrOfDays=1; NrOfDays<=256; NrOfDays++)
	{
		// Basically, the NewGroup counts are just a copy of the previous Group counts,
		//   shifted by one index position
		memcpy(NewGroup, Group+1, 8*sizeof(long long));
		// Special treatment of the old Group[0]:
		//   - it adds to NewGroup[6], joining the shifted old Group[7] there
		//   - it is copied into the previously empty NewGroup[8]
		NewGroup[6] += Group[0];
		NewGroup[8]  = Group[0];
		// Now put these NewGroup results into the Group array
		memcpy(Group, NewGroup, 9*sizeof(long long));
		// Compute the sum of all Fish
		SumFish = 0;
		for (GroupNr=0; GroupNr<9; GroupNr++)
			SumFish += Group[GroupNr];
		if ((NrOfDays<=18) || (NrOfDays==80) || (NrOfDays==256))
			printf("After %2d days, SumFish is %lld\n", NrOfDays, SumFish);
	}
	return 0;
}		
