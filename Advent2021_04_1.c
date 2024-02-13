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

		///////////////////////////   Get the Draw numbers   ///////////////////////////

	int Draw[100];
	int DrawNr, NrOfDraws=0;
	fgets(InputLine, 1000, InputFile);
	while (InputPtr = strtok(NrOfDraws?NULL:InputLine, ",\n"))
	{
		if (1 != sscanf(InputPtr, "%d", Draw+NrOfDraws))
		{
			fprintf(stderr, "Could not scan Draw #%d from: %s\n", NrOfDraws,InputPtr);
			exit(2);
		}
		NrOfDraws++;
	}

		///////////////////////////   Get the Boards   ///////////////////////////
		int NrOfLines=1;
		int NrOfBoards=0;
		int NrOfRows=0,NrOfColumns;
		int Board[100][5][5];

	while (fgets(InputLine, 100, InputFile))
	{
		if (!NrOfRows)
		{
			// Every Board starts with a blank line
			if (strcmp(InputLine, "\n"))
			{
				fprintf(stderr, "Blank line #%d expected instead of:\n%s", NrOfLines, InputLine);
				exit(2);
			}
		}
		else
		{
			// Get all numbers within this Row
			NrOfColumns = 0;
			while (InputPtr = strtok(NrOfColumns?NULL:InputLine, " \n"))
			{
				if (NrOfColumns > 4)
				{
					fprintf(stderr, "Expected only 5 numbers on line #%d\n", NrOfLines);
					exit(2);
				}
				if (1 != sscanf(InputPtr, "%d", &Board[NrOfBoards][NrOfRows-1][NrOfColumns]))
				{
					fprintf(stderr, "Could not scan [Board %d][Row %d][Column %d] from: %s\n",
							NrOfBoards, NrOfRows-1, NrOfColumns, InputPtr);
					exit(2);
				}
				NrOfColumns++;
			}
		} /* if (!NrOfRows) */
		if (++NrOfRows > 5)
		{
			NrOfRows=0;
			NrOfBoards++;
		}
		NrOfLines++;
	}
	// Should end nicely on a complete Board
	if (NrOfRows)
	{
		fprintf(stderr, "Expected full board #%d at line #%d\n", NrOfBoards, NrOfLines);
		exit(2);
	}
	printf("%d lines scanned - found %d Draws and %d Boards\n", NrOfLines, NrOfDraws, NrOfBoards);

	///////////////////////////   Play Bingo   ///////////////////////////
		unsigned char Hit[100][5][5];
		memset(Hit, 0, 100 * 5 * 5);
		int RowNr, ColumnNr, BoardNr;
		int CheckRow, CheckColumn;
		int Bingo;

		for (DrawNr=0; DrawNr<NrOfDraws; DrawNr++)
		{
			// Look for this Draw in all Boards
			for (BoardNr=0; BoardNr<NrOfBoards; BoardNr++)
			{
				int BoardWasHit=0;
				for (RowNr=0; RowNr<5; RowNr++)
				{
					for (ColumnNr=0; ColumnNr<5; ColumnNr++)
						if (Board[BoardNr][RowNr][ColumnNr] == Draw[DrawNr])
						{
							Hit[BoardNr][RowNr][ColumnNr] = 1;
							BoardWasHit = 1;
							break;
						}
					if (BoardWasHit)
						break;
				}
				if ((BoardWasHit) && (DrawNr >= 4))
				{
					// Check for Bingo in this Row and Column, on this Board
					Bingo = 0;
					for (CheckRow=0; CheckRow<5; CheckRow++)
						if (!Hit[BoardNr][CheckRow][ColumnNr])
							break;
					if (CheckRow >= 5)
						Bingo = 1;
					for (CheckColumn=0; CheckColumn<5; CheckColumn++)
						if (!Hit[BoardNr][RowNr][CheckColumn])
							break;
					if (CheckColumn >= 5)
						Bingo = 1;
				}
				if (Bingo)  break;
			} /* for (BoardNr) */
			if (Bingo)  break;
		} /* for (DrawNr) */
	
	///////////////////////////   Compute result   ///////////////////////////
	if (Bingo)
	{
		printf("Draw[#%d]=%d led to Bingo for [Board %d][Row %d][Column %d]\n",
				DrawNr, Draw[DrawNr], BoardNr, RowNr, ColumnNr);
		int Sum=0;
		for (CheckRow=0; CheckRow<5; CheckRow++)
			for (CheckColumn=0; CheckColumn<5; CheckColumn++)
				if (!Hit[BoardNr][CheckRow][CheckColumn])
					Sum += Board[BoardNr][CheckRow][CheckColumn];
		printf("Result is Sum %d x Draw %d = %d\n", Sum, Draw[DrawNr], Sum * Draw[DrawNr]);
	}

	return 0;
}		
