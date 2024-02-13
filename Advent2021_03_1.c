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
	int NrOfNumbers=0, NrOfOnes[100]={0};

	while (fgets(InputLine, 100, InputFile))
	{
		///////////////////////////   Get input number   ///////////////////////////
		if (feof(InputFile))  break;
		for (InputPos=0;; InputPos++)
		{
			if (InputLine[InputPos] == '1')
				NrOfOnes[InputPos]++;
			else if (InputLine[InputPos] != '0')
				break;
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

	///////////////////////////   Processing results   ///////////////////////////
		int Majority=0, Minority=0;
		int BitWeight=1<<(InputLen-1);

	printf("%d numbers scanned - frequencies {", NrOfNumbers);
	for (InputPos=0; InputPos<InputLen; InputPos++)
	{
		printf("%d%s", NrOfOnes[InputPos], (InputPos==InputLen-1) ? "}\n" : ",");
		if (NrOfOnes[InputPos] > NrOfNumbers/2)
			Majority += BitWeight;
		else
			Minority += BitWeight;
		BitWeight >>= 1;
	}

	printf("Multiplication result: %d * %d = %d\n", Majority, Minority, Majority*Minority);
	return 0;
}		
