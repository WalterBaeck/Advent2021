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
	int Curr;
	int NrOfNumbers=0, NrOfIncreases=0;

#define cSliderSize 3
	int Slider[cSliderSize];
	int SliderPtr=0;
	int SumPrev, SumCurr=0;

	while (fgets(InputLine, 100, InputFile))
	{
		///////////////////////////   Get input number   ///////////////////////////
		if (feof(stdin))  break;
		if (1 != sscanf(InputLine, "%d", &Curr))
		{
			fprintf(stderr, "Could not scan integer from %s\n", InputLine);
			exit(3);
		}

		/////////////////////////////   Bookkeeping    /////////////////////////////
		NrOfNumbers++;
printf("DEBUG #%d: %d\n", NrOfNumbers, Curr);
		// First thing, consume the oldest stored number
		if (NrOfNumbers > cSliderSize)
		{
printf("          subtracting %d\n", Slider[SliderPtr]);
			SumCurr -= Slider[SliderPtr];
		}
		// Then use this freed-up slot to store the new number
		Slider[SliderPtr] = Curr;

		////////////////////////   Processing this number   ////////////////////////
		SumCurr += Curr;
printf("          added to sum %d\n", SumCurr);
		if ((SumCurr > SumPrev) && (NrOfNumbers > cSliderSize))
			NrOfIncreases++;

		///////////////////////////   Prepare for next   ///////////////////////////
		SumPrev = SumCurr;
		if (++SliderPtr >= cSliderSize)  SliderPtr=0;
	}

	printf("%d numbers scanned - %d window increases found\n", NrOfNumbers, NrOfIncreases);
	return 0;
}		
