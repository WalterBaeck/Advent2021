#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	int ComponentsToUse, ToysToUse=20;

	FILE* InputFile = stdin;

	if (Arguments > 3)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s [InputFile [ToysToUse]]\n", Argument[0]);
		fprintf(stderr, "\twith no [InputFile] argument, data will be read from stdin\n");
		fprintf(stderr, "\t[ToysToUse] is the constraint for the length of the output string (default 20)\n");
		exit(1);
	}
	else if (Arguments == 3)
	{
		if (1 != sscanf(Argument[2], "%d", &ToysToUse))
		{
			fprintf(stderr, "Could not open %s for reading\n", Argument[2]);
			exit(2);
		}
	}
	if (Arguments >= 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n", Argument[1]);
			exit(2);
		}
	}

	char InputLine[100], *InputPtr;
	int InputField;
	int InputAmount;
	char InputName[100];
	int NrOfLines=0;

	// Compose a directed graph
	struct tNode {
		char* Name;
		int TotalAmount;
		int TotalKnown;
		int NrOfChildren;
		struct tChild* Child;
		int NrOfParents;
		struct tParent* Parent;
	};
	struct tChild {
		int Amount;
		int Nr;
	};
	struct tParent {
		int Nr;
		int ChildNr;
	};

	struct tNode* Node=NULL;
	int NodeNr,NrOfNodes=0;
	int ChildNr, ParentNr;

	int FindNode(char* ByName)
	{
		for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
			if (!strcmp(ByName, Node[NodeNr].Name))
				// Found existing node with this name, stop searching, return a pointer
				return NodeNr;
		// Arriving here means that Node was not found
		return -1;
	}

	int FindExistingNode(char* ByName)
	{
		int FoundNr = FindNode(ByName);
		if (FoundNr >= 0)
			return FoundNr;
		else
		{
			fprintf(stderr, "Existing Node %s not found ?\n", ByName);
			exit(4);
		}
	}

	int FindOrCreateNode(char* ByName)
	{
		int FoundNr = FindNode(ByName);
		if (FoundNr >= 0)
			return FoundNr;
		else
		{
			NrOfNodes++;
			if (!(Node = (struct tNode*)realloc(Node, NrOfNodes * sizeof(struct tNode))))
			{
				fprintf(stderr, "Could not extend Node array to %d Nodes\n", NrOfNodes);
				exit(3);
			}
			// Populate the new Node
			struct tNode* thisNode = &Node[NrOfNodes-1];
			thisNode->Name = malloc(strlen(ByName)+1);
			strcpy(thisNode->Name, ByName);
			thisNode->TotalAmount = 0;
			thisNode->TotalKnown = 0;
			thisNode->NrOfChildren = 0;
			thisNode->Child = NULL;
			thisNode->NrOfParents = 0;
			thisNode->Parent = NULL;
			return NrOfNodes-1;
		}
	} /* FindOrCreateNode() */


	while (fgets(InputLine, 100, InputFile))
	{
		if (!NrOfLines)
		{
			// This is the missing parts count
			if (1 != sscanf(InputLine, "%d", &ComponentsToUse))
			{
				fprintf(stderr, "Could not scan ComponentsToUse from first line:\n%s", InputLine);
				exit(2);
			}
			NrOfLines++;
			continue;
		}
		///////////////////////////   Get one node   ///////////////////////////
		for (InputField=0; InputPtr=strtok(InputField?NULL:InputLine, ":,\n"); InputField++)
		{
			static int thisNr;
			static struct tNode* thisNode;
			static char* thisName;
			if (!InputField)
			{
				// The first Field contains just the name of this node
				thisNr = FindOrCreateNode(InputPtr);
				thisNode = Node + thisNr;
				printf("Node %s being declared\n", thisNode->Name);
				thisName = thisNode->Name;
			}
			else
			{
				if (2 != sscanf(InputPtr, "%d %s", &InputAmount, InputName))
				{
					fprintf(stderr, "Could not scan InputField #%d from line #%d:\n%s",
							InputField, NrOfLines, InputLine);
					exit(2);
				}
				printf("..uses %d %s's: ", InputAmount, InputName);
				// Add this child
				thisNode->NrOfChildren++;
				if (!(thisNode->Child =
								(struct tChild*)realloc(thisNode->Child, thisNode->NrOfChildren*sizeof(struct tChild))))
				{
					fprintf(stderr, "Could not extend node %s to %d Children\n",
							thisNode->Name, thisNode->NrOfChildren);
					exit(3);
				}
				thisNode->Child[thisNode->NrOfChildren-1].Amount = InputAmount;
				int ChildNr = FindOrCreateNode(InputName);
				// Chance exists that Node array has gotten reallocated by previous call,
				//   so the thisNode pointer may have become invalid; search again
				thisNode = Node + thisNr;
				thisNode->Child[thisNode->NrOfChildren-1].Nr = ChildNr;
				struct tNode* ChildPtr = Node + ChildNr;
				printf("added a link to %s - ", ChildPtr->Name);
				// Also put a link back from this Child to its new Parent
				ChildPtr->NrOfParents++;
				if (!(ChildPtr->Parent =
								(struct tParent*)realloc(ChildPtr->Parent, ChildPtr->NrOfParents*sizeof(struct tParent))))
				{
					fprintf(stderr, "Could not extend child node %s to %d Parents\n",
							ChildPtr->Name, ChildPtr->NrOfParents);
					exit(3);
				}
				ChildPtr->Parent[ChildPtr->NrOfParents-1].Nr = thisNr;
				ChildPtr->Parent[ChildPtr->NrOfParents-1].ChildNr = thisNode->NrOfChildren-1;
				printf("and back to Parent %s which has this as Child #%d\n",
						Node[thisNr].Name, thisNode->NrOfChildren-1);
			}
		} /* for (InputField) */
		NrOfLines++;
	} /* while (fgets()) */
	printf("%d lines parsed\n", NrOfLines);

#if 0
	///////////////////////////   Graph complete   ///////////////////////////
	// Overview for debugging
	for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
	{
		struct tNode* thisNode = Node + NodeNr;
		printf("Node %d: %s ===========================\n", NodeNr, thisNode->Name);
		printf("Made of: ");
		for (ChildNr=0; ChildNr<thisNode->NrOfChildren; ChildNr++)
			printf("%d %s's  ", thisNode->Child[ChildNr].Amount, Node[thisNode->Child[ChildNr].Nr].Name);
		printf("\nUsed in: ");
		for (ParentNr=0; ParentNr<thisNode->NrOfParents; ParentNr++)
			printf("%s (as #%d)  ", Node[thisNode->Parent[ParentNr].Nr].Name, thisNode->Parent[ParentNr].ChildNr);
		printf("\n");
	}
#endif

	///////////////////////////   Count Components   ///////////////////////////
	// Keep going over the list and looking for TotalKnown component counts
	int NrOfUnknown=0;
	for (NrOfUnknown=0;;NrOfUnknown=0)
	{
		for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
			if (!Node[NodeNr].TotalKnown)
			{
				struct tNode* thisNode = Node + NodeNr;
				thisNode->TotalAmount = 0;
				for (ChildNr=0; ChildNr<thisNode->NrOfChildren; ChildNr++)
					if (!Node[thisNode->Child[ChildNr].Nr].TotalKnown)
						break;
					else
						thisNode->TotalAmount +=
							thisNode->Child[ChildNr].Amount * Node[thisNode->Child[ChildNr].Nr].TotalAmount;
				if (ChildNr<thisNode->NrOfChildren)
					NrOfUnknown++;
				else
				{
					// Special case: leaf nodes need 1 component to be built, themselves
					if (!thisNode->NrOfChildren)
						thisNode->TotalAmount = 1;
#if 0
					printf("Determining for Node #%d : %s the TotalAmount as %d\n",
							NodeNr, thisNode->Name, thisNode->TotalAmount);
#endif
					thisNode->TotalKnown = 1;
				}
			}
		// Did this iteration complete the graph ?
		//printf("NrOfUnknown is now %d\n", NrOfUnknown);
		if (!NrOfUnknown)  break;
	}

	///////////////////////////   Rank Toys   ///////////////////////////
		struct tToy {
			char* Name;
			int TotalAmount;
			int Used;
		};
		struct tToy* Toy = (struct tToy*)malloc(NrOfNodes * sizeof(struct tToy));
		int ToyNr=0,NrOfToys=0;

	// List the top nodes in descending TotalAmount order
	printf("Compose %d components by picking %d from:\n", ComponentsToUse, ToysToUse);
	int NrOfEligible;
	int* Eligible=(int*)malloc(NrOfNodes*sizeof(int));
	memset(Eligible, 0, NrOfNodes*sizeof(int));
	// Initialize the Eligible flag
	for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
	{
		struct tNode* thisNode = Node + NodeNr;
		if (!thisNode->NrOfParents)
			Eligible[NodeNr] = 1;
	}
	// Look for the largest TotalAmount until all Eligibles have been used up
	for (NrOfEligible=0;;NrOfEligible=0)
	{
		int MaxAmount=0, MaxNr=0;
		for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
		{
			struct tNode* thisNode = Node + NodeNr;
			if (Eligible[NodeNr])
			{
				NrOfEligible++;
				if (thisNode->TotalAmount > MaxAmount)
				{
					MaxAmount = thisNode->TotalAmount;
					MaxNr = NodeNr;
				}
			}
		}
		// Display the best result and remove it from the Eligible list
		printf("%s(%d)  -  ", Node[MaxNr].Name, MaxAmount);
		Eligible[MaxNr] = 0;
		NrOfEligible--;
		// Put this Toy into the ranked Toy list
		Toy[NrOfToys].Name = Node[MaxNr].Name;
		Toy[NrOfToys].TotalAmount = MaxAmount;
		Toy[NrOfToys].Used = 0;
		NrOfToys++;
		// Did this iteration complete the graph ?
		if (!NrOfEligible)  break;
	}
	printf("\n");

	///////////////////////////   Use Toys   ///////////////////////////
		int ToysUsed=0, ComponentsUsed=0;
		int ToysLeft, ComponentsLeft;
	for(;;)
	{
		// At the current ToyNr, decide how many to use of this Toy
		ComponentsLeft = ComponentsToUse - ComponentsUsed;
		ToysLeft = ToysToUse - ToysUsed;
		// Since Toys are ranked in descending order of Components,
		//   the constraints can't possibly be met if using this Toy
		//    for maximum possible amount won't fill in the ComponentsLeft.
		// Another reason to backtrack, is when we have gone past
		//    the 'right' edge of the array without satisfying constraints.
		if ((ToyNr >= NrOfToys) ||
				(ToysLeft * Toy[ToyNr].TotalAmount < ComponentsLeft))
		{
			// Need to backtrack to the 'left' of the array, to the larger Toys
			while (--ToyNr >= 0)
				if (Toy[ToyNr].Used)
					break;
			// Has backtracking brought us past the left edge of the array ?
			if (ToyNr < 0)
			{
				fprintf(stderr, "All Toy combinations exhausted\n");
				exit(5);
			}
			// If not, decrease the Toys used at this position, and move one step 'right'
			Toy[ToyNr].Used--;
			ToysUsed--;
			ComponentsUsed -= Toy[ToyNr].TotalAmount;
			printf("[Toys %2d  Components %5d]  Reduced to Used=%d at Toy #%d : %s\n",
					ToysUsed, ComponentsUsed, Toy[ToyNr].Used, ToyNr, Toy[ToyNr].Name);
			ToyNr++;
			continue;
		}
		// Use the maximum possible
		Toy[ToyNr].Used = ComponentsLeft / Toy[ToyNr].TotalAmount;
		ToysUsed += Toy[ToyNr].Used;
		ComponentsUsed += (Toy[ToyNr].Used * Toy[ToyNr].TotalAmount);
		printf("[Toys %2d  Components %5d]  Maximum Used=%d at Toy #%d : %s\n",
					ToysUsed, ComponentsUsed, Toy[ToyNr].Used, ToyNr, Toy[ToyNr].Name);
		// Does this choice satisfy all the constraints ?
		if ((ComponentsToUse == ComponentsUsed) && (ToysToUse == ToysUsed))
		{
			printf("Constraints met !\n");
			break;
		}
		// If not, move one step 'right' within the array
		ToyNr++;
	} /* for(;;) */

	///////////////////////////   Result String   ///////////////////////////
		char* Output = (char*)malloc(ToysToUse+1);
		memset(Output, 0, ToysToUse+1);
		// Reuse the Eligible array, as flags referring to the Toys array this time
	memset(Eligible, 0, NrOfNodes*sizeof(int));
	for (ToyNr=0; ToyNr<NrOfToys; ToyNr++)
		if (Toy[ToyNr].Used)  Eligible[ToyNr] = 1;
	// Also reuse this counter
	ToysUsed = 0;

	for (;;)
	{
		char LowestLetter='Z';
		int LowestNr = NrOfToys;
		// Look for the lowest first letter of the name of an Eligible Toy
		for (ToyNr=0; ToyNr<NrOfToys; ToyNr++)
			if ((Eligible[ToyNr]) && (Toy[ToyNr].Name[0] <= LowestLetter))
			{
				LowestLetter = Toy[ToyNr].Name[0];
				LowestNr = ToyNr;
			}
		printf("Found LowestNr=%d : %s with Letter %c\n", LowestNr, Toy[LowestNr].Name, LowestLetter);
		if (LowestNr == NrOfToys)
		{
			fprintf(stderr, "Could not find any low letters anymore\n");
			exit(5);
		}
		printf("Printing %d times letter %c from OutputPos %d onward\n",
				Toy[LowestNr].Used, LowestLetter, ToysUsed);
		int OutputPos;
		for (OutputPos=ToysUsed; OutputPos<ToysUsed+Toy[LowestNr].Used; OutputPos++)
			Output[OutputPos] = LowestLetter;
		Eligible[LowestNr] = 0;
		ToysUsed += Toy[LowestNr].Used;
		if (ToysUsed >= ToysToUse)  break;
	}
	printf("Result string: %s\n", Output);

	return 0;
}		
