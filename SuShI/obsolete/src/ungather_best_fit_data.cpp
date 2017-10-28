#include <best_fit_data.h>
#include <stdio.h>
#include <glob.h>
#include <vector>

int fget_delim(FILE * i_file, const char * i_lpszDelimiters, char * o_lpszBuffer, char o_cDelimiter, bool i_bIgnore_EOL, bool i_bStop_At_EOL, bool &o_bEOF, bool & o_bEOL)
{
	unsigned int uiCount = 0;
	o_lpszBuffer[0] = 0;
	if (i_file)
	{
		const char * lpcDelimiter = NULL;
		o_bEOL = false;
		do
		{
			char chChar = fgetc(i_file);
			lpcDelimiter = strchr(i_lpszDelimiters,chChar);
			o_bEOL |= (chChar == 10 || chChar == 13);
			if (chChar != EOF && (!o_bEOL || !i_bIgnore_EOL) && lpcDelimiter == NULL)
			{
				o_lpszBuffer[uiCount] = chChar;
				uiCount++;
				o_lpszBuffer[uiCount] = 0;
			}
			o_bEOF = chChar == EOF;
		} while (lpcDelimiter == NULL && (!o_bEOL || !i_bStop_At_EOL) && !o_bEOF);
		if (lpcDelimiter)
			o_cDelimiter = lpcDelimiter[0];
		else
			o_cDelimiter = 0;
		o_lpszBuffer[uiCount] = 0;
	}
	return uiCount;
}

int fpass_eol(FILE * i_file, bool &o_bEOF)
{
	int iRet = 0;
	if (i_file)
	{
		char chChar;
		do
		{
			chChar = fgetc(i_file);
			iRet++;
		} while (chChar != EOF && (chChar == 10 || chChar == 13));
		if (chChar != EOF)
		{
			iRet--;
			fseek(i_file,-1,SEEK_CUR);
		}

		o_bEOF = (chChar == EOF);
	}
	return iRet;
}
int fpass_whitespace(FILE * i_file, bool &o_bEOF)
{
	int iRet = 0;
	if (i_file)
	{
		char chChar;
		do
		{
			chChar = fgetc(i_file);
			iRet++;
		} while (chChar != EOF && (chChar == ' ' || chChar == '\t'));
		if (chChar != EOF)
		{
			iRet--;
			fseek(i_file,-1,SEEK_CUR);
		}

		o_bEOF = (chChar == EOF);
	}
	return iRet;
}

void ReadString(FILE * fileIn, char * o_lpszBuffer, bool &o_bEOF, bool & o_bEOL)
{
	char cDelimeter;
	fpass_whitespace(fileIn,o_bEOF);
	fget_delim(fileIn,",",o_lpszBuffer,cDelimeter,false,true,o_bEOF, o_bEOL);
	if ((cDelimeter == 10 || cDelimeter == 13 || o_bEOL) && !o_bEOF)
		fpass_eol(fileIn,o_bEOF);
}
double ReadDouble(FILE * fileIn, bool &o_bEOF, bool & o_bEOL)
{
	char cDelimeter;
	char lpszBuffer[256];
	fpass_whitespace(fileIn,o_bEOF);
	fget_delim(fileIn,",",lpszBuffer,cDelimeter,false,true,o_bEOF, o_bEOL);
	if ((cDelimeter == 10 || cDelimeter == 13 || o_bEOL) && !o_bEOF)
		fpass_eol(fileIn,o_bEOF);
	return (atof(lpszBuffer));
}
int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cFit_Data;
	bool	bEOF,bEOL;

	FILE * fileIn = fopen("fit.user.csv","rt");
	
	if (fileIn)
	{
		char lpszBuffer[1024];
		fgets(lpszBuffer,1024,fileIn); // bypass header line
		do
		{
			char	lpszModel[32];
			char lpszDate_String[32];
			char lpszFit_Region[32];
			char lpszFilename[256];

			ReadString(fileIn,lpszModel,bEOF,bEOL);
			ReadString(fileIn,lpszDate_String,bEOF,bEOL);
			ReadString(fileIn,lpszFit_Region,bEOF,bEOL);

			sprintf(lpszFilename,"*%s*run%s*databin",lpszDate_String,lpszModel);
			glob_t	cGlob;
			int	iRead = 0;
			int iErr = glob(lpszFilename, 0, NULL, &cGlob);
			if (iErr == 0 && cGlob.gl_pathc == 1)
			{
				cFit_Data.ReadBinary(cGlob.gl_pathv[0]);
				iRead = 1;
			}
			else
			{
				sprintf(lpszFilename,"*%s*run%s*data",lpszDate_String,lpszModel);
				iErr = glob(lpszFilename, 0, NULL, &cGlob);
				if (iErr == 0 && cGlob.gl_pathc == 1)
				{
					cFit_Data.ReadFromFile(cGlob.gl_pathv[0]);
					iRead = 2;
				}
			}
			if (iRead > 0)
			{
				cFit_Data.dSource_Data_MJD = ReadDouble(fileIn,bEOF,bEOL);
				cFit_Data.dBest_Fit_Day = ReadDouble(fileIn,bEOF,bEOL);
				cFit_Data.dBest_Fit_PS_Vel = ReadDouble(fileIn,bEOF,bEOL);
				cFit_Data.dBest_Fit_PS_Temp = ReadDouble(fileIn,bEOF,bEOL);
				cFit_Data.dBest_Fit_PS_Log_Tau = ReadDouble(fileIn,bEOF,bEOL);
				cFit_Data.dBest_Fit_HVF_Log_Tau = ReadDouble(fileIn,bEOF,bEOL);
				ReadDouble(fileIn,bEOF,bEOL); // ejecta abundance
				ReadDouble(fileIn,bEOF,bEOL); // ionization
				ReadDouble(fileIn,bEOF,bEOL); // shell abundance
				cFit_Data.m_dPS_Absorption_Peak_Velocity = ReadDouble(fileIn,bEOF,bEOL);
				cFit_Data.m_dHVF_Absorption_Peak_Velocity = ReadDouble(fileIn,bEOF,bEOL);

				std::vector<double> vdMoments;
				do
				{
					double dVal = ReadDouble(fileIn,bEOF,bEOL);
					if (!bEOL && !bEOF)
					{
						vdMoments.push_back(dVal);
						vdMoments.push_back(ReadDouble(fileIn,bEOF,bEOL));
						vdMoments.push_back(ReadDouble(fileIn,bEOF,bEOL));
					}
				} while (!bEOL && !bEOF);
				cFit_Data.SetNumMoments(vdMoments.size() / 3);
				for (unsigned int uiI = 0; uiI < cFit_Data.GetNumMoments(); uiI++)
				{
					cFit_Data.SetMoments(uiI,vdMoments[uiI * 3],vdMoments[uiI * 3 + 1],vdMoments[uiI * 3 + 2]);
				}
				switch(iRead)
				{
				case 1:
					printf("Writing to %s\n",cGlob.gl_pathv[0]);
					cFit_Data.OutputBinary(cGlob.gl_pathv[0]);
					cGlob.gl_pathv[0][strlen(cGlob.gl_pathv[0])-3] = 0; // cut "bin" off of filename for output of human readable.
				case 2:
					printf("Writing to %s\n",cGlob.gl_pathv[0]);
					cFit_Data.Output(cGlob.gl_pathv[0],false);
					break;
				}
			}
			else
			{
				switch(iErr)
				{
				case GLOB_ABORTED:
					fprintf(stderr,"Glob aborted resolving %s.\n",lpszFilename);
					break;
				case GLOB_NOMATCH:
					fprintf(stderr,"Unable to resolve %s.\n",lpszFilename);
					break;
				case GLOB_NOSPACE:
					fprintf(stderr,"Insufficient space resolving %s.\n",lpszFilename);
					break;
				default:
					fprintf(stderr,"Unknown error (%i) resolving %s.\n",iErr,lpszFilename);
					break;
				}
			}

	
		} while (!bEOF);
	}
}
