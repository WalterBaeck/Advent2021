#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;

	if (Arguments == 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n");
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

	char InputLine[100];
	int InputPos, InputLen=0;
	int NumberNr, NrOfNumbers=0;

	unsigned char Bits[2000][20];

	while (fgets(InputLine, 100, InputFile))
	{
		///////////////////////////   Get input number   ///////////////////////////
		if (feof(InputFile))  break;
		for (InputPos=0;; InputPos++)
		{
			if (InputLine[InputPos] == '1')
				Bits[NrOfNumbers][InputPos] = 1;
			else if (InputLine[InputPos] != '0')
				break;
			else
				Bits[NrOfNumbers][InputPos] = 0;
		}
		if (!InputLen)
			InputLen = InputPos;
		else if (InputLen != InputPos)
		{
			fprintf(stderr, "Established InputLen %d earlier but now found Len=%d in Number #%d: %s\n",
					InputLen, InputPos, NrOfNumbers, InputLine);
			exit(3);
		}
		NrOfNumbers++;
	}
	printf("%d numbers scanned\n", NrOfNumbers);

	///////////////////////////   Processing results   ///////////////////////////
		typedef enum {
			eMajority,
			eMinority,
		} tQuest;
		tQuest Quest;
		int Majority=0, Minority=0;
		unsigned char BitGoal;
		int BitWeight;
		int NrOfOnes;
	int NrOfEligible, Eligible[2000]={0};

	for (Quest=eMajority; Quest<=eMinority; Quest++)
	{
		printf("============ QUEST: %s ============\n", (Quest==eMajority) ? "Majority" : "Minority");

		// Reset the Eligibility array
		for (NumberNr=0; NumberNr<NrOfNumbers; NumberNr++)
			Eligible[NumberNr] = 1;
		NrOfEligible = NrOfNumbers;

		// Go over the bits from left to right
		for (InputPos=0; InputPos<InputLen; InputPos++)
		{
			// Count the bit at this position within all remaining Eligible numbers
			NrOfOnes = 0;
			for (NumberNr=0; NumberNr<NrOfNumbers; NumberNr++)
				if (Eligible[NumberNr])
					if (Bits[NumberNr][InputPos])
						NrOfOnes++;

			// Determine what the desired bit is going to be
			if (Quest == eMajority)
			{ if (NrOfOnes*2 < NrOfEligible)	BitGoal=0;  else BitGoal=1; }
			else
			{ if (NrOfOnes*2 < NrOfEligible)	BitGoal=1;  else BitGoal=0; }
			printf("Found %d one-bits among %d Eligibles, so BitGoal is set to %hhd",
					NrOfOnes, NrOfEligible, BitGoal);

			// Go over remaining Eligible numbers again, and apply the new goal to further reduce Eligibles
			NrOfEligible = 0;
			for (NumberNr=0; NumberNr<NrOfNumbers; NumberNr++)
				if (Eligible[NumberNr])
				{
					if (Bits[NumberNr][InputPos] == BitGoal)
						NrOfEligible++;
					else
						Eligible[NumberNr] = 0;
				}
			printf(" ... leaves %d Eligibles\n", NrOfEligible);

			// When only 1 number remains Eligible, find it and compute it from its Bits
			if (NrOfEligible == 1)
			{
				for (NumberNr=0; NumberNr<NrOfNumbers; NumberNr++)
					if (Eligible[NumberNr])  break;
				int NumberGoal=0;
				BitWeight=1<<(InputLen-1);
				for (InputPos=0; InputPos<InputLen; InputPos++)
				{
					if (Bits[NumberNr][InputPos])
						NumberGoal += BitWeight;
					BitWeight >>= 1;
				}
				if (Quest == eMajority)
					Majority = NumberGoal;
				else
					Minority = NumberGoal;
				break;  /* out of the larger for(InputPos) loop */
			}
		} /* for (InputPos) */
	} /* for (Quest) */


	printf("Multiplication result: %d * %d = %d\n", Majority, Minority, Majority*Minority);
	return 0;
}		
