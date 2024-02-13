#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Variations.h"

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

	//////////////////////    Prepare to permute the original 7-segment display     //////////////////////
	int Orig[10][7] = {{1,1,1,0,1,1,1},    // Digit 0 = abc.efg
										 {0,0,1,0,0,1,0},    // Digit 1 = ..c..f.
										 {1,0,1,1,1,0,1},    // Digit 2 = a.cde.g
										 {1,0,1,1,0,1,1},    // Digit 3 = a.cd.fg
										 {0,1,1,1,0,1,0},    // Digit 4 = .bcd.f.
										 {1,1,0,1,0,1,1},    // Digit 5 = ab.d.fg
										 {1,1,0,1,1,1,1},    // Digit 6 = ab.defg
										 {1,0,1,0,0,1,0},    // Digit 7 = a.c..f.
										 {1,1,1,1,1,1,1},    // Digit 8 = abcdefg
										 {1,1,1,1,0,1,1}};   // Digit 9 = abcd.fg

	int NrOfLines=0;
	int TotalSum=0;
	while (fgets(InputLine, 100, InputFile))
	{
	///////////////////////////    Get one full line of segments     ///////////////////////////
		int NrOfStrings=0;
		int Seen[10][7];
		int Display[4][7];
		int InputPos;
		memset(Seen, 0, 10 * 7 * sizeof(int));
		memset(Display, 0, 4 * 7 * sizeof(int));
		while (InputPtr = strtok(NrOfStrings?NULL:InputLine, " |\n"))
		{
			for (InputPos=0; InputPos<strlen(InputPtr); InputPos++)
				if (NrOfStrings<10)
					Seen[NrOfStrings][InputPtr[InputPos]-'a'] = 1;
				else
					Display[NrOfStrings-10][InputPtr[InputPos]-'a'] = 1;
			NrOfStrings++;
		} /* while (strtok) */

		// Now compute all possible ways to permutate the 7 original segments
		// For each possible permutation, see if it fits with the 10 patterns Seen here
		int Garbled[10][7];
		int Perm[7];
		InitPermutation(Perm, 7);
		do {
			int DigitNr, SegmentNr;
			// For this permutation, compute the 10 Garbled patterns that would arise
			for (DigitNr=0; DigitNr<10; DigitNr++)
				for (SegmentNr=0; SegmentNr<7; SegmentNr++)
					Garbled[DigitNr][SegmentNr] = Orig[DigitNr][Perm[SegmentNr]];
			// Try to match the Garbled patterns with the Seen patterns
			int SeenIsDigit[10], DigitUsed[10];
			memset(DigitUsed, 0, 10 * sizeof(int));
			for (DigitNr=0; DigitNr<10; DigitNr++)
			{
				int GarbledNr;
				for (GarbledNr=0; GarbledNr<10; GarbledNr++)
				{
					if (DigitUsed[GarbledNr])  continue;
					for (SegmentNr=0; SegmentNr<7; SegmentNr++)
						if (Seen[DigitNr][SegmentNr] != Garbled[GarbledNr][SegmentNr])
							break;
					if (SegmentNr >= 7)
					{
						SeenIsDigit[DigitNr] = GarbledNr;
						DigitUsed[GarbledNr] = 1;
						break;
					}
				}
				if (GarbledNr >= 10)
					// No Garbled pattern could be found to match this Seen pattern
					//   So this Permutation is not viable for the set of Seen patterns
					break;
			}
			if (DigitNr >= 10)
			{
				printf("Matching permutation found: ");
				for (SegmentNr=0; SegmentNr<7; SegmentNr++)
					printf("%d", Perm[SegmentNr]);
				printf(" - decoding digits as:");
				int DisplayNumber = 0;
				for (DigitNr=0; DigitNr<4; DigitNr++)
				{
					int GarbledNr;
					for (GarbledNr=0; GarbledNr<10; GarbledNr++)
					{
						for (SegmentNr=0; SegmentNr<7; SegmentNr++)
							if (Display[DigitNr][SegmentNr] != Garbled[GarbledNr][SegmentNr])
								break;
						if (SegmentNr >= 7)
						{
							printf(" %d", GarbledNr);
							DisplayNumber *= 10;
							DisplayNumber += GarbledNr;
							break;
						}
					}
				}
				printf(" - Number=%d\n", DisplayNumber);
				TotalSum += DisplayNumber;
				break;
			}
		} while (NextPermutation(Perm, 7));
		NrOfLines++;
	} /* while (fgets) */
	printf("%d lines scanned - TotalSum is %d\n", NrOfLines, TotalSum);

	return 0;
}		
