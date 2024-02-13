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

	char String[100]={0}, FarRight;
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
	FarRight = String[StringLength-1];
	printf("%d lines scanned - String %s of length %d - %d Rules\n",
			NrOfLines, String, StringLength, NrOfPairs);

	///////////////////////////    Analyze the rules     ///////////////////////////
	int NewLeftPair[100],NewRightPair[100];
	int Left, Right, SearchNr;
	for (PairNr=0; PairNr<NrOfPairs; PairNr++)
	{
		// Insertion of a Child into this Pair, leads to a new Pair at the left side ..
		Left = LeftParent[PairNr];
		Right = Child[PairNr];
		for (SearchNr=0; SearchNr<NrOfPairs; SearchNr++)
			if ((Left == LeftParent[SearchNr]) && (Right == RightParent[SearchNr]))
				break;
		if (SearchNr >= NrOfPairs)
		{
			fprintf(stderr, "Rule #%d Pair %c%c has to insert Child %c but NewLeftPair %c%c unknown ?\n",
					PairNr, Left, RightParent[PairNr], Right, Left, Right);
			exit(5);
		}
		NewLeftPair[PairNr] = SearchNr;
		// ... and a new Pair on the right side
		Left = Child[PairNr];
		Right = RightParent[PairNr];
		for (SearchNr=0; SearchNr<NrOfPairs; SearchNr++)
			if ((Left == LeftParent[SearchNr]) && (Right == RightParent[SearchNr]))
				break;
		if (SearchNr >= NrOfPairs)
		{
			fprintf(stderr, "Rule #%d Pair %c%c has to insert Child %c but NewRightPair %c%c unknown ?\n",
					PairNr, LeftParent[PairNr], Right, Left, Left, Right);
			exit(5);
		}
		NewRightPair[PairNr] = SearchNr;
	}

	///////////////////////////    Analyze the start string     ///////////////////////////
	long long PairCount[100]={0}, NewPairCount[100];
	for (StringPos=0; StringPos<StringLength-1; StringPos++)
	{
		Left = String[StringPos];
		Right = String[StringPos+1];
		for (SearchNr=0; SearchNr<NrOfPairs; SearchNr++)
			if ((Left == LeftParent[SearchNr]) && (Right == RightParent[SearchNr]))
				break;
		if (SearchNr >= NrOfPairs)
		{
			fprintf(stderr, "InputString at pos %d: %c%c cannot be found as a Pair ?\n", StringPos, Left, Right);
			exit(6);
		}
		PairCount[SearchNr]++;
	}

	///////////////////////////    Perform insertions     ///////////////////////////
	int Iteration;
	for (Iteration=0; Iteration<40; Iteration++)
	{
		memset(NewPairCount, 0, 100 * sizeof(long long));
		for (PairNr=0; PairNr<NrOfPairs; PairNr++)
		{
			NewPairCount[NewLeftPair[PairNr]] += PairCount[PairNr];
			NewPairCount[NewRightPair[PairNr]] += PairCount[PairNr];
		}
		// This becomes the new situation after this Iteration is finished
		for (PairNr=0; PairNr<NrOfPairs; PairNr++)  PairCount[PairNr] = NewPairCount[PairNr];
	} /* for (Iteration) */

	///////////////////////////    Census     ///////////////////////////
	long long Count[26]={0};
	long long MaxCount=0, MinCount=0;
	int CharNr;
	for (PairNr=0; PairNr<NrOfPairs; PairNr++)
		Count[LeftParent[PairNr] - 'A'] += PairCount[PairNr];
	// Don't forget the far-right character, which has never changed
	Count[FarRight - 'A']++;
	for (CharNr=0; CharNr<26; CharNr++)
	{
		if ((Count[CharNr]) && (Count[CharNr] > MaxCount))  MaxCount = Count[CharNr];
		if ((Count[CharNr]) && ((!MinCount) || (Count[CharNr] < MinCount)))  MinCount = Count[CharNr];
	}
	printf("After 10 iterations, MaxCount %lld MinCount %lld subtracted: %lld\n", MaxCount, MinCount, MaxCount - MinCount);

	return 0;
}		
