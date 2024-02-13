#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[100];
	char* InputPtr;
	int InputPos, InputLen;

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

	int NrOfLines=0, LineNr;
	while (fgets(InputLine, 100, InputFile))  NrOfLines++;
	printf("Parsed %d Lines\n", NrOfLines);

	struct Node;
	typedef struct Node {
		struct Node *Left, *Right;
		char LeftNum, RightNum;
		struct Node *Parent;
		char ParentLeftNotRight;
		unsigned long long Magnitude;
	} tNode;

unsigned long long MaxMagnitude = 0;
char InputLines[2][100];
int FirstLine,SecondLine;
for (FirstLine=0; FirstLine<NrOfLines; FirstLine++)
for (SecondLine=0; SecondLine<NrOfLines; SecondLine++)
{
	if (FirstLine == SecondLine) continue;

	tNode *Sum=NULL,*Term=NULL,*Current=NULL;
	int LeftNotRight;

	for (int TermNr=0; TermNr<2; TermNr++)
	{
		rewind(InputFile);
		for (LineNr=0; fgets(InputLine, 100, InputFile); LineNr++)
			if (LineNr == (TermNr?SecondLine:FirstLine))
			{
				strcpy(InputLines[TermNr], InputLine);
				break;
			}
	}

	for (int TermNr=0; TermNr<2; TermNr++)
	{
		strcpy(InputLine, InputLines[TermNr]);

		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		InputLen = strlen(InputLine);

	///////////////////////////    Get this Term parsed     ///////////////////////////
		Current = NULL;
		LeftNotRight = 1;
		for (InputPos=0; InputPos<InputLen; InputPos++)
			if (InputLine[InputPos] == '[')
			{
				// Need to create a new Node that will have Current as Parent
				tNode *Node = (tNode*)malloc(sizeof(tNode));
				Node->Left = NULL;  Node->Right = NULL;
				Node->LeftNum = -1;  Node->RightNum = -1;
				Node->Magnitude = 0;
				if (!Current)
				{
					// This is the very beginning of the InputLine
					Term = Node;
					Node->Parent = NULL;
				}
				else
				{
					Node->Parent = Current;
					Node->ParentLeftNotRight = LeftNotRight;
					if (LeftNotRight)
					{
						// The Left side cannot be already filled in
						if ((Current->Left) || (Current->LeftNum!=-1))
						{
							fprintf(stderr, "Missing Left side on InputLine #%d pos #%d\n%s",
									NrOfLines+1, InputPos, InputLine);
							exit(2);
						}
						Current->Left = Node;
					}
					else
					{
						// The Right side cannot be already filled in
						if ((Current->Right) || (Current->RightNum!=-1))
						{
							fprintf(stderr, "Missing Right side on InputLine #%d pos #%d\n%s",
									NrOfLines+1, InputPos, InputLine);
							exit(2);
						}
						Current->Right = Node;
					}
				}
				// Now the new Node becomes the new Current, always starting Left
				Current = Node;
				LeftNotRight = 1;
			}
			else if (InputLine[InputPos] == ']')
			{
				// Closing a Node can only happen at Right side
				if (LeftNotRight)
				{
					fprintf(stderr, "Not expecting closing bracket on InputLine #%d pos #%d\n%s",
							NrOfLines+1, InputPos, InputLine);
					exit(2);
				}
				// And both sides have to be already filled in
				if ((!Current->Left) && (Current->LeftNum==-1))
				{
					fprintf(stderr, "Missing Left side on InputLine #%d pos #%d\n%s",
							NrOfLines+1, InputPos, InputLine);
					exit(2);
				}
				if ((!Current->Right) && (Current->RightNum==-1))
				{
					fprintf(stderr, "Missing Right side on InputLine #%d pos #%d\n%s",
							NrOfLines+1, InputPos, InputLine);
					exit(2);
				}
				// This closes the Current, need to rise a level to its Parent
				LeftNotRight = Current->ParentLeftNotRight;
				Current = Current->Parent;
			}
			else if (InputLine[InputPos] == ',')
			{
				if (!LeftNotRight)
				{
					fprintf(stderr, "Not expecting another comma on InputLine #%d pos #%d\n%s",
							NrOfLines+1, InputPos, InputLine);
					exit(2);
				}
				// The Left side has to be already filled in
				if ((!Current->Left) && (Current->LeftNum==-1))
				{
					fprintf(stderr, "Missing Left side on InputLine #%d pos #%d\n%s",
							NrOfLines+1, InputPos, InputLine);
					exit(2);
				}
				LeftNotRight = 0;
			}
			else  // Should be a literal digit now
			{
				if ((InputLine[InputPos] < '0') || (InputLine[InputPos] > '9'))
				{
					fprintf(stderr, "Expecting a literal digit on InputLine #%d pos #%d\n%s",
							NrOfLines+1, InputPos, InputLine);
					exit(2);
				}
				char Num = InputLine[InputPos] - '0';
				if (LeftNotRight)
				{
					// The Left side cannot be already filled in
					if ((Current->Left) || (Current->LeftNum!=-1))
					{
						fprintf(stderr, "Unexpected Left side on InputLine #%d pos #%d\n%s",
								NrOfLines+1, InputPos, InputLine);
						exit(2);
					}
					Current->LeftNum  = Num;  Current->Left  = NULL;
				}
				else
				{
					// The Right side cannot be already filled in
					if ((Current->Right) || (Current->RightNum!=-1))
					{
						fprintf(stderr, "Unexpected Right side on InputLine #%d pos #%d\n%s",
								NrOfLines+1, InputPos, InputLine);
						exit(2);
					}
					Current->RightNum  = Num;  Current->Right  = NULL;
				}
			}
		// InputLine parsing complete
		if (Current)
		{
			fprintf(stderr, "InputLine #%d leaves incomplete Parent\n%s",
					NrOfLines+1, InputLine);
			exit(2);
		}

	///////////////////////////    Add this Term to the Sum     ///////////////////////////
		if (!Sum)
			Sum = Term;
		else
		{
			// Create a new Node that has Sum and Term as children
			tNode *Node = (tNode*)malloc(sizeof(tNode));
			Node->Left = Sum;  Node->Right = Term;
			Node->LeftNum = -1;  Node->RightNum = -1;
			// Tie the children Sum and Term properly to their parent
			Sum->Parent = Node;   Sum->ParentLeftNotRight = 1;
			Term->Parent = Node;  Term->ParentLeftNotRight = 0;
			Sum = Node;
		}

	///////////////////////////    Display the Sum & Magnitude     ///////////////////////////
		void DisplaySum(void)
		{
			Current = Sum;
			LeftNotRight = 1;
			for (;;)
				if (LeftNotRight)
				{
					putchar('[');
					if (Current->LeftNum >= 0)
					{
						printf("%hhd", Current->LeftNum);
						// Double-check
						if (Current->Left)
						{
							fprintf(stderr, "Left should be NULL then!");
							exit(3);
						}
						putchar(',');
						LeftNotRight = 0;
					}
					else
					{
						Current = Current->Left;
						LeftNotRight = 1;
					}
				}
				else // !LeftNotRight
				{
					if (Current->RightNum >= 0)
					{
						printf("%hhd", Current->RightNum);
						// Double-check
						if (Current->Right)
						{
							fprintf(stderr, "Right should be NULL then!");
							exit(3);
						}
						// May need to close multiple Nodes within the tree
						while (Current)
						{
							putchar(']');

							// Closing a Node is the suitable moment to compute its magnitude
							unsigned long long LeftMagnitude;
							if (Current->Left)
								LeftMagnitude = Current->Left->Magnitude;
							else
								LeftMagnitude = Current->LeftNum;
							unsigned long long RightMagnitude;
							if (Current->Right)
								RightMagnitude = Current->Right->Magnitude;
							else
								RightMagnitude = Current->RightNum;
							Current->Magnitude = 3*LeftMagnitude + 2*RightMagnitude;

							char wasParentLeftNotRight = Current->ParentLeftNotRight;
							Current = Current->Parent;
							if (wasParentLeftNotRight)  break;
						}
						if (!Current)
							// Also break out of the outer loop
							break;
						putchar(',');
						LeftNotRight = 0;
					}
					else
					{
						Current = Current->Right;
						LeftNotRight = 1;
					}
				} /* if (LeftNotRight) */
			printf("  with Magnitude %llu\n", Sum->Magnitude);
		} /* void DisplaySum(void) */

	///////////////////////////    Redude this Sum     ///////////////////////////
		printf("Fresh new Sum:   ");
		DisplaySum();

		// !!!!!!!!!!! FIRST any explosions, even if they occur rightward of a Split opportunity //

		for (;;)
		{
			Current = Sum;
			LeftNotRight = 1;
			int Level = 0;
			char *PreviousLiteral=NULL;
			char ExplodedRight = -1;
			for (;;)
			{
				if (LeftNotRight)
				{
					if ((++Level > 4) && (ExplodedRight == -1))
					{
						// Guaranteed to happen only for a fully-literal Node
						if (PreviousLiteral)  *PreviousLiteral += Current->LeftNum;
						ExplodedRight = Current->RightNum;
						// Replace this Node with a literal 0 within its Parent
						if (Current->ParentLeftNotRight)
						{
							Current->Parent->LeftNum = 0;
							Current->Parent->Left = NULL;
							// Continue from that Parent's Right side
							LeftNotRight = 0;
							Current = Current->Parent;
							Level--;
							continue;
						}
						else
						{
							Current->Parent->RightNum = 0;
							Current->Parent->Right = NULL;

							// May need to rise multiple Nodes within the tree
							while (Current)
							{
								Level--;
								char wasParentLeftNotRight = Current->ParentLeftNotRight;
								Current = Current->Parent;
								if (wasParentLeftNotRight)  break;
							}
							if (!Current)
								// Also break out of the outer loop
								break;
							LeftNotRight = 0;
							continue;
						}
					} /* if (++Level > 4) */
					if (Current->LeftNum >= 0)
					{
						if (ExplodedRight != -1)
						{
							Current->LeftNum += ExplodedRight;
							break;
						}
						PreviousLiteral = &(Current->LeftNum);
						LeftNotRight = 0;
					} /* if (Current->LeftNum >= 0) */
					else
					{
						Current = Current->Left;
						LeftNotRight = 1;
					}
				}
				else // !LeftNotRight
				{
					if (Current->RightNum >= 0)
					{
						if (ExplodedRight != -1)
						{
							Current->RightNum += ExplodedRight;
							break;
						}
						PreviousLiteral = &(Current->RightNum);

						// May need to close multiple Nodes within the tree
						while (Current)
						{
							Level--;
							char wasParentLeftNotRight = Current->ParentLeftNotRight;
							Current = Current->Parent;
							if (wasParentLeftNotRight)  break;
						}
						if (!Current)
							// Also break out of the outer loop
							break;
						LeftNotRight = 0;
					}
					else
					{
						Current = Current->Right;
						LeftNotRight = 1;
					}
				} /* if (LeftNotRight) */
			} /* for (;;) */

			if (ExplodedRight != -1)
			{
				printf("After Explosion: ");
				DisplaySum();
				continue;
			}

		// !!!!!!!!!!! Splits are only allowed after no more Explosions occur //

			Current = Sum;
			LeftNotRight = 1;
			for (;;)
			{
				if (LeftNotRight)
				{
					Level++;
					if (Current->LeftNum >= 0)
					{
						if (Current->LeftNum >= 10)
						{
							// Create a new Node to Split this high literal
							tNode *Node = (tNode*)malloc(sizeof(tNode));
							Node->LeftNum = Current->LeftNum>>1;
							Node->RightNum = (Current->LeftNum + 1)>>1;
							Node->Left = NULL;  Node->Right = NULL;
							// Tie this Node properly to its parent
							Current->LeftNum = -1;
							Current->Left = Node;
							Node->Parent = Current;
							Node->ParentLeftNotRight = 1;
							break;
						}
						LeftNotRight = 0;
					} /* if (Current->LeftNum >= 0) */
					else
					{
						Current = Current->Left;
						LeftNotRight = 1;
					}
				}
				else // !LeftNotRight
				{
					if (Current->RightNum >= 0)
					{
						if (Current->RightNum >= 10)
						{
							// Create a new Node to Split this high literal
							tNode *Node = (tNode*)malloc(sizeof(tNode));
							Node->LeftNum = Current->RightNum>>1;
							Node->RightNum = (Current->RightNum + 1)>>1;
							Node->Left = NULL;  Node->Right = NULL;
							// Tie this Node properly to its parent
							Current->RightNum = -1;
							Current->Right = Node;
							Node->Parent = Current;
							Node->ParentLeftNotRight = 0;
							break;
						}

						// May need to close multiple Nodes within the tree
						while (Current)
						{
							Level--;
							char wasParentLeftNotRight = Current->ParentLeftNotRight;
							Current = Current->Parent;
							if (wasParentLeftNotRight)  break;
						}
						if (!Current)
							// Also break out of the outer loop
							break;
						LeftNotRight = 0;
					}
					else
					{
						Current = Current->Right;
						LeftNotRight = 1;
					}
				} /* if (LeftNotRight) */
			} /* for (;;) */

			if (Current)
			{
				printf("After Split:     ");
				DisplaySum();
				continue;
			}
			break;
		} /* Looking for Explosions or Splits */

	} /* for (TermNr) */
	if (Sum->Magnitude > MaxMagnitude)  MaxMagnitude = Sum->Magnitude;

} /* for (SecondLine) */

printf("MaxMagnitude %llu\n", MaxMagnitude);
	return 0;
}		
