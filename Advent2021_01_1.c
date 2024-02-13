#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument)
{
	char InputLine[100];
	int Prev=-1, Curr;
	int NrOfNumbers=0, NrOfIncreases=0;

	while (fgets(InputLine, 100, stdin))
	{
		if (feof(stdin))  break;
		if (1 != sscanf(InputLine, "%d", &Curr))
		{
			fprintf(stderr, "Could not scan integer from %s\n", InputLine);
			exit(3);
		}
		if ((Curr > Prev) && (Prev >= 0))
			NrOfIncreases++;
		NrOfNumbers++;
		Prev = Curr;
	}

	printf("%d numbers scanned - %d increases found\n", NrOfNumbers, NrOfIncreases);
	return 0;
}		
