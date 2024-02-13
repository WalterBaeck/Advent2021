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

	char* String = (char*)malloc(20000);
	memset(String, 0, 20000);
	int StringLength, StringPos;

	char LeftParent[100], RightParent[100], Child[100];
	int NrOfPairs=0, PairNr;

	///////////////////////////    Input the start string     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 100, InputFile))
	{
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!NrOfLines)
		{
			strcpy(String, InputLine);
			StringLength = strlen(InputLine);
		}
		NrOfLines++;
		if (!strlen(InputLine))  break;
	} /* while (fgets) */

	///////////////////////////    Input the rules     ///////////////////////////
	while (fgets(InputLine, 100, InputFile))
	{
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (strlen(InputLine) != 7)
		{
			fprintf(stderr, "Expecting a Rule of length 7 on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}
		if (strncmp(InputLine+2, " -> ", 4))
		{
			fprintf(stderr, "Expecting arrow in a Rule on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}

		LeftParent[NrOfPairs]  = InputLine[0];
		RightParent[NrOfPairs] = InputLine[1];
		Child[NrOfPairs]       = InputLine[6];
		NrOfPairs++;
	} /* while (fgets) */
	printf("%d lines scanned - String %s of length %d - %d Rules\n",
			NrOfLines, String, StringLength, NrOfPairs);

	///////////////////////////    Insertions     ///////////////////////////
	int Iteration;
	for (Iteration=0; Iteration<10; Iteration++)
	{
		char Left,Right;
		// Make room by copying each original letter to its new location
		Right = String[StringLength-1];
		if (2*StringLength-2 >= 20000)
		{
			fprintf(stderr, "No room for more than 20000 characters\n");
			exit(4);
		}
//printf("Moving far-Right %c to pos %d\n", Right, 2*StringLength-2);
		String[2*StringLength-2] = Right;
		for (StringPos=StringLength-2; StringPos>=0; StringPos--)
		{
			Left = String[StringPos];
//printf("Moving Left %c to pos %d\n", Left, 2*StringPos);
			String[2*StringPos] = Left;
			// Now go looking for the proper Rule to fit this Pair
			for (PairNr=0; PairNr<NrOfPairs; PairNr++)
				if ((Left == LeftParent[PairNr]) && (Right == RightParent[PairNr]))
					break;
			if (PairNr >= NrOfPairs)
			{
				fprintf(stderr, "Pair %c%c at pos %d has no Rule\n", Left, Right, StringPos);
				exit(5);
			}
			// Arriving here means that a Rule was found, so insert the Child now
//printf("Inserting Child %c at pos %d\n", Child[PairNr], 2*StringPos+1);
			String[2*StringPos+1] = Child[PairNr];
			// The Left side of this pair, will become the Right side of the next pair
			Right = Left;
		} /* for (StringPos) */
		StringLength <<= 1;
		StringLength--;
if (Iteration < 5)  printf("#%d: String %s Length %d\n", Iteration, String, StringLength);
	} /* for (Iteration) */

	///////////////////////////    Census     ///////////////////////////
	int Count[26]={0};
	int MaxCount=0, MinCount=1000, CharNr;
	for (StringPos=0; StringPos<StringLength; StringPos++)
		Count[String[StringPos] - 'A']++;
	for (CharNr=0; CharNr<26; CharNr++)
	{
		if ((Count[CharNr]) && (Count[CharNr] > MaxCount))  MaxCount = Count[CharNr];
		if ((Count[CharNr]) && (Count[CharNr] < MinCount))  MinCount = Count[CharNr];
	}
	printf("After 10 iterations, MaxCount %d MinCount %d subtracted: %d\n", MaxCount, MinCount, MaxCount - MinCount);

	return 0;
}		
