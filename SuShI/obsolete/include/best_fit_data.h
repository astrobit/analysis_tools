#pragma once

#include <xstdlib.h>
#include <stdio.h>
#include <float.h>
#include "ES_Synow.hh"
#include <math.h>
#include <unistd.h>


class BEST_FIT_DATA
{
public:
	double	dBest_Fit;
	double	dSource_Data_MJD;
	double	dBest_Fit_PS_Ion_Temp;
	double	dBest_Fit_PS_Temp;
	double 	dBest_Fit_Day;
	double	dBest_Fit_PS_Log_Tau;
	double	dBest_Fit_HVF_Ion_Temp;
	double	dBest_Fit_PS_Vel;
	double	dBest_Fit_HVF_Log_Tau;
	double	dBest_Fit_PS_Ion_Vmin;
	double	dBest_Fit_PS_Ion_Vmax;
	double	dBest_Fit_PS_Ion_Aux;
	double	dBest_Fit_HVF_Ion_Vmin;
	double	dBest_Fit_HVF_Ion_Vmax;
	double	dBest_Fit_HVF_Ion_Aux;

	double	m_dPS_Absorption_Peak_Velocity;
	double	m_dHVF_Absorption_Peak_Velocity;

	double	m_dPVF_Pseudo_Equivalent_Width; // @ADDED v1.2
	double	m_dHVF_Pseudo_Equivalent_Width; // @ADDED v1.2

	int iBest_Fit_File;
	const char * lpszBest_Fit_File;
	bool bBest_Fit_With_Shell;
    ES::Spectrum cBest_Fit_Spectrum;
    ES::Spectrum cBest_Fit_Spectrum_No_PS;
    ES::Spectrum cBest_Fit_Spectrum_No_HVF;
    ES::Spectrum cBest_Fit_Spectrum_Continuum; // @ADDED v1.3

private:
	unsigned int uiNum_Fit_Moments;
	double * lpdFit_Raw_Moments;
	double * lpdFit_Central_Moments;
	double * lpdFit_Standardized_Moments;

private:
	char * lpszInternal_Best_Fit_File;
	int		iInternal_Best_Fit_File_Size;

	void Clear(void)
	{
		lpszBest_Fit_File = NULL;
		if (lpszInternal_Best_Fit_File)
			delete [] lpszInternal_Best_Fit_File;
		lpszInternal_Best_Fit_File = NULL;
		iInternal_Best_Fit_File_Size = 0;
		Reset();
	}
	void Copy(const BEST_FIT_DATA & i_cRHO)
	{
		dBest_Fit = i_cRHO.dBest_Fit;
		dSource_Data_MJD = i_cRHO.dSource_Data_MJD;
		dBest_Fit_PS_Ion_Temp = i_cRHO.dBest_Fit_PS_Ion_Temp;
		dBest_Fit_PS_Temp = i_cRHO.dBest_Fit_PS_Temp;
		dBest_Fit_Day = i_cRHO.dBest_Fit_Day;
		dBest_Fit_PS_Log_Tau = i_cRHO.dBest_Fit_PS_Log_Tau;
		dBest_Fit_HVF_Ion_Temp = i_cRHO.dBest_Fit_HVF_Ion_Temp;
		dBest_Fit_PS_Vel = i_cRHO.dBest_Fit_PS_Vel;
		dBest_Fit_HVF_Log_Tau = i_cRHO.dBest_Fit_HVF_Log_Tau;
		dBest_Fit_PS_Ion_Vmin = i_cRHO.dBest_Fit_PS_Ion_Vmin;
		dBest_Fit_PS_Ion_Vmax = i_cRHO.dBest_Fit_PS_Ion_Vmax;
		dBest_Fit_PS_Ion_Aux = i_cRHO.dBest_Fit_PS_Ion_Aux;
		dBest_Fit_HVF_Ion_Vmin = i_cRHO.dBest_Fit_HVF_Ion_Vmin;
		dBest_Fit_HVF_Ion_Vmax = i_cRHO.dBest_Fit_HVF_Ion_Vmax;
		dBest_Fit_HVF_Ion_Aux = i_cRHO.dBest_Fit_HVF_Ion_Aux;
		iBest_Fit_File = i_cRHO.iBest_Fit_File;
		bBest_Fit_With_Shell = i_cRHO.bBest_Fit_With_Shell;
		m_dPS_Absorption_Peak_Velocity = i_cRHO.m_dPS_Absorption_Peak_Velocity;
		m_dHVF_Absorption_Peak_Velocity = i_cRHO.m_dHVF_Absorption_Peak_Velocity;

		m_dPVF_Pseudo_Equivalent_Width = i_cRHO.m_dPVF_Pseudo_Equivalent_Width;
		m_dHVF_Pseudo_Equivalent_Width = i_cRHO.m_dHVF_Pseudo_Equivalent_Width;

		cBest_Fit_Spectrum = i_cRHO.cBest_Fit_Spectrum;
		cBest_Fit_Spectrum_No_PS = i_cRHO.cBest_Fit_Spectrum_No_PS;
		cBest_Fit_Spectrum_No_HVF = i_cRHO.cBest_Fit_Spectrum_No_HVF;
		cBest_Fit_Spectrum_Continuum = i_cRHO.cBest_Fit_Spectrum_Continuum;
//		lpszBest_Fit_File;
		if (lpszInternal_Best_Fit_File)
			delete [] lpszInternal_Best_Fit_File;
		iInternal_Best_Fit_File_Size = 0;
		if (i_cRHO.lpszInternal_Best_Fit_File && i_cRHO.lpszBest_Fit_File == i_cRHO.lpszInternal_Best_Fit_File)
		{
			lpszInternal_Best_Fit_File = new char[i_cRHO.iInternal_Best_Fit_File_Size];
			iInternal_Best_Fit_File_Size = i_cRHO.iInternal_Best_Fit_File_Size;
			strcpy(lpszInternal_Best_Fit_File,i_cRHO.lpszInternal_Best_Fit_File);
			lpszBest_Fit_File = lpszInternal_Best_Fit_File;
		}
		else
			lpszBest_Fit_File = i_cRHO.lpszBest_Fit_File;

		SetNumMoments(i_cRHO.uiNum_Fit_Moments);
		if (uiNum_Fit_Moments > 0)
		{
			memset(lpdFit_Raw_Moments,0,sizeof(double) * uiNum_Fit_Moments);
			memset(lpdFit_Central_Moments,0,sizeof(double) * uiNum_Fit_Moments);
			memset(lpdFit_Standardized_Moments,0,sizeof(double) * uiNum_Fit_Moments);
		}
	}
public:
	BEST_FIT_DATA(void)
	{
		uiNum_Fit_Moments = 0;
		lpszInternal_Best_Fit_File = NULL;
		lpdFit_Raw_Moments = NULL;
		lpdFit_Central_Moments = NULL;
		lpdFit_Standardized_Moments = NULL;
		Clear();
	}

	void Reset(void)
	{
		dBest_Fit = DBL_MAX;
		dSource_Data_MJD = 
		dBest_Fit_PS_Ion_Temp =
		dBest_Fit_PS_Temp =
		dBest_Fit_Day =
		dBest_Fit_PS_Log_Tau =
		dBest_Fit_HVF_Ion_Temp =
		dBest_Fit_PS_Vel =
		dBest_Fit_HVF_Log_Tau =
		dBest_Fit_PS_Ion_Vmin =
		dBest_Fit_PS_Ion_Vmax =
		dBest_Fit_PS_Ion_Aux =
		dBest_Fit_HVF_Ion_Vmin =
		dBest_Fit_HVF_Ion_Vmax =
		dBest_Fit_HVF_Ion_Aux = nan("");
		m_dPS_Absorption_Peak_Velocity =
		m_dHVF_Absorption_Peak_Velocity = 
		m_dPVF_Pseudo_Equivalent_Width = 
		m_dHVF_Pseudo_Equivalent_Width = nan("");

		iBest_Fit_File = -128;
		bBest_Fit_With_Shell = false;
		cBest_Fit_Spectrum.clear();
		cBest_Fit_Spectrum_No_PS.clear();
		cBest_Fit_Spectrum_No_HVF.clear();
		cBest_Fit_Spectrum_Continuum.clear();
		if (lpszInternal_Best_Fit_File)
			lpszInternal_Best_Fit_File[0] = 0;

		if (uiNum_Fit_Moments > 0)
		{
			if (lpdFit_Raw_Moments)
				memset(lpdFit_Raw_Moments,0,sizeof(double) * uiNum_Fit_Moments);
			if (lpdFit_Central_Moments)
				memset(lpdFit_Central_Moments,0,sizeof(double) * uiNum_Fit_Moments);
			if (lpdFit_Standardized_Moments)
				memset(lpdFit_Standardized_Moments,0,sizeof(double) * uiNum_Fit_Moments);
		}
	}

	~BEST_FIT_DATA(void)
	{
		if (lpszInternal_Best_Fit_File)
			delete [] lpszInternal_Best_Fit_File;
		lpszInternal_Best_Fit_File = NULL;
		lpszBest_Fit_File = NULL;
		if (lpdFit_Raw_Moments)
			delete [] lpdFit_Raw_Moments;
		if (lpdFit_Central_Moments)
			delete [] lpdFit_Central_Moments;
		if (lpdFit_Standardized_Moments)
			delete [] lpdFit_Standardized_Moments;
		lpdFit_Raw_Moments = NULL;
		lpdFit_Central_Moments = NULL;
		lpdFit_Standardized_Moments = NULL;
		uiNum_Fit_Moments = 0;
	}

	BEST_FIT_DATA(const BEST_FIT_DATA & i_cRHO)
	{
		uiNum_Fit_Moments = 0;
		lpszInternal_Best_Fit_File = NULL;
		lpdFit_Raw_Moments = NULL;
		lpdFit_Central_Moments = NULL;
		lpdFit_Standardized_Moments = NULL;
		Clear();
		Copy(i_cRHO);
	}

	BEST_FIT_DATA & operator =(const BEST_FIT_DATA & i_cRHO)
	{
		Copy(i_cRHO);
		return *this;
	}
	void SetNumMoments(unsigned int i_uiNum_Moments)
	{
		if (uiNum_Fit_Moments < i_uiNum_Moments)
		{
			if (lpdFit_Raw_Moments)
				delete [] lpdFit_Raw_Moments;
			if (lpdFit_Central_Moments)
				delete [] lpdFit_Central_Moments;
			if (lpdFit_Standardized_Moments)
				delete [] lpdFit_Standardized_Moments;
			lpdFit_Raw_Moments = NULL;
			lpdFit_Central_Moments = NULL;
			lpdFit_Standardized_Moments = NULL;
		}
		uiNum_Fit_Moments = i_uiNum_Moments;
		if (uiNum_Fit_Moments > 0)
		{
			lpdFit_Raw_Moments = new double[uiNum_Fit_Moments];
			lpdFit_Central_Moments = new double[uiNum_Fit_Moments];
			lpdFit_Standardized_Moments = new double[uiNum_Fit_Moments];
			memset(lpdFit_Raw_Moments,0,sizeof(double) * uiNum_Fit_Moments);
			memset(lpdFit_Central_Moments,0,sizeof(double) * uiNum_Fit_Moments);
			memset(lpdFit_Standardized_Moments,0,sizeof(double) * uiNum_Fit_Moments);
		}
	}
	unsigned int GetNumMoments(void) const {return uiNum_Fit_Moments;}
	void SetMoments(unsigned int i_uiMoment,const double & i_dRaw, const double & i_dCentral, const double & i_dStandardized)
	{
		if (i_uiMoment > 0 && i_uiMoment <= uiNum_Fit_Moments)
		{
			lpdFit_Raw_Moments[i_uiMoment - 1] = i_dRaw;
			lpdFit_Central_Moments[i_uiMoment - 1] = i_dCentral;
			lpdFit_Standardized_Moments[i_uiMoment - 1] = i_dStandardized;
		}
	}
	void GetMoments(unsigned int i_uiMoment,double & o_dRaw, double & o_dCentral, double & o_dStandardized) const
	{
		if (i_uiMoment > 0 && i_uiMoment <= uiNum_Fit_Moments)
		{
			o_dRaw = lpdFit_Raw_Moments[i_uiMoment - 1];
			o_dCentral = lpdFit_Central_Moments[i_uiMoment - 1];
			o_dStandardized = lpdFit_Standardized_Moments[i_uiMoment - 1];
		}
	}
	void SetBestFitFile(const char * i_lspzBest_Fit_File)
	{
		// copy to internal storage - note that at this point lpszBest_Fit_File is a pointer to lpszBuffer
		if (!lpszInternal_Best_Fit_File || strlen(i_lspzBest_Fit_File) < (iInternal_Best_Fit_File_Size + 1))
		{
			if (lpszInternal_Best_Fit_File)
				delete [] lpszInternal_Best_Fit_File;
			iInternal_Best_Fit_File_Size = strlen(i_lspzBest_Fit_File) + 1;
			if (iInternal_Best_Fit_File_Size < 32)
				iInternal_Best_Fit_File_Size = 32;
			lpszInternal_Best_Fit_File = new char[iInternal_Best_Fit_File_Size];
		}
		strcpy(lpszInternal_Best_Fit_File,i_lspzBest_Fit_File);
		lpszBest_Fit_File = lpszInternal_Best_Fit_File;
	}
	void Output(const char * i_lpszFilename, bool i_bChi2)
	{
		FILE * fileDataOut;
		if (i_lpszFilename && i_lpszFilename[0] != 0)
			fileDataOut = fopen(i_lpszFilename,"wt");
		else
			fileDataOut = stdout;
		if (i_bChi2)
			fprintf(fileDataOut,"Minimum chi^2: %.3e\n", dBest_Fit);
		else
			fprintf(fileDataOut,"Minimum variance: %.3e\n", dBest_Fit);
		fprintf(fileDataOut,"model: %s\n",lpszBest_Fit_File);
		if (iBest_Fit_File >= 0)
		{
			fprintf(fileDataOut,"Data MJD: %.2f\n",dSource_Data_MJD);
			fprintf(fileDataOut,"Day: %.4f\n",dBest_Fit_Day);
			fprintf(fileDataOut,"PS Vel: %.4f\n",dBest_Fit_PS_Vel);
			fprintf(fileDataOut,"PS Temp: %.4f\n",dBest_Fit_PS_Temp);
			fprintf(fileDataOut,"PS Log Tau: %.4f\n",dBest_Fit_PS_Log_Tau);
			fprintf(fileDataOut,"PS Ion Temp: %.4f\n",dBest_Fit_PS_Ion_Temp);
			if (bBest_Fit_With_Shell)
			{
				fprintf(fileDataOut,"HVF Log Tau: %.4f\n",dBest_Fit_HVF_Log_Tau);
				fprintf(fileDataOut,"HVF Ion Temp: %.4f\n",dBest_Fit_HVF_Ion_Temp);
			}
			fprintf(fileDataOut,"PS Peak Vel: %.4f\n",m_dPS_Absorption_Peak_Velocity);
			fprintf(fileDataOut,"HVF Peak Vel: %.4f\n",m_dHVF_Absorption_Peak_Velocity);
			fprintf(fileDataOut,"PVF pEW: %.4f\n",m_dPVF_Pseudo_Equivalent_Width);
			fprintf(fileDataOut,"HVF pEW: %.4f\n",m_dHVF_Pseudo_Equivalent_Width);
		}
		else
		{
			fprintf(fileDataOut,"Data MJD: %.2f\n",dSource_Data_MJD);
			fprintf(fileDataOut,"PS Vel: %.4f\n",dBest_Fit_PS_Vel);
			fprintf(fileDataOut,"PS Temp: %.4f\n",dBest_Fit_PS_Temp);
			fprintf(fileDataOut,"PS Log Tau: %.4f\n",dBest_Fit_PS_Log_Tau);
			fprintf(fileDataOut,"PS Ion Temp: %.4f\n",dBest_Fit_PS_Ion_Temp);
			fprintf(fileDataOut,"PS Ion Vmin: %.4f\n",dBest_Fit_PS_Ion_Vmin);
			fprintf(fileDataOut,"PS Ion Vmax: %.4f\n",dBest_Fit_PS_Ion_Vmax);
			fprintf(fileDataOut,"PS Ion Aux: %.4f\n",dBest_Fit_PS_Ion_Aux);
			if (bBest_Fit_With_Shell)
			{
				fprintf(fileDataOut,"HVF Log Tau: %.4f\n",dBest_Fit_HVF_Log_Tau);
				fprintf(fileDataOut,"HVF Ion Temp: %.4f\n",dBest_Fit_HVF_Ion_Temp);
				fprintf(fileDataOut,"HVF Ion Vmin: %.4f\n",dBest_Fit_HVF_Ion_Vmin);
				fprintf(fileDataOut,"HVF Ion Vmax: %.4f\n",dBest_Fit_HVF_Ion_Vmax);
				fprintf(fileDataOut,"HVF Ion Aux: %.4f\n",dBest_Fit_HVF_Ion_Aux);
			}
		}
		if (uiNum_Fit_Moments > 0)
		{
			fprintf(fileDataOut,"Num moments: %i\n",uiNum_Fit_Moments);
			for (unsigned int uiI = 0; uiI < uiNum_Fit_Moments; uiI++)
				fprintf(fileDataOut,"Raw %i: %.17e\n",uiI,lpdFit_Raw_Moments[uiI]);
			for (unsigned int uiI = 0; uiI < uiNum_Fit_Moments; uiI++)
				fprintf(fileDataOut,"Central %i: %.17e\n",uiI,lpdFit_Central_Moments[uiI]);
			for (unsigned int uiI = 0; uiI < uiNum_Fit_Moments; uiI++)
				fprintf(fileDataOut,"Standardized %i: %.17e\n",uiI,lpdFit_Standardized_Moments[uiI]);
		}
		if (i_lpszFilename && i_lpszFilename[0] != 0)
			fclose(fileDataOut);
	}
	void OutputBinary(const char * i_lpszFilename)
	{
		FILE * fileOut = fopen(i_lpszFilename,"wb");
		if (fileOut)
		{
			const char lpszHeader[]={"FITDATA"};
			double	dVersion = 1.3;			
			fwrite(lpszHeader,sizeof(char),sizeof(lpszHeader),fileOut);
			fwrite(&dVersion,sizeof(double),1,fileOut);

			fwrite(&dBest_Fit,sizeof(double),1,fileOut);
			fwrite(&dSource_Data_MJD,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Ion_Temp,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Temp,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_Day,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Log_Tau,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_HVF_Ion_Temp,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Vel,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_HVF_Log_Tau,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Ion_Vmin,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Ion_Vmax,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_PS_Ion_Aux,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_HVF_Ion_Vmin,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_HVF_Ion_Vmax,sizeof(double),1,fileOut);
			fwrite(&dBest_Fit_HVF_Ion_Aux,sizeof(double),1,fileOut);
			fwrite(&m_dPS_Absorption_Peak_Velocity,sizeof(double),1,fileOut);
			fwrite(&m_dHVF_Absorption_Peak_Velocity,sizeof(double),1,fileOut);
			fwrite(&m_dPVF_Pseudo_Equivalent_Width,sizeof(double),1,fileOut);
			fwrite(&m_dHVF_Pseudo_Equivalent_Width,sizeof(double),1,fileOut);
			fwrite(&iBest_Fit_File,sizeof(int),1,fileOut);
			int iSize = strlen(lpszBest_Fit_File) + 1;
			fwrite(&iSize,sizeof(int),1,fileOut);
			fwrite(lpszBest_Fit_File,sizeof(char),iSize,fileOut);
			fwrite(&bBest_Fit_With_Shell,sizeof(bool),1,fileOut);
			iSize = cBest_Fit_Spectrum.size();
			fwrite(&iSize,sizeof(int),1,fileOut);
			for (unsigned int uiI = 0; uiI < iSize; uiI++)
			{
				fwrite(&cBest_Fit_Spectrum.wl(uiI),sizeof(double),1,fileOut);
				fwrite(&cBest_Fit_Spectrum.flux(uiI),sizeof(double),1,fileOut);
			}
			iSize = cBest_Fit_Spectrum_No_HVF.size();
			fwrite(&iSize,sizeof(int),1,fileOut);
			for (unsigned int uiI = 0; uiI < iSize; uiI++)
			{
				fwrite(&cBest_Fit_Spectrum_No_HVF.wl(uiI),sizeof(double),1,fileOut);
				fwrite(&cBest_Fit_Spectrum_No_HVF.flux(uiI),sizeof(double),1,fileOut);
			}
			if (bBest_Fit_With_Shell)
			{
				iSize = cBest_Fit_Spectrum_No_PS.size();
				fwrite(&iSize,sizeof(int),1,fileOut);
				for (unsigned int uiI = 0; uiI < iSize; uiI++)
				{
					fwrite(&cBest_Fit_Spectrum_No_PS.wl(uiI),sizeof(double),1,fileOut);
					fwrite(&cBest_Fit_Spectrum_No_PS.flux(uiI),sizeof(double),1,fileOut);
				}
			}
			iSize = cBest_Fit_Spectrum_Continuum.size();
			fwrite(&iSize,sizeof(int),1,fileOut);
			for (unsigned int uiI = 0; uiI < iSize; uiI++)
			{
				fwrite(&cBest_Fit_Spectrum_Continuum.wl(uiI),sizeof(double),1,fileOut);
				fwrite(&cBest_Fit_Spectrum_Continuum.flux(uiI),sizeof(double),1,fileOut);
			}
			fwrite(&uiNum_Fit_Moments,sizeof(int),1,fileOut);
			if (uiNum_Fit_Moments != 0)
			{
				fwrite(lpdFit_Raw_Moments,sizeof(double),uiNum_Fit_Moments,fileOut);
				fwrite(lpdFit_Central_Moments,sizeof(double),uiNum_Fit_Moments,fileOut);
				fwrite(lpdFit_Standardized_Moments,sizeof(double),uiNum_Fit_Moments,fileOut);
			}
			fclose(fileOut);
		}
	}
	int ReadBinary(const char * i_lpszFilename, bool i_bQuiet = false)
	{
		int iRet = 1;
		FILE * fileIn = fopen(i_lpszFilename,"rb");
		if (fileIn)
		{
			const char lpszHeader[]={"FITDATA"};
			char lpszRead_Header[32];
			double	dVersion;
			fread(lpszRead_Header,sizeof(char),sizeof(lpszHeader),fileIn);
			if (strcmp(lpszRead_Header,lpszHeader) == 0)
			{
				fread(&dVersion,sizeof(double),1,fileIn);
				if (dVersion < 1.3 && !i_bQuiet)
					fprintf(stderr,"Best fit data Read Binary: warning: %s is a version %f data file.\n",i_lpszFilename,dVersion);

				fread(&dBest_Fit,sizeof(double),1,fileIn);
				if (dVersion >= 1.1)
					fread(&dSource_Data_MJD,sizeof(double),1,fileIn);

				fread(&dBest_Fit_PS_Ion_Temp,sizeof(double),1,fileIn);
				fread(&dBest_Fit_PS_Temp,sizeof(double),1,fileIn);
				fread(&dBest_Fit_Day,sizeof(double),1,fileIn);
				fread(&dBest_Fit_PS_Log_Tau,sizeof(double),1,fileIn);
				fread(&dBest_Fit_HVF_Ion_Temp,sizeof(double),1,fileIn);
				fread(&dBest_Fit_PS_Vel,sizeof(double),1,fileIn);
				fread(&dBest_Fit_HVF_Log_Tau,sizeof(double),1,fileIn);
				fread(&dBest_Fit_PS_Ion_Vmin,sizeof(double),1,fileIn);
				fread(&dBest_Fit_PS_Ion_Vmax,sizeof(double),1,fileIn);
				fread(&dBest_Fit_PS_Ion_Aux,sizeof(double),1,fileIn);
				fread(&dBest_Fit_HVF_Ion_Vmin,sizeof(double),1,fileIn);
				fread(&dBest_Fit_HVF_Ion_Vmax,sizeof(double),1,fileIn);
				fread(&dBest_Fit_HVF_Ion_Aux,sizeof(double),1,fileIn);
				fread(&m_dPS_Absorption_Peak_Velocity,sizeof(double),1,fileIn);
				fread(&m_dHVF_Absorption_Peak_Velocity,sizeof(double),1,fileIn);
				if (dVersion >= 1.2)
				{
					fread(&m_dPVF_Pseudo_Equivalent_Width,sizeof(double),1,fileIn);
					fread(&m_dHVF_Pseudo_Equivalent_Width,sizeof(double),1,fileIn);
				}
				fread(&iBest_Fit_File,sizeof(int),1,fileIn);
				int iSize;
				fread(&iSize,sizeof(int),1,fileIn);
				if (lpszInternal_Best_Fit_File)
					delete [] lpszInternal_Best_Fit_File;
				lpszInternal_Best_Fit_File = new char [iSize];
				fread(lpszInternal_Best_Fit_File,sizeof(char),iSize,fileIn);
				lpszBest_Fit_File = lpszInternal_Best_Fit_File;
				iInternal_Best_Fit_File_Size = iSize;				
				fread(&bBest_Fit_With_Shell,sizeof(bool),1,fileIn);
				fread(&iSize,sizeof(int),1,fileIn);
				cBest_Fit_Spectrum = ES::Spectrum::create_from_size(iSize);
				for (unsigned int uiI = 0; uiI < iSize; uiI++)
				{
					fread(&cBest_Fit_Spectrum.wl(uiI),sizeof(double),1,fileIn);
					fread(&cBest_Fit_Spectrum.flux(uiI),sizeof(double),1,fileIn);
				}
				fread(&iSize,sizeof(int),1,fileIn);
				cBest_Fit_Spectrum_No_HVF = ES::Spectrum::create_from_size(iSize);
				for (unsigned int uiI = 0; uiI < iSize; uiI++)
				{
					fread(&cBest_Fit_Spectrum_No_HVF.wl(uiI),sizeof(double),1,fileIn);
					fread(&cBest_Fit_Spectrum_No_HVF.flux(uiI),sizeof(double),1,fileIn);
				}
				if (bBest_Fit_With_Shell)
				{
					fread(&iSize,sizeof(int),1,fileIn);
					cBest_Fit_Spectrum_No_PS = ES::Spectrum::create_from_size(iSize);
					for (unsigned int uiI = 0; uiI < iSize; uiI++)
					{
						fread(&cBest_Fit_Spectrum_No_PS.wl(uiI),sizeof(double),1,fileIn);
						fread(&cBest_Fit_Spectrum_No_PS.flux(uiI),sizeof(double),1,fileIn);
					}
				}
				else 
					cBest_Fit_Spectrum_No_PS.clear();
				if (dVersion >= 1.3)
				{
					fread(&iSize,sizeof(int),1,fileIn);
					cBest_Fit_Spectrum_Continuum = ES::Spectrum::create_from_size(iSize);
					for (unsigned int uiI = 0; uiI < iSize; uiI++)
					{
						fread(&cBest_Fit_Spectrum_Continuum.wl(uiI),sizeof(double),1,fileIn);
						fread(&cBest_Fit_Spectrum_Continuum.flux(uiI),sizeof(double),1,fileIn);
					}
				}
				fread(&iSize,sizeof(int),1,fileIn);
				SetNumMoments(iSize);
				if (uiNum_Fit_Moments != 0)
				{
					fread(lpdFit_Raw_Moments,sizeof(double),uiNum_Fit_Moments,fileIn);
					fread(lpdFit_Central_Moments,sizeof(double),uiNum_Fit_Moments,fileIn);
					fread(lpdFit_Standardized_Moments,sizeof(double),uiNum_Fit_Moments,fileIn);
				}
				iRet = 0;
			}
			else
			{
				if (!i_bQuiet)
					fprintf(stderr,"Best fit data Read Binary: %s is not a fit data file.\n",i_lpszFilename);
				iRet = 2;
			}
			fclose(fileIn);
		}
		return iRet;
	}

	void ReadFromFile(const char * i_lpszFilename)
	{
		Reset();
		FILE * fileDataIn = fopen(i_lpszFilename,"rt");
		char lpszBuffer[1024];
		if (fileDataIn)
		{
			fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
			if (strstr(lpszBuffer,"model") == NULL)
			{
				dBest_Fit =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				const char * lpszBest_Fit_File_Cursor = xGetLastWordInString(lpszBuffer);
				SetBestFitFile(lpszBest_Fit_File_Cursor);
			}
			else
			{
				char * lpszBest_Fit_File_Cursor = xGetLastWordInString(lpszBuffer);
				SetBestFitFile(lpszBest_Fit_File_Cursor);
				lpszBest_Fit_File_Cursor -= 7;
				lpszBest_Fit_File_Cursor[0] = 0; // get rid of the 'model: xxxxx' part
				dBest_Fit =	xGetNumberFromEndOfString(lpszBuffer);
			}
			// strip CR/LF from best fit file name
			if (lpszInternal_Best_Fit_File)
			{
				char * lpszCursor = lpszInternal_Best_Fit_File + strlen(lpszInternal_Best_Fit_File);
				lpszCursor--;
				if (lpszCursor >= lpszBest_Fit_File)
				{
					while (lpszCursor[0] == 10 || lpszCursor[0] == 13)
					{
						lpszCursor[0] = 0;	
						lpszCursor--;
					}
				}
			}

			if (strcmp(lpszBest_Fit_File,"exphvf") == 0)
			{
				iBest_Fit_File = -1;
			}
			else if (strcmp(lpszBest_Fit_File,"expps") == 0)
			{
				iBest_Fit_File = -2;
			}
			else
			{
				iBest_Fit_File = 0;
			}
				
			switch (iBest_Fit_File)
			{
			case 0:
			default:
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				if (strstr(lpszBuffer,"MJD") != 0)
				{
					dSource_Data_MJD = xGetNumberFromEndOfString(lpszBuffer);
					fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
					dBest_Fit_Day =	xGetNumberFromEndOfString(lpszBuffer);
				}
				else
					dBest_Fit_Day =	xGetNumberFromEndOfString(lpszBuffer);

				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Vel =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Temp =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Log_Tau =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Ion_Temp =	xGetNumberFromEndOfString(lpszBuffer);

				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				bBest_Fit_With_Shell = (strstr(lpszBuffer,"HVF") != NULL);
				
				if (bBest_Fit_With_Shell)
				{
					dBest_Fit_HVF_Log_Tau =	xGetNumberFromEndOfString(lpszBuffer);
					fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
					dBest_Fit_HVF_Ion_Temp =	xGetNumberFromEndOfString(lpszBuffer);
				}
				break;
			case -1:
			case -2:
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Vel =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Temp =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Log_Tau =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Ion_Temp =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Ion_Vmin =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Ion_Vmax =	xGetNumberFromEndOfString(lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				dBest_Fit_PS_Ion_Aux =	xGetNumberFromEndOfString(lpszBuffer);

				fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
				bBest_Fit_With_Shell = (strstr(lpszBuffer,"HVF") != NULL);
				
				if (bBest_Fit_With_Shell)
				{
					dBest_Fit_HVF_Log_Tau =	xGetNumberFromEndOfString(lpszBuffer);
					fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
					dBest_Fit_HVF_Ion_Temp =	xGetNumberFromEndOfString(lpszBuffer);
					fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
					dBest_Fit_HVF_Ion_Vmin =	xGetNumberFromEndOfString(lpszBuffer);
					fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
					dBest_Fit_HVF_Ion_Vmax =	xGetNumberFromEndOfString(lpszBuffer);
					fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
					dBest_Fit_HVF_Ion_Aux =	xGetNumberFromEndOfString(lpszBuffer);
				}
				break;
			}
			if (fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn))
			{
				unsigned int uiFile_Num_Fit_Moments =	xGetNumberFromEndOfString(lpszBuffer);
				SetNumMoments(uiFile_Num_Fit_Moments);
				if (uiNum_Fit_Moments > 0)
				{
					for (unsigned int uiI = 0; uiI < uiNum_Fit_Moments; uiI++)
					{
						fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
						lpdFit_Raw_Moments[uiI] = xGetNumberFromEndOfString(lpszBuffer);
					}
					for (unsigned int uiI = 0; uiI < uiNum_Fit_Moments; uiI++)
					{
						fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
						lpdFit_Central_Moments[uiI] = xGetNumberFromEndOfString(lpszBuffer);
					}
					for (unsigned int uiI = 0; uiI < uiNum_Fit_Moments; uiI++)
					{
						fgets(lpszBuffer,sizeof(lpszBuffer),fileDataIn);
						lpdFit_Standardized_Moments[uiI] = xGetNumberFromEndOfString(lpszBuffer);
					}
				}
			}

			fclose(fileDataIn);
		}
		else
			fprintf(stderr,"Failed to read file %s\n",i_lpszFilename);
	}
};

