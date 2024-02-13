#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[100];
	char* InputPtr;
	int InputPos, InputLen=0;

	if (Arguments >= 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n", Argument[1]);
			exit(2);
		}
	}
	else if (Arguments > 3)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s [InputFile [InputDigits]]\n", Argument[0]);
		fprintf(stderr, "\twith no [InputFile] argument, data will be read from stdin\n");
		fprintf(stderr, "\twith no [InputDigits] argument, highest 14-digit number will be searched.\n");
		fprintf(stderr, "\tWhen [InputDigits] starts with a minus sign, next 2 input digits will be probed\n");
		fprintf(stderr, "\tWhen [InputDigits] starts with a plus sign, all numbers of this length will be run\n");
		exit(1);
	}

	enum tInstruction {eInp, eAdd, eMul, eDiv, eMod, eEql,
				eAddLit, eMulLit, eDivLit, eModLit, eEqlLit};
	int NrOfInstructions=0, InstructionNr;
	enum tInstruction Instr[1000];
	int Oprnd[1000][2];
	int Literal[1000];
	int InpInstrNr[20] = {0};
	int InpInstrCount = 0;

	///////////////////////////    Input program     ///////////////////////////
	int NrOfLines=0;
	while (fgets(InputLine, 100, InputFile))
	{
		while ((InputLine[strlen(InputLine)-1]=='\r') || (InputLine[strlen(InputLine)-1]=='\n'))
			InputLine[strlen(InputLine)-1] = '\0';
		NrOfLines++;
		if (!strncmp(InputLine, "inp", 3))  
		{                                        Instr[NrOfInstructions] = eInp;
			InpInstrNr[InpInstrCount++] = NrOfInstructions;
		}
		else if (!strncmp(InputLine, "add", 3))  Instr[NrOfInstructions] = eAdd;
		else if (!strncmp(InputLine, "mul", 3))  Instr[NrOfInstructions] = eMul;
		else if (!strncmp(InputLine, "div", 3))  Instr[NrOfInstructions] = eDiv;
		else if (!strncmp(InputLine, "mod", 3))  Instr[NrOfInstructions] = eMod;
		else if (!strncmp(InputLine, "eql", 3))  Instr[NrOfInstructions] = eEql;
		else
		{
			fprintf(stderr, "Unrecognized instruction on InputLine #%d\n%s\n", NrOfLines, InputLine);
			exit(3);
		}
		if (InputLine[3] != ' ')
		{
			fprintf(stderr, "Expected space at InputLine[3] #%d, got %c\n%s\n", NrOfLines, InputLine[3], InputLine);
			exit(3);
		}
		if ((InputLine[4] < 'w') || (InputLine[4] > 'z'))
		{
			fprintf(stderr, "Expected a variable at InputLine[4] #%d, got %c\n%s\n", NrOfLines, InputLine[4], InputLine);
			exit(3);
		}
		Oprnd[NrOfInstructions][0] = InputLine[4] - 'w';
		if (Instr[NrOfInstructions] != eInp)
		{
		if (InputLine[5] != ' ')
		{
			fprintf(stderr, "Expected space at InputLine[5] #%d, got %c\n%s\n", NrOfLines, InputLine[5], InputLine);
			exit(3);
		}
		if ((InputLine[6] >= 'w') && (InputLine[6] <= 'z'))
			Oprnd[NrOfInstructions][1] = InputLine[6] - 'w';
		else
		{
			if (1 != sscanf(InputLine + 6, "%d", &(Literal[NrOfInstructions])))
			{
				fprintf(stderr, "Could not scan literal at InputLine[6] #%d\n%s\n", NrOfLines, InputLine);
				exit(3);
			}
			switch (Instr[NrOfInstructions])
			{
				case eAdd: Instr[NrOfInstructions] = eAddLit;  break;
				case eMul: Instr[NrOfInstructions] = eMulLit;  break;
				case eDiv: Instr[NrOfInstructions] = eDivLit;  break;
				case eMod: Instr[NrOfInstructions] = eModLit;  break;
				case eEql: Instr[NrOfInstructions] = eEqlLit;  break;
			} /* switch (Instr[NrOfInstructions]) */
		} /* when Literal 2nd operand */
		}
		NrOfInstructions++;
	} /* while (fgets) */
	printf("Obtained %d instructions for Program, with %d input instructions\n", NrOfInstructions, InpInstrCount);
	for (; InpInstrCount<20; InpInstrCount++)
		InpInstrNr[InpInstrCount] = NrOfInstructions;

	///////////////////////////    Execute program     ///////////////////////////
	int Input[20];
	int InputNr, NrOfInputs;
	long long Var[4];

	void RunInstructions(int EndInputNr)
	{
		int StopExecution = 0;
		for (; InstructionNr<NrOfInstructions; InstructionNr++)
		{
			switch (Instr[InstructionNr])
				{
					case eInp:
						if (InputNr >= EndInputNr)
							StopExecution = 1;
						else
							Var[Oprnd[InstructionNr][0]] = Input[InputNr++];
						break;
					case eAdd: Var[Oprnd[InstructionNr][0]] += Var[Oprnd[InstructionNr][1]];  break;
					case eMul: Var[Oprnd[InstructionNr][0]] *= Var[Oprnd[InstructionNr][1]];  break;
					case eDiv: Var[Oprnd[InstructionNr][0]] /= Var[Oprnd[InstructionNr][1]];  break;
					case eMod: Var[Oprnd[InstructionNr][0]] %= Var[Oprnd[InstructionNr][1]];  break;
					case eEql:
						if (Var[Oprnd[InstructionNr][0]] == Var[Oprnd[InstructionNr][1]])
							Var[Oprnd[InstructionNr][0]] = 1;
						else
							Var[Oprnd[InstructionNr][0]] = 0;
						break;
					case eAddLit: Var[Oprnd[InstructionNr][0]] += Literal[InstructionNr];  break;
					case eMulLit: Var[Oprnd[InstructionNr][0]] *= Literal[InstructionNr];  break;
					case eDivLit: Var[Oprnd[InstructionNr][0]] /= Literal[InstructionNr];  break;
					case eModLit: Var[Oprnd[InstructionNr][0]] %= Literal[InstructionNr];  break;
					case eEqlLit:
						if (Var[Oprnd[InstructionNr][0]] == Literal[InstructionNr])
							Var[Oprnd[InstructionNr][0]] = 1;
						else
							Var[Oprnd[InstructionNr][0]] = 0;
						break;
				} /* switch(Instr[InstructionNr]) */
			if (StopExecution)
			{
				//printf("Halted at instruction #%d: no digit #%d available as input\n", InstructionNr, EndInputNr);
				break;
			}
		} /* for (InstructionNr) */
	} /* RunInstructions(EndInputNr) */

	void RunProgram(int EndInputNr)
	{
		memset(Var, 0, 4 * sizeof(long long));
		InputNr = 0;
		InstructionNr = 0;
		RunInstructions(EndInputNr);
	} /* RunProgram(EndInputNr) */

	// Need to set the Var[] array BEFORE invoking this call
	void ProbeProgram(int StartInputNr, int EndInputNr)
	{
		InputNr = StartInputNr;
		InstructionNr = InpInstrNr[StartInputNr];
		RunInstructions(EndInputNr);
	} /* ProbeProgram(StartInputNr, EndInputNr) */

	///////////////////////////    Handle InputDigits (if supplied)     ///////////////////////////
	if ((Arguments == 3) && (Argument[2][0]>='0') && (Argument[2][0]<='9'))
	{
		for (NrOfInputs=0; NrOfInputs<strlen(Argument[2]); NrOfInputs++)
			Input[NrOfInputs] = Argument[2][NrOfInputs] - '0';
		printf("Scanned %d Input digits\n", NrOfInputs);
		RunProgram(NrOfInputs);
		for (int v=0; v<4; v++)
			printf("Var[%d]=%lld   ", v, Var[v]);
		putchar('\n');
		return 0;
	} /* if (Arguments == 3) */

	///////////////////////////    Probe (auto-generated digits)     ///////////////////////////
#if 0
	void SetupProbe(int DigitNr)
	{
		switch (DigitNr)
		{
			case 0:	// Probe digits 1,2
				memset(Var, 0, 4 * sizeof(long long));                 break;
			case 1:	// Probe digits 2,3
				Var[0] = 5;  Var[1] = 1;  Var[2] = 12;  Var[3] = 12;   break;
			case 2:	// Probe digits 3,4
				Var[0] = 5;  Var[1] = 1;  Var[2] = 13;  Var[3] = 325;  break;
			case 3:	// Probe digits 4,5
				Var[0] = 5;  Var[1] = 1;  Var[2] = 7;  Var[3] = 8457;  break;
			case 4:	// Probe digits 5,6
				Var[0] = 5;  Var[1] = 1;  Var[2] = 16;  Var[3] = 219898;  break;
			case 5:	// Probe digits 6,7
				Var[0] = 5;  Var[1] = 1;  Var[2] = 11;  Var[3] = 219893;  break;
			case 6:	// Probe digits 7,8
				Var[0] = 5;  Var[1] = 1;  Var[2] = 17;  Var[3] = 5717235;  break;
			case 7:	// Probe digits 8,9
				Var[0] = 5;  Var[1] = 1;  Var[2] = 19;  Var[3] = 148648129;  break;
			case 8:	// Probe digits 9,10
				Var[0] = 3;  Var[1] = 0;  Var[2] = 0;  Var[3] = 5717235;  break;
			case 9:	// Probe digits 10,11
				Var[0] = 5;  Var[1] = 1;  Var[2] = 20;  Var[3] = 148648130;  break;
			case 10:	// Probe digits 11,12
				Var[0] = 5;  Var[1] = 1;  Var[2] = 15;  Var[3] = 148648125;  break;
			case 11:	// Probe digits 12,13
				Var[0] = 3;  Var[1] = 0;  Var[2] = 0;  Var[3] = 5717235;  break;
			case 12:	// Probe digits 13,14
				Var[0] = 5;  Var[1] = 1;  Var[2] = 15;  Var[3] = 5717233;  break;
		} /* switch (DigitNr) */
	} /* SetupProbe(DigitNr) */
#endif

#if 1
	void SetupProbe(int DigitNr)
	{
		if (!DigitNr)
		{	memset(Var, 0, 4 * sizeof(long long));  return; }
		int theDigit = ((19 - DigitNr) % 9) + 1;
		Var[0] = theDigit;
		Var[1] = 1;
		Var[3] = 5149940969;
		switch (DigitNr)
		{
			case  1:  Var[2] = theDigit +  7;  break;
			case  2:  Var[2] = theDigit +  8;  break;
			case  3:  Var[2] = theDigit +  2;  break;
			case  4:  Var[2] = theDigit + 11;  break;
			case  5:  Var[2] = theDigit +  6;  break;
			case  6:  Var[2] = theDigit + 12;  break;
			case  7:  Var[2] = theDigit + 14;  break;
			case  8:  Var[2] = theDigit + 13;  break;
			case  9:  Var[2] = theDigit + 15;  break;
			case 10:  Var[2] = theDigit + 10;  break;
			case 11:  Var[2] = theDigit +  6;  break;
			case 12:  Var[2] = theDigit + 10;  break;
		} /* switch (DigitNr) */
	} /* SetupProbe(DigitNr) */
#endif

	// Analysis results :
	// After Digit 1 :   w=Digit(1)  x=1  y=Digit(1)+7  z=Digit(1)+7
	// After Digit 2 :   w=Digit(2)  x=1  y=Digit(1)+8  z=Digit(1)*26+Digit(2)+190
	// After Digit 3 :   w=Digit(3)  x=1  y=Digit(3)+2  z=Digit(1)*26*26+Digit(2)*26+Digit(3)+4942
	// After Digit 4 :   w=Digit(4)  x=1  y=Digit(4)+11 z=Digit(1)*26*26*26+Digit(2)*26*26+Digit(3)*26+Digit(4)+128503
	// After Digit 5 :   w=Digit(5)  x=1  y=Digit(5)+6  z=Digit(1)*26*26*26+Digit(2)*26*26+Digit(3)*26+Digit(5)+128498
	// After Digit 6 :   w=Digit(6)  x=1  y=Digit(6)+12
	//                   z=Digit(1)*26^4+Digit(2)*26^3+Digit(3)*26^2+Digit(5)*26+Digit(6)+3340960
	// After Digit 7 :   w=Digit(7)  x=1  y=Digit(7)+14
	//                   z=Digit(1)*26^5+Digit(2)*26^4+Digit(3)*26^3+Digit(5)*26^2+Digit(6)*26+Digit(7)+86864974
	// After Digit 8 :   *** Dependent on whether Digit(8)==Digit(7)-2  ***
	//      * if yes :   w=Digit(8)  x=0  y=0
	//                   z=Digit(1)*26^4+Digit(2)*26^3+Digit(3)*26^2+Digit(5)*26+Digit(6)+3340960
	//      * if not :   w=Digit(8)  x=1  y=Digit(8)+13
	//                   z=Digit(1)*26^5+Digit(2)*26^4+Digit(3)*26^3+Digit(5)*26^2+Digit(6)*26+Digit(8)+86864973
	// After Digit 9 :   *** Dependent on whether Digit(8)==Digit(7)-2  ***
	//                   w=Digit(9)  x=1  y=Digit(9)+15
	//      * if yes :   z=Digit(1)*26^5+Digit(2)*26^4+Digit(3)*26^3+Digit(5)*26^2+Digit(6)*26+Digit(9)+86864975
	//      * if not :   z=Digit(1)*26^6+Digit(2)*26^5+Digit(3)*26^4+Digit(5)*26^3+Digit(6)*26^2+Digit(8)*26
	//                    +Digit(9)+2258489313
	// After Digit 10:   --- Dependent on whether Digit(10)==Digit(9)+7  ---
	//      - if yes :   w=Digit(10)  x=0  y=0
	//                   z=Digit(1)*26^4+Digit(2)*26^3+Digit(3)*26^2+Digit(5)*26+Digit(6)+3340960
	//      - if not :   w=Digit(10)  x=1  y=Digit(10)+10
	//                   z=Digit(1)*26^5+Digit(2)*26^4+Digit(3)*26^3+Digit(5)*26^2+Digit(6)*26+Digit(10)+86864970
	// After Digit 11:   *** Dependent on whether Digit(11)==Digit(10)-2  ***
	//      * if yes :   w=Digit(11)  x=0  y=0
	//                   z=Digit(1)*26^4+Digit(2)*26^3+Digit(3)*26^2+Digit(5)*26+Digit(6)+3340960
	//      * if not :   w=Digit(11)  x=1  y=Digit(11)+6
	//                   z=Digit(1)*26^5+Digit(2)*26^4+Digit(3)*26^3+Digit(5)*26^2+Digit(6)*26+Digit(11)+86864966
	// After Digit 12:   +++ Dependent on whether Digit(12)==Digit(11)-1  +++
	//      + if yes :   w=Digit(12)  x=0  y=0
	//                   z=Digit(1)*26^4+Digit(2)*26^3+Digit(3)*26^2+Digit(5)*26+Digit(6)+3340960
	//      + if not :   w=Digit(12)  x=1  y=Digit(12)+10
	//                   z=Digit(1)*26^5+Digit(2)*26^4+Digit(3)*26^3+Digit(5)*26^2+Digit(6)*26+Digit(12)+86864970
	// After Digit 13:   /// Dependent on whether Digit(13)==Digit(12)+4  ///
	//      / if yes :   w=Digit(13)  x=0  y=0
	//                   z==Digit(1)*26*26*26+Digit(2)*26*26+Digit(3)*26+Digit(5)+128498
	//      / if not :   w=Digit(13)  x=1  y=Digit(12)+8
	//                   z=Digit(1)*26^4+Digit(2)*26^3+Digit(3)*26^2+Digit(5)*26+Digit(13)+3340956

	if ((Arguments == 3) && (Argument[2][0] == '-'))
	{
		int DigitNr = 0;  // 0 for first digit, 12 for thirteenth - always uses the next digit too
		if (1 != sscanf(Argument[2]+1, "%d", &DigitNr))
		{
			fprintf(stderr, "Could not scan DigitNr behind minus sign in Argument[2]=%s\n", Argument[2]);
			DigitNr = 0;
		}
		printf("Probing effects of input digits #%d and #%d\n",	DigitNr+1, DigitNr+2);
		SetupProbe(DigitNr);
		printf("Start condition: ");
		for (int v=0; v<4; v++)
			printf("Var[%d]=%lld   ", v, Var[v]);
		putchar('\n');

		for (Input[DigitNr]=1; Input[DigitNr]<=9; Input[DigitNr]++)
		{
			SetupProbe(DigitNr);
			printf("%d   |  ", Input[DigitNr]);
			ProbeProgram(DigitNr, DigitNr+1);
			for (int v=0; v<4; v++)
				printf("Var[%d]=%lld   ", v, Var[v]);
			putchar('\n');
		}
		printf("\n= = = = = = = = = = = = =\n\n");
		for (Input[DigitNr+1]=1; Input[DigitNr+1]<=9; Input[DigitNr+1]++)
		{
			for (Input[DigitNr]=1; Input[DigitNr]<=9; Input[DigitNr]++)
			{
				SetupProbe(DigitNr);
				printf("%d%d  |  ", Input[DigitNr], Input[DigitNr+1]);
				ProbeProgram(DigitNr, DigitNr+2);
				for (int v=0; v<4; v++)
					printf("Var[%d]=%lld   ", v, Var[v]);
				putchar('\n');
			}
			printf("- - - - - - - - - - -\n");
		}
		return 0;
	} /* if (minus sign) */

	///////////////////////////    Examine all numbers of given length     ///////////////////////////
	int Decrement(int NrOfDigits)
	{
		int digit;
		// Decrement, starting from the least-significant digit
		for (digit=NrOfDigits; digit; digit--)
			if (--Input[digit-1])
				break;
			else
				Input[digit-1] = 9;
		// Halt when all numbers have been exhausted
		if (!digit)  return 0;  else return 1;
	} /* Decrement(NrOfDigits) */

	if ((Arguments == 3) && (Argument[2][0] == '+'))
	{
		int DigitNr = 0;
		if (1 != sscanf(Argument[2]+1, "%d", &DigitNr))
		{
			fprintf(stderr, "Could not scan DigitNr behind plus sign in Argument[2]=%s\n", Argument[2]);
			exit(4);
		}
		printf("Running all numbers of length %d\n\n",	DigitNr);

		int digit;
		for (digit=0; digit<DigitNr; digit++)  Input[digit] = 9;  // Start out with max number
		long long PredictZ;

		for (;;)
		{
			RunProgram(DigitNr);

			// Try to predict what the outcome should be
			PredictZ = 0;
			for (digit=0; digit<DigitNr; digit++)
				switch (digit)
				{
					case  0:  PredictZ = 26*PredictZ + Input[digit] +  7;  break;
					case  1:  PredictZ = 26*PredictZ + Input[digit] +  8;  break;
					case  2:  PredictZ = 26*PredictZ + Input[digit] +  2;  break;
					case  3:  PredictZ = 26*PredictZ + Input[digit] + 11;  break;
					case  4:
#if 1
						        // Relationship between NewDigit and Z
									if (Input[digit] == (PredictZ%26) -  3)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] +  6;
#else
									  // The 5th digit makes the 4th digit disappear
						        PredictZ =  PredictZ - Input[digit-1] - 11;
									if (Input[digit] == Input[digit-1] + 8)
										PredictZ /= 26;
									else
						        PredictZ =    PredictZ + Input[digit] +  6;
#endif
										break;
					case  5:  PredictZ = 26*PredictZ + Input[digit] + 12;  break;
					case  6:  PredictZ = 26*PredictZ + Input[digit] + 14;  break;
					case  7:
#if 1
						        // Relationship between NewDigit and Z
									if (Input[digit] == (PredictZ%26) - 16)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] + 13;
#else
									  // The 8th digit makes the 7th digit disappear
									  PredictZ =  PredictZ - Input[digit-1] - 14;
									if (Input[digit] == Input[digit-1] - 2)
										PredictZ /= 26;
									else
						        PredictZ =    PredictZ + Input[digit] + 13;
#endif
										break;
					case  8:  PredictZ = 26*PredictZ + Input[digit] + 15;  break;
					case  9:
#if 1
						        // Relationship between NewDigit and Z
									if (Input[digit] == (PredictZ%26) -  8)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] + 10;
#else
									  // The 10th digit makes the 9th digit disappear
						        PredictZ =  PredictZ - Input[digit-1] - 15;
									if (Input[digit] == Input[digit-1] + 7)
										PredictZ /= 26;
									else
						        PredictZ =    PredictZ + Input[digit] + 10;
#endif
										break;
					case 10:  // Special dependence on Z%26-12
						      if (Input[digit] == (PredictZ%26) - 12)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] + 6;
										break;
					case 11:if (Input[digit] == (PredictZ%26) -  7)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] + 10;
										break;
					case 12:if (Input[digit] == (PredictZ%26) -  6)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] + 8;
										break;
					case 13:if (Input[digit] == (PredictZ%26) - 11)
										PredictZ /= 26;
									else
										PredictZ = 26*(PredictZ/26) + Input[digit] + 5;
										break;
				} /* switch (digit) */

			// Was this outcome expected ?
			if (Var[3] != PredictZ)
			{
				printf("Anomaly: input ");
				for (digit=0; digit<DigitNr; digit++)  putchar('0' + Input[digit]);
				printf(" should make z=%lld but found %lld instead (discrepancy %lld)\n",
						PredictZ, Var[3], Var[3] - PredictZ);
				//break; // From forever loop
			}

			if (!Decrement(DigitNr))  break;
		} /* for (;;) */
		return 0;
	} /* if (plus sign) */

	///////////////////////////    Optimize     ///////////////////////////
	int digit;
	for (digit=0; digit<8; digit++)  Input[digit] = 9;
	Input[ 8] = 1;
	Input[ 9] = 8;
	Input[10] = 6;
	Input[11] = 5;
	Input[12] = 9;
	Input[13] = 6;
	for (;;)
	{
		//for (digit=0; digit<14; digit++)  putchar('0' + Input[digit]);
		RunProgram(14);
		//printf("   ->  z=%lld\n", Var[3]);
		if (!Var[3])  break;

		// Nonzero result in var Z means that we need to find a lower 14-digit number
		if (!Decrement(8))  break;
	}
	return 0;
}		
