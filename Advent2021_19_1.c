#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Variations.h"

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

	int NrOfScanners=0, ScannerNr;
	int ScanNrOfData[100]={0}, ScanDataNr;
	int ScanData[100][100][3];
	int AxisChoice[100][3];
	int AxisSign[100][3];
	int Offset[100][3];
	int ScannerAligned[100]={1};
	int ScannerTreated[100]={0};

	int NrOfBeacons=0, BeaconNr;
	int BeaconData[1000][3];

	int Dim;

	///////////////////////////    Input data     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 100, InputFile))
	{
		NrOfLines++;
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!strlen(InputLine))  continue;
		if (!strncmp(InputLine, "--- scanner ", 12))
		{
			if (1 != sscanf(InputLine + 12, "%d", &ScannerNr))
			{
				fprintf(stderr, "Cannot find ScannerNr on InputLine #%d:\n%s", NrOfLines, InputLine);
				exit(2);
			}
			if (ScannerNr >= NrOfScanners)
				NrOfScanners = ScannerNr + 1;
			continue;
		}
		if (3 != sscanf(InputLine, "%d,%d,%d",
						&(ScanData[ScannerNr][ScanNrOfData[ScannerNr]][0]),
						&(ScanData[ScannerNr][ScanNrOfData[ScannerNr]][1]),
						&(ScanData[ScannerNr][ScanNrOfData[ScannerNr]][2])))
		{
			fprintf(stderr, "Cannot scan 3 coordinates on InputLine #%d:\n%s", NrOfLines, InputLine);
			exit(2);
		}
		ScanNrOfData[ScannerNr]++;
	} /* while (fgets) */
	printf("Parsed %d Lines, found %d Scanners\n", NrOfLines, NrOfScanners);

	///////////////////////////    Scanner 0 data is sacred     ///////////////////////////
	for (Dim=0; Dim<3; Dim++) {	AxisChoice[0][Dim] = Dim;  AxisSign[0][Dim] = 1;  Offset[0][Dim] = 0; }
	for (NrOfBeacons=0; NrOfBeacons<ScanNrOfData[0]; NrOfBeacons++)
		memcpy(BeaconData[NrOfBeacons], ScanData[0][NrOfBeacons], 3*sizeof(int));

	//////////////////////    Pick an Aligned Scanner to Treat     ///////////////////////
	int Fixed,Loose;
	for (;;)
	{
		for (Fixed=0; Fixed<NrOfScanners; Fixed++)
			if ((ScannerAligned[Fixed]) && (!ScannerTreated[Fixed]))
				break;
		if (Fixed >= NrOfScanners)  break;  // From outer loop

		// So we're going ahead with this Aligned Scanner
		int FixedPermutation[3];
		int FixedPolarity[3];
		int FixedDiff[3];
		memcpy(FixedPermutation, AxisChoice[Fixed], 3*sizeof(int));
		memcpy(FixedPolarity, AxisSign[Fixed], 3*sizeof(int));
		memcpy(FixedDiff, Offset[Fixed], 3*sizeof(int));
printf("++++++ Fixed Scanner %d with Permute[%d,%d,%d] - Sign[%d,%d,%d] - Offset[%d,%d,%d]\n", Fixed,
										FixedPermutation[0], FixedPermutation[1], FixedPermutation[2],
										FixedPolarity[0], FixedPolarity[1], FixedPolarity[2],
										FixedDiff[0], FixedDiff[1], FixedDiff[2]);

		// Try to match this Aligned Scanner with ALL the remaining non-Aligned Scanners
		for (Loose=0; Loose<NrOfScanners; Loose++)
		{
			if (ScannerAligned[Loose])  continue;

	//////////////////////    Match the Loose Scanner with the Aligned     ///////////////////////
			int Matched = 0;
			// This can be done by any permutation of X,Y,Z axis and by any choice of signs
			int LoosePermutation[3];
			int LoosePolarity[3];
			int LooseDiff[3];
			InitPermutation(LoosePermutation, 3);
			do {
				/* Start with all signs chosen positive */
				for (Dim=0; Dim<3; Dim++)  LoosePolarity[Dim] = 1;
				for(;;)  // all signs
				{
	///////////////////////////    Examine this permutations+signs setup     ///////////////////////////
printf("------ Loose Scanner %d with Permute[%d,%d,%d] - Sign[%d,%d,%d]\n", Loose,
										LoosePermutation[0], LoosePermutation[1], LoosePermutation[2],
										LoosePolarity[0], LoosePolarity[1], LoosePolarity[2]);

					// All that remains to do, is to pick a diff - it follows from picking any pair of points
					int FixedScanNr,LooseScanNr;
					int FixedProjected[3], LooseProjected[3];
					for (FixedScanNr=0; FixedScanNr<ScanNrOfData[Fixed]-11; FixedScanNr++)
					{
						for (Dim=0; Dim<3; Dim++)
							FixedProjected[Dim] =
								ScanData[Fixed][FixedScanNr][FixedPermutation[Dim]] * FixedPolarity[Dim] + FixedDiff[Dim];
#if 0
printf("  ++   Anchor point Fixed #%d [%d,%d,%d] projects to [%d,%d,%d]\n",
								FixedScanNr, ScanData[Fixed][FixedScanNr][0], ScanData[Fixed][FixedScanNr][1],
								ScanData[Fixed][FixedScanNr][2], FixedProjected[0], FixedProjected[1], FixedProjected[2]);
#endif
						for (LooseScanNr=0; LooseScanNr<ScanNrOfData[Loose]; LooseScanNr++)
						{
							for (Dim=0; Dim<3; Dim++)
							{
								LooseProjected[Dim] =
									ScanData[Loose][LooseScanNr][LoosePermutation[Dim]] * LoosePolarity[Dim];
								// Enforce a mapping by setting the diff just right
								LooseDiff[Dim] = FixedProjected[Dim] - LooseProjected[Dim];
							}
#if 0
printf("  --   Anchor point Loose #%d [%d,%d,%d] projects to [%d,%d,%d]: requires Diff [%d,%d,%d]\n",
								LooseScanNr, ScanData[Loose][LooseScanNr][0], ScanData[Loose][LooseScanNr][1],
										ScanData[Loose][LooseScanNr][2], LooseProjected[0], LooseProjected[1], LooseProjected[2],
										LooseDiff[0], LooseDiff[1], LooseDiff[2]);
#endif

	///////////////////////////    For this permutations+signs+diff setup     ///////////////////////////
							int FixedPoint, LoosePoint;
							int FixedProj[3], LooseProj[3];
							int NrOfMatches = 0;
							for (FixedPoint=0; FixedPoint<ScanNrOfData[Fixed]; FixedPoint++)
							{
								for (Dim=0; Dim<3; Dim++)
									FixedProj[Dim] =
										ScanData[Fixed][FixedPoint][FixedPermutation[Dim]] * FixedPolarity[Dim] + FixedDiff[Dim];
#if 0
printf(" +  +  Matching Fixed #%d [%d,%d,%d] projects to [%d,%d,%d]\n", FixedPoint,
										ScanData[Fixed][FixedPoint][0], ScanData[Fixed][FixedPoint][1], ScanData[Fixed][FixedPoint][2],
										FixedProj[0], FixedProj[1], FixedProj[2]);
#endif
								for (LoosePoint=0; LoosePoint<ScanNrOfData[Loose]; LoosePoint++)
								{
									for (Dim=0; Dim<3; Dim++)
									{
										LooseProj[Dim] =
											ScanData[Loose][LoosePoint][LoosePermutation[Dim]] * LoosePolarity[Dim] + LooseDiff[Dim];
										if (LooseProj[Dim] != FixedProj[Dim])  break;
									}
#if 0
printf(" -  -  Matching Loose #%d [%d,%d,%d] projects to [%d,%d,%d] SCORE %d\n", LoosePoint,
										ScanData[Loose][LoosePoint][0], ScanData[Loose][LoosePoint][1], ScanData[Loose][LoosePoint][2],
										LooseProj[0], LooseProj[1], LooseProj[2], Dim);
#endif
									if (Dim >= 3)
									{
										// Full match
										NrOfMatches++;
										break;
									}
								} /* for (LoosePoint) */
								if (NrOfMatches >= 12)
								{
									printf("====== %d matches found between scanner %d and %d\n", NrOfMatches, Fixed, Loose);
									printf(" with Permute[%d,%d,%d] - Sign[%d,%d,%d] - Offset[%d,%d,%d]\n",
											LoosePermutation[0], LoosePermutation[1], LoosePermutation[2],
											LoosePolarity[0], LoosePolarity[1], LoosePolarity[2],
											LooseDiff[0], LooseDiff[1], LooseDiff[2]);
									Matched = 1;
									break;
								}
							} /* for (FixedPoint) */
							if (Matched)  break;
						} /* for (LooseScanNr) */
						if (Matched)  break;
					} /* for (FixedScanNr) */

					if (Matched)  break;

					// Go to the next sign setting
					for (Dim=2; Dim>=0; Dim--)
						if (LoosePolarity[Dim] == 1)
						{
							LoosePolarity[Dim] = -1;
							break;
						}
					if (Dim < 0)  break;  // all signs done
					for (Dim++; Dim<3; Dim++)
						LoosePolarity[Dim] = 1;
				} /* for(;;) - all signs */
			} while ((!Matched) && (NextPermutation(LoosePermutation, 3)));

	//////////////////////    When Matched, register the Loose Beacons      ///////////////////////
			if (Matched)
			{
				for (ScanDataNr=0; ScanDataNr<ScanNrOfData[Loose]; ScanDataNr++)
				{
					// Can this Loose Beacon be projected onto any existing Beacon ?
					int Projected[3];
					for (Dim=0; Dim<3; Dim++)
						Projected[Dim] = ScanData[Loose][ScanDataNr][LoosePermutation[Dim]] * LoosePolarity[Dim] + LooseDiff[Dim];
					for (BeaconNr=0; BeaconNr<NrOfBeacons; BeaconNr++)
						if ((BeaconData[BeaconNr][0] == Projected[0]) &&
								(BeaconData[BeaconNr][1] == Projected[1]) &&
								(BeaconData[BeaconNr][2] == Projected[2]))
							break;
					// When not found, we have to register a new Beacon
					if (BeaconNr >= NrOfBeacons)
						memcpy(BeaconData[NrOfBeacons++], Projected, 3*sizeof(int));
				} /* for (ScanDataNr) */

				// Register this Scanner as Aligned now
				memcpy(AxisChoice[Loose], LoosePermutation, 3*sizeof(int));
				memcpy(AxisSign[Loose], LoosePolarity, 3*sizeof(int));
				memcpy(Offset[Loose], LooseDiff, 3*sizeof(int));
				ScannerAligned[Loose] = 1;
			}

		} /* for (Loose) */

		// Mark this Aligned Scanner as being fully Treated now
		ScannerTreated[Fixed] = 1;
	} /* for(;;) */

	printf("Concluded with %d Beacons :\n", NrOfBeacons);
	for (BeaconNr=0; BeaconNr<NrOfBeacons; BeaconNr++)
		printf("%5d,%5d,%5d\n", BeaconData[BeaconNr][0], BeaconData[BeaconNr][1], BeaconData[BeaconNr][2]);

	return 0;
}		
