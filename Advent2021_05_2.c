#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

		///////////////////////////   Get the Lines   ///////////////////////////
		int NrOfLines=0;
		unsigned char Grid[1000][1000];
		memset(Grid, 0, 1000 * 1000);
		int x1,x2,y1,y2;
		int x,y;

		while (fgets(InputLine, 100, InputFile))
		{
			if (4 != sscanf(InputLine, "%d,%d -> %d,%d", &x1, &y1, &x2, &y2))
			{
				fprintf(stderr, "Could not scan x1,y1 -> x2,y2 from InputLine #%d:\n%s", NrOfLines, InputLine);
				exit(2);
			}
			NrOfLines++;
			if (x1 == x2)
			{
				if (y1 < y2)
					for (y=y1; y<=y2; y++)
						Grid[x1][y]++;
				else
					for (y=y1; y>=y2; y--)
						Grid[x1][y]++;
			}
			else if (y1 == y2)
			{
				if (x1 < x2)
					for (x=x1; x<=x2; x++)
						Grid[x][y1]++;
				else
					for (x=x1; x>=x2; x--)
						Grid[x][y1]++;
			}
			else
			{
				if ((x1<=x2) && (y1<=y2))
					for (x=x1,y=y1; (x<=x2) && (y<=y2); x++,y++)
						Grid[x][y]++;
				else if ((x1<=x2) && (y1>=y2))
					for (x=x1,y=y1; (x<=x2) && (y>=y2); x++,y--)
						Grid[x][y]++;
				else if ((x1>=x2) && (y1<=y2))
					for (x=x1,y=y1; (x>=x2) && (y<=y2); x--,y++)
						Grid[x][y]++;
				else if ((x1>=x2) && (y1>=y2))
					for (x=x1,y=y1; (x>=x2) && (y>=y2); x--,y--)
						Grid[x][y]++;
			}
		} /* while (fgets) */
		printf("%d lines scanned\n", NrOfLines);

		///////////////////////////   Count occupancy   ///////////////////////////
			int NrOfHOV=0;
			for (x=0; x<1000; x++)
				for (y=0; y<1000; y++)
					if (Grid[x][y] >= 2)
						NrOfHOV++;
			printf("Counted %d HOV\n", NrOfHOV);

	return 0;
}		
