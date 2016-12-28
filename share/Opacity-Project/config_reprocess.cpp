#include<cstdio>
#include<cstdlib>
#include <cstring>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	if (i_iArg_Count == 3)
	{
		int iZ = atoi(i_lpszArg_Values[1]);
		int iN = atoi(i_lpszArg_Values[2]);
		char lpszFileOut[64];
		char lpszFileIn[64];
		sprintf(lpszFileIn,"cr%02i.%02i",iZ,iN);
		sprintf(lpszFileOut,"c%02i.%02i",iZ,iN);

		FILE * fileIn = fopen(lpszFileIn,"rt");
		FILE * fileOut = fopen(lpszFileOut,"wt");

		if (fileIn)
		{
			char lpszBuffer[1024];
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // <html>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // <head>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // <title>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // TOPbase: Table of Contents
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // </title>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // </head>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // <body>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // <pre>
			fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // NZ = ? NE = ?
			char * lpszCursor = lpszBuffer;
			while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
				lpszCursor++;
			unsigned int uiZ = atoi(lpszCursor);
			while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] >= '0' && lpszCursor[0] <= '9'))
				lpszCursor++;
			while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
				lpszCursor++;
			unsigned int uiN = atoi(lpszCursor);
			printf("Z = %i N = %i\n",uiZ,uiN);
			if (uiZ == iZ && uiN == iN) // confirm element and electron number
			{
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); // Target States
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); //  ===========================================
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); //    i 2S+1  L  P   Energy(Ryd)  Configuration
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); //  ===========================================
				bool bQuit = false;
				do
				{
					fgets(lpszBuffer,sizeof(lpszBuffer),fileIn);
					lpszCursor = lpszBuffer;
					while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
						lpszCursor++; // bypass whitespace
					if (lpszCursor != nullptr && lpszCursor[0] != 0 && lpszCursor[0] != '=') // we've reached the end
					{
						unsigned int uiIdx = atoi(lpszCursor);
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] >= '0' && lpszCursor[0] <= '9'))
							lpszCursor++; // bypass number
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
							lpszCursor++; // bypass whitespace
						unsigned int uiS = atoi(lpszCursor);
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] >= '0' && lpszCursor[0] <= '9'))
							lpszCursor++; // bypass number
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
							lpszCursor++; // bypass whitespace
						unsigned int uiL = atoi(lpszCursor);
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] >= '0' && lpszCursor[0] <= '9'))
							lpszCursor++; // bypass number
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
							lpszCursor++; // bypass whitespace
						unsigned int uiP = atoi(lpszCursor);
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] >= '0' && lpszCursor[0] <= '9'))
							lpszCursor++; // bypass number
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
							lpszCursor++; // bypass whitespace
						double dState_Ener = atof(lpszCursor);
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] != ' ' && lpszCursor[0] != '\t'))
							lpszCursor++; // bypass number
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
							lpszCursor++; // bypass whitespace
						char lpszState[32];
						strcpy(lpszState,lpszCursor);
						lpszCursor = lpszState + strlen(lpszState) - 1;
						while (lpszCursor[0] == 10 || lpszCursor[0] == 13 || lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						{
							lpszCursor[0] = 0;
							lpszCursor--;
						}
						fprintf(fileOut,"T    %2i    %sX  %2i  %2i  %2i  %.5e\n",uiIdx,lpszState,uiS,uiL,uiP,dState_Ener);
						printf("T    %2i    %sX  %2i  %2i  %2i  %.5f\n",uiIdx,lpszState,uiS,uiL,uiP,dState_Ener);
						
					}
					else
						bQuit = true;
				} while (!bQuit);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn); //  N+1 Configurations:
				bQuit = false;
				do
				{
					fgets(lpszBuffer,sizeof(lpszBuffer),fileIn);
					lpszCursor = lpszBuffer;
					while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
						lpszCursor++; // bypass whitespace
					if (lpszCursor != nullptr && lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					{
						unsigned int uiIdx = atoi(lpszCursor);
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] >= '0' && lpszCursor[0] <= '9'))
							lpszCursor++; // bypass number
						while (lpszCursor != nullptr && lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
							lpszCursor++; // bypass whitespace
						char lpszState[32];
						strcpy(lpszState,lpszCursor);
						lpszCursor = lpszState + strlen(lpszState) - 1;
						while (lpszCursor[0] == 10 || lpszCursor[0] == 13 || lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						{
							lpszCursor[0] = 0;
							lpszCursor--;
						}
						fprintf(fileOut,"C    %2i    %sX  --  --  --\n",uiIdx,lpszState);
						printf("C    %2i    %sX  --  --  --\n",uiIdx,lpszState);
					}
					else
						bQuit = true;
				} while (!bQuit);
			}
			fclose(fileIn);
			fclose(fileOut);
		}
	}
	return 0;
}
