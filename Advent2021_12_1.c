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

	typedef struct {
		char Name[10];
		int SingleUse;
		int Used;
		int NrOfNeighbors;
		int Neighbor[10];
	} tNode;
	tNode Node[100];
	memset(Node, 0, 100 * sizeof(tNode));
	int NrOfNodes=0, NodeNr;

	int StartNode=-1, EndNode=-1;

	///////////////////////////    Input the graph     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 100, InputFile))
	{
		int NrOfStrings=0;
		int FromNode=-1, ToNode=-1;
		while (InputPtr = strtok(NrOfStrings?NULL:InputLine, "-\n"))
		{
			if (NrOfStrings >= 2)
			{
				fprintf(stderr, "Can't connect more than 2 strings on InputLine #%d\n%s", NrOfLines, InputLine);
				exit(3);
			}
			for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
				if (!strcmp(Node[NodeNr].Name, InputPtr))  break;
			if (NodeNr == NrOfNodes)
			{
				// NodeName not previously listed, create it now
				strcpy(Node[NodeNr].Name, InputPtr);
				if ((*InputPtr >= 'a') && (*InputPtr <= 'z'))
					Node[NodeNr].SingleUse = 1;
printf("Created Node[%d]=%s%s\n", NodeNr, Node[NodeNr].Name, (Node[NodeNr].SingleUse)?" SingleUse":"");
				if (!strcmp(InputPtr, "start"))
				{
					if (StartNode == -1)
						StartNode = NodeNr;
					else
					{
						fprintf(stderr, "Already seen start as Node #%d, now it's found as #%d ?\n", StartNode, NodeNr);
						exit(4);
					}
				}
				else if (!strcmp(InputPtr, "end"))
				{
					if (EndNode == -1)
						EndNode = NodeNr;
					else
					{
						fprintf(stderr, "Already seen end as Node #%d, now it's found as #%d ?\n", EndNode, NodeNr);
						exit(4);
					}
				}
				NrOfNodes++;
			} /* if (new node needed) */
			// Remember the Node that was just created or mentioned
			if (!NrOfStrings)
				FromNode = NodeNr;
			else if (NrOfStrings == 1)
				ToNode = NodeNr;
			NrOfStrings++;
		} /* while (strtok) */
		if (NrOfStrings != 2)
		{
			fprintf(stderr, "Supposed to connect exactly 2 Nodes on InputLine #%d\n%s", NrOfLines, InputLine);
			exit(3);
		}
		// The link is seen from both endpoints as a new Neighbor Node
		Node[FromNode].Neighbor[Node[FromNode].NrOfNeighbors++] = ToNode;
		Node[ToNode].Neighbor[Node[ToNode].NrOfNeighbors++] = FromNode;
		NrOfLines++;
	} /* while (fgets) */
	printf("%d lines scanned - NrOfNodes is %d - Start %d End %d\n", NrOfLines, NrOfNodes, StartNode, EndNode);

	///////////////////////////    COnstruct all Paths     ///////////////////////////
	int NrOfPaths=0;
	int NrOfSteps=0, StepNr;
	int Step[100]={-1};
	int NodeAtStep[100];

	NodeNr = StartNode;
	NodeAtStep[NrOfSteps] = StartNode;
	// The Start is lowercase, so SingleUse - mark it as Used because we always begin there
	Node[StartNode].Used = 1;

	// Forever keep trying to add one more Step to the current Path
	for (;;)
	{
		// From the current NodeNr, is it possible to pick a further Choice ?
		int ChoiceNr = Step[NrOfSteps];
		int NrOfChoices = Node[NodeNr].NrOfNeighbors;
printf("Iteration starts Step[%d] from Node[%d]=%s beyond Choice %d out of %d\n",
		NrOfSteps, NodeNr, Node[NodeNr].Name, ChoiceNr, NrOfChoices);
		int DestNode;
		for (ChoiceNr++; ChoiceNr<NrOfChoices; ChoiceNr++)
		{
			DestNode = Node[NodeNr].Neighbor[ChoiceNr];
			if ((Node[DestNode].SingleUse) && (Node[DestNode].Used))
				// Can't visit a 'small' node more than once, skip this ChoiceNr
				continue;
			// That's the only obstacle : so we found a new viable Step
			break;
		}
		if (ChoiceNr >= NrOfChoices)
		{
			// At the current Node, we have exhausted all Choices to make another Step.
			// Instead, we need to undo the last taken Step, and reconsider from there.
			if (!NrOfSteps)
				// Cannot backtrack any further : the search ends
				break;
			// Backtrack : break up the last taken Step
			// This might clear the Used status of a SingleUse Node
			if (Node[NodeNr].SingleUse)  Node[NodeNr].Used = 0;
			NrOfSteps--;
			NodeNr = NodeAtStep[NrOfSteps];
			continue;
		}
		// Getting here means that a viable new Step was found to extend the Path
		NodeAtStep[NrOfSteps] = NodeNr;
		Step[NrOfSteps++] = ChoiceNr;
		NodeNr = DestNode;
		if (Node[DestNode].SingleUse)
			Node[DestNode].Used = 1;
		// Not any choice has been made from this DestNode
		Step[NrOfSteps] = -1;
		// If we have arrived at the End, register this, and extend no further
		if (DestNode == EndNode)
		{
			NrOfPaths++;
			// Ensure backtracking instead of extending the Path
			Step[NrOfSteps] = Node[EndNode].NrOfNeighbors;
		}
	}

	printf("Found %d paths\n", NrOfPaths);
	return 0;
}		
