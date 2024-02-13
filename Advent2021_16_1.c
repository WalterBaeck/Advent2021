#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[1500];
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

	int InputPos, InputLen=0;
	int NrOfLines=0, LineNr;

	unsigned char Num[1500];
	while (fgets(InputLine, 1500, InputFile))
	{
	///////////////////////////    Get one full line of hex digits     ///////////////////////////
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		InputLen = strlen(InputLine);
		for (InputPos=0; InputPos<InputLen; InputPos++)
		{
			if ((InputLine[InputPos] >= '0') && (InputLine[InputPos] <= '9'))
				Num[InputPos] = InputLine[InputPos] - '0';
			else if ((InputLine[InputPos] >= 'A') && (InputLine[InputPos] <= 'F'))
				Num[InputPos] = InputLine[InputPos] - 'A' + 10;
			else
			{
				fprintf(stderr, "Cannot interpret line %d character %d: %c as a hex digit\n",
						NrOfLines+1, InputPos, InputLine[InputPos]);
				exit(2);
			}
		}
		int InputPos=0;
		int BitsLeft = 4;

	///////////////////////////    Helper function     ///////////////////////////
		unsigned int GetBitField(int NrBits)
		{
			// Start by using the remaining Bits of the current hex digit
			unsigned int Result = (unsigned int)(Num[InputPos] & ((1<<BitsLeft)-1));
			NrBits -= BitsLeft;
			// Now take as many extra hex digits as needed
			while (NrBits > 0)
			{
				Result <<= 4;
				Result |= (unsigned int)Num[++InputPos];
				NrBits -= 4;
			}
			// After the extra hex digits have been consumed, some bits might be left over
			BitsLeft = -NrBits;
			Result >>= BitsLeft;
			// This is the outcome
			return Result;
		}

	///////////////////////////    Start parsing     ///////////////////////////
		typedef struct {
			unsigned int Version, Type;
			unsigned int PacketsNotBits;
			unsigned int Count;
			unsigned int BitLen;
			unsigned int Value;
		} tElement;
		tElement Stack[100];
		tElement *Current=Stack, *Parent=NULL;
		int StackPtr=0;

		int TotalSum = 0;

		// Any remaining code requires at least 2 hex digits
		while (InputPos < InputLen-1)
		{
			Current->Version = GetBitField(3);
			Current->Type    = GetBitField(3);
			Current->Value   = 0;
			Current->BitLen  = 6;
			TotalSum += Current->Version;
			switch (Current->Type)
			{
				case 4:
					unsigned int MoreDigits;
					do {
						MoreDigits = GetBitField(1);
						Current->BitLen++;
						Current->Value <<= 4;
						Current->Value |= GetBitField(4);
						Current->BitLen += 4;
					} while (MoreDigits);
					break;
				default:
					Current->PacketsNotBits = GetBitField(1);
					Current->BitLen++;
					if (Current->PacketsNotBits)
					{
						Current->Count = GetBitField(11);
						Current->BitLen += 11;
					}
					else
					{
						Current->Count = GetBitField(15);
						Current->BitLen += 15;
					}
					// First of the subpackets is needed now: go a stack level deeper
					Parent = Current;
					Current = &Stack[++StackPtr];
					if (StackPtr >= 100)
					{
						fprintf(stderr, "No room for Stack deeper than 100 levels\n");
						exit(4);
					}
					continue;
			} /* switch (Type) */

			// Arriving here, means that Current element is complete, so we can rise a stack level.
			// This may also complete the Parent, etc, so we may need to rise several levels.
			for (;;)
			{
printf("[%2d] Type=%d  Value=%d  BitLen=%d\n", StackPtr, Current->Type, Current->Value, Current->BitLen);
				if (!Parent)  break;
				// The Parent is definitely an Operator-Type, as Literal-Type has no subpackets.
				// Accumulate all BitLen from subpackets into the Parent's BitLen
				Parent->BitLen += Current->BitLen;
				// No way to compute Parent->Value
				// Update the Parent's required Count.
				// If it is not satisfied yet, just stay at this stack level, and parse the next subpacket
				if (Parent->PacketsNotBits)
					Parent->Count--;
				else  // Bit counting
					Parent->Count -= Current->BitLen;
printf("[%2d] Parent wants %d more..\n", StackPtr, Parent->Count);
				if (Parent->Count)  break;
				// Arriving here, means that the Parent is also completed - should definitely rise a stack level
				Current = &Stack[--StackPtr];
printf("[%2d] ..Parent completed\n", StackPtr);
				if (StackPtr)
					Parent = &Stack[StackPtr-1];
				else
					Parent = NULL;
			} /* stack level rise */

		} /* while (InputPos) */

		NrOfLines++;
		printf("Line #%d has TotalSum %d\n", NrOfLines, TotalSum);
	} /* while (fgets) */
	printf("%d lines scanned\n", NrOfLines);

	return 0;
}		
