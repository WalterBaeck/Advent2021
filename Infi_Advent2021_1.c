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

	int FindExistingNode(char* ByName)
	{
		for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
			if (!strcmp(ByName, Node[NodeNr].Name))
				// Found existing node with this name, stop searching, return a pointer
				return NodeNr;
		// Arriving here means that Node was not found
		fprintf(stderr, "Existing Node %s not found ?\n", ByName);
		exit(4);
	}

	int FindOrCreateNode(char* ByName)
	{
		for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
			if (!strcmp(ByName, Node[NodeNr].Name))
				// Found existing node with this name, stop searching, return a pointer
				return NodeNr;
		// Arriving here means that Node was not found : create a new one
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
	} /* FindOrCreateNode() */


	while (fgets(InputLine, 100, InputFile))
	{
		///////////////////////////   Get one node   ///////////////////////////
		if (!NrOfLines)
		{
			// This is the missing parts count
			int NrOfMissing;
			if (1 != sscanf(InputLine, "%d", &NrOfMissing))
			{
				fprintf(stderr, "Could not scan NrOfMissing from first line:\n%s", InputLine);
				exit(2);
			}
			NrOfLines++;
			continue;
		}
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
					printf("Determining for Node #%d : %s the TotalAmount as %d\n",
							NodeNr, thisNode->Name, thisNode->TotalAmount);
					thisNode->TotalKnown = 1;
				}
			}
		// Did this iteration complete the graph ?
		printf("NrOfUnknown is now %d\n", NrOfUnknown);
		if (!NrOfUnknown)  break;
	}

	///////////////////////////   Processing results   ///////////////////////////
	return 0;
}		
