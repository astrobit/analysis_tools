#pragma once
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>

class opacity_profile_data
{
public:
	enum	GROUP	{CARBON,OXYGEN,MAGNESIUM,SILICON,IRON,SHELL};
	enum 	group	{carbon,oxygen,magnesium,silicon,iron,shell};
	double	m_dReference_Velocity[6];
	double	m_dReference_Abundance[6];
	double	m_dReference_Density[6];
	double	m_dScalar[6];
	double	m_dReference_Time_s;
	double	m_dReference_Normalization_Time[6];
public:
	opacity_profile_data(void)
	{
		m_dReference_Velocity[0] = 0.0;
		m_dReference_Velocity[1] = 0.0;
		m_dReference_Velocity[2] = 0.0;
		m_dReference_Velocity[3] = 0.0;
		m_dReference_Velocity[4] = 0.0;
		m_dReference_Velocity[5] = 0.0;
		m_dReference_Abundance[0] = 0.0;
		m_dReference_Abundance[1] = 0.0;
		m_dReference_Abundance[2] = 0.0;
		m_dReference_Abundance[3] = 0.0;
		m_dReference_Abundance[4] = 0.0;
		m_dReference_Abundance[5] = 0.0;
		m_dReference_Density[0] = 0.0;
		m_dReference_Density[1] = 0.0;
		m_dReference_Density[2] = 0.0;
		m_dReference_Density[3] = 0.0;
		m_dReference_Density[4] = 0.0;
		m_dReference_Density[5] = 0.0;
		m_dScalar[0] = 0.0;
		m_dScalar[1] = 0.0;
		m_dScalar[2] = 0.0;
		m_dScalar[3] = 0.0;
		m_dScalar[4] = 0.0;
		m_dScalar[5] = 0.0;
		m_dReference_Time_s = 0.0;
		m_dReference_Normalization_Time[0] = 0.0;
		m_dReference_Normalization_Time[1] = 0.0;
		m_dReference_Normalization_Time[2] = 0.0;
		m_dReference_Normalization_Time[3] = 0.0;
		m_dReference_Normalization_Time[4] = 0.0;
		m_dReference_Normalization_Time[5] = 0.0;
	}
private:
	double Read_Number(FILE * fileIn)
	{
		double dRet = nan("");
		char	lpszBuffer[128];
		if (fgets(lpszBuffer,sizeof(lpszBuffer),fileIn))
		{
			if (lpszBuffer[0] != 0)
			{
				const char * lpszCursor = lpszBuffer;
				while (lpszCursor[0] != 0)
					lpszCursor++;
				while (lpszCursor >= lpszBuffer && lpszCursor[0] != ',')
					lpszCursor--;
				while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
					lpszCursor++;
				if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					dRet = atof(lpszCursor);
			}
		}
		return dRet;
	}
public:
	double &	Get_Reference_Time(void)
	{
		return m_dReference_Time_s;
	}
	void Set_Reference_Time(const double & i_dTime)
	{
		m_dReference_Time_s = i_dTime;
	}
	double &	Get_Scalar(GROUP i_eGroup)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		return m_dScalar[uiIdx];
	}
	void Set_Scalar(GROUP i_eGroup, const double & i_dValue)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		m_dScalar[uiIdx] = i_dValue;
	}
	double &	Get_Density(GROUP i_eGroup)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		return m_dReference_Density[uiIdx];
	}
	void Set_Density(GROUP i_eGroup, const double & i_dValue)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		m_dReference_Density[uiIdx] = i_dValue;
	}
	double &	Get_Velocity(GROUP i_eGroup)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		return m_dReference_Velocity[uiIdx];
	}
	void Set_Velocity(GROUP i_eGroup, const double & i_dValue)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		m_dReference_Velocity[uiIdx] = i_dValue;
	}
	double &	Get_Abundance(GROUP i_eGroup)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		return m_dReference_Abundance[uiIdx];
	}
	void Set_Abundance(GROUP i_eGroup, const double & i_dValue)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		m_dReference_Abundance[uiIdx] = i_dValue;
	}
	double &	Get_Normalization_Time(GROUP i_eGroup)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		return m_dReference_Normalization_Time[uiIdx];
	}
	void Set_Normalization_Time(GROUP i_eGroup, const double & i_dValue)
	{
		unsigned int uiIdx = (unsigned int)(i_eGroup - CARBON);
		m_dReference_Normalization_Time[uiIdx] = i_dValue;
	}
public:
	int	Read_Verbose(const char * i_lpszFilename)
	{
		int iRet = 1;
		FILE * fileIn = fopen(i_lpszFilename,"rt");
		if (fileIn)
		{
			m_dReference_Time_s = Read_Number(fileIn);
			Get_Scalar(CARBON) = Read_Number(fileIn);
			Get_Scalar(OXYGEN) = Read_Number(fileIn);
			Get_Scalar(MAGNESIUM) = Read_Number(fileIn);
			Get_Scalar(SILICON) = Read_Number(fileIn);
			Get_Scalar(IRON) = Read_Number(fileIn);
			Get_Scalar(SHELL) = Read_Number(fileIn);
			Get_Abundance(CARBON) = Read_Number(fileIn);
			Get_Abundance(OXYGEN) = Read_Number(fileIn);
			Get_Abundance(MAGNESIUM) = Read_Number(fileIn);
			Get_Abundance(SILICON) = Read_Number(fileIn);
			Get_Abundance(IRON) = Read_Number(fileIn);
			Get_Density(CARBON) = Read_Number(fileIn);
			Get_Density(OXYGEN) = Read_Number(fileIn);
			Get_Density(MAGNESIUM) = Read_Number(fileIn);
			if (std::isnan(Get_Density(MAGNESIUM)))
			{
				Get_Density(MAGNESIUM) = Get_Density(CARBON);
				Get_Density(SILICON) = Get_Density(CARBON);
				Get_Density(IRON) = Get_Density(CARBON);
				Get_Density(SHELL) = Get_Density(OXYGEN);
				Get_Density(OXYGEN) = Get_Density(CARBON);
				Set_Velocity(CARBON,11.5e8);
				Set_Velocity(OXYGEN,11.5e8);
				Set_Velocity(MAGNESIUM,11.5e8);
				Set_Velocity(SILICON,11.5e8);
				Set_Velocity(IRON,11.5e8);
				Set_Velocity(SHELL,18.5e8);
				Set_Normalization_Time(CARBON,32.75*24*3600);
				Set_Normalization_Time(OXYGEN,32.75*24*3600);
				Set_Normalization_Time(MAGNESIUM,32.75*24*3600);
				Set_Normalization_Time(SILICON,32.75*24*3600);
				Set_Normalization_Time(IRON,32.75*24*3600);
				Set_Normalization_Time(SHELL,32.75*24*3600);

			}
			else
			{
				Get_Density(SILICON) = Read_Number(fileIn);
				Get_Density(IRON) = Read_Number(fileIn);
				Get_Density(SHELL) = Read_Number(fileIn);
				Get_Velocity(CARBON) = Read_Number(fileIn);
				Get_Velocity(OXYGEN) = Read_Number(fileIn);
				Get_Velocity(MAGNESIUM) = Read_Number(fileIn);
				Get_Velocity(SILICON) = Read_Number(fileIn);
				Get_Velocity(IRON) = Read_Number(fileIn);
				Get_Velocity(SHELL) = Read_Number(fileIn);
				Get_Normalization_Time(CARBON) = Read_Number(fileIn);
				Get_Normalization_Time(OXYGEN) = Read_Number(fileIn);
				Get_Normalization_Time(MAGNESIUM) = Read_Number(fileIn);
				Get_Normalization_Time(SILICON) = Read_Number(fileIn);
				Get_Normalization_Time(IRON) = Read_Number(fileIn);
				Get_Normalization_Time(SHELL) = Read_Number(fileIn);
			}
			fclose(fileIn);
			iRet = 0;
		}
		return iRet;
	}
	int Save_Verbose(const char * i_lpszFilename)
	{
		int iRet = 1;
		FILE * fileOutScalars = fopen(i_lpszFilename,"wt");
		if (fileOutScalars)
		{
			fprintf(fileOutScalars,"t ref, , %.17e\n",m_dReference_Time_s);
			fprintf(fileOutScalars,"Ejecta, C, %.17e\n",Get_Scalar(CARBON));
			fprintf(fileOutScalars,"Ejecta, O, %.17e\n",Get_Scalar(OXYGEN));
			fprintf(fileOutScalars,"Ejecta, Mg, %.17e\n",Get_Scalar(MAGNESIUM));
			fprintf(fileOutScalars,"Ejecta, Si, %.17e\n",Get_Scalar(SILICON));
			fprintf(fileOutScalars,"Ejecta, Fe, %.17e\n",Get_Scalar(IRON));
			fprintf(fileOutScalars,"Shell, ,%.17e\n",Get_Scalar(SHELL));
			fprintf(fileOutScalars,"Abd, C, %.17e\n",Get_Abundance(CARBON));
			fprintf(fileOutScalars,"Abd, O, %.17e\n",Get_Abundance(OXYGEN));
			fprintf(fileOutScalars,"Abd, Mg, %.17e\n",Get_Abundance(MAGNESIUM));
			fprintf(fileOutScalars,"Abd, Si, %.17e\n",Get_Abundance(SILICON));
			fprintf(fileOutScalars,"Abd, Fe, %.17e\n",Get_Abundance(IRON));
			fprintf(fileOutScalars,"Dens, C, %.17e\n",Get_Density(CARBON));
			fprintf(fileOutScalars,"Dens, O, %.17e\n",Get_Density(OXYGEN));
			fprintf(fileOutScalars,"Dens, Mg, %.17e\n",Get_Density(MAGNESIUM));
			fprintf(fileOutScalars,"Dens, Si, %.17e\n",Get_Density(SILICON));
			fprintf(fileOutScalars,"Dens, Fe, %.17e\n",Get_Density(IRON));
			fprintf(fileOutScalars,"Dens, Shell, %.17e\n",Get_Density(SHELL));
			fprintf(fileOutScalars,"Vel, C, %.17e\n",Get_Velocity(CARBON));
			fprintf(fileOutScalars,"Vel, O, %.17e\n",Get_Velocity(OXYGEN));
			fprintf(fileOutScalars,"Vel, Mg, %.17e\n",Get_Velocity(MAGNESIUM));
			fprintf(fileOutScalars,"Vel, Si, %.17e\n",Get_Velocity(SILICON));
			fprintf(fileOutScalars,"Vel, Fe, %.17e\n",Get_Velocity(IRON));
			fprintf(fileOutScalars,"Vel, Shell, %.17e\n",Get_Velocity(SHELL));
			fprintf(fileOutScalars,"t norm, C, %.17e\n",Get_Normalization_Time(CARBON));
			fprintf(fileOutScalars,"t norm, O, %.17e\n",Get_Normalization_Time(OXYGEN));
			fprintf(fileOutScalars,"t norm, Mg, %.17e\n",Get_Normalization_Time(MAGNESIUM));
			fprintf(fileOutScalars,"t norm, Si, %.17e\n",Get_Normalization_Time(SILICON));
			fprintf(fileOutScalars,"t norm, Fe, %.17e\n",Get_Normalization_Time(IRON));
			fprintf(fileOutScalars,"t norm, Shell, %.17e\n",Get_Normalization_Time(SHELL));
			fclose(fileOutScalars);
			iRet = 0;
		}
		return iRet;
	}
	int	Load(const char * i_lpszFilename)
	{
		int iRet = 1; // 1 = failure
		FILE * fileOut = fopen(i_lpszFilename,"rb");
		if (fileOut)
		{
			const char lpszHeader[] = {"OPACITYMAPDATA"};
			char lpszHeader_Read[32];
			double	dVersion;
			fread(lpszHeader_Read,sizeof(lpszHeader),1,fileOut);
			if (strcmp(lpszHeader_Read,lpszHeader) == 0)
			{
				fread(&dVersion,sizeof(double),1,fileOut);
				if (dVersion < 1.1)
				{
					fprintf(stderr,"Warning: %s is a version %f opacity profile data file.\n",i_lpszFilename,dVersion);
				}
				if (dVersion >= 1.0)
				{
					fread(&m_dReference_Time_s,sizeof(double),1,fileOut);
					if (dVersion < 1.2)
					{
						fread(&m_dReference_Velocity[0],sizeof(double),1,fileOut);
						m_dReference_Velocity[4] = m_dReference_Velocity[3] = m_dReference_Velocity[2] = m_dReference_Velocity[1] = m_dReference_Velocity[0];
						fread(&m_dReference_Velocity[5],sizeof(double),1,fileOut);
						if (dVersion == 1.1)
						{
							fread(&m_dReference_Density[0],sizeof(double),1,fileOut);
							fread(&m_dReference_Density[5],sizeof(double),1,fileOut);
							m_dReference_Density[4] = m_dReference_Density[3] = m_dReference_Density[2] = m_dReference_Density[1] = 	m_dReference_Density[0];
						}
					}
					else
					{
						fread(&m_dReference_Velocity,sizeof(double),6,fileOut);
						fread(&m_dReference_Density,sizeof(double),6,fileOut);
					}
					fread(m_dReference_Abundance,sizeof(double),6,fileOut);
					fread(m_dScalar,sizeof(double),6,fileOut);
					if (dVersion >= 1.2)
						fread(m_dReference_Normalization_Time,sizeof(double),6,fileOut);
					iRet = 0;
				}
			}
			else
			{
				iRet = 2;
				fprintf(stderr,"Opacity profile: %s is not a valid opacity profile data file.\n",i_lpszFilename);
			}
			fclose(fileOut);
		}
		else
		{
			iRet = 1;
			fprintf(stderr,"Opacity profile: cannot open %s.\n",i_lpszFilename);
		}
		return iRet;
	}

	int	Save(const char * i_lpszFilename)
	{
		int iRet = 1; // 1 = failure
		FILE * fileOut = fopen(i_lpszFilename,"wb");
		if (fileOut)
		{
			const char lpszHeader[] = {"OPACITYMAPDATA"};
			double	dVersion = 1.2;
			fwrite(lpszHeader,sizeof(lpszHeader),1,fileOut);
			fwrite(&dVersion,sizeof(double),1,fileOut);
			fwrite(&m_dReference_Time_s,sizeof(double),1,fileOut);
			fwrite(&m_dReference_Velocity,sizeof(double),6,fileOut);
			fwrite(&m_dReference_Density,sizeof(double),6,fileOut);
			fwrite(m_dReference_Abundance,sizeof(double),6,fileOut);
			fwrite(m_dScalar,sizeof(double),6,fileOut);
			fwrite(m_dReference_Normalization_Time,sizeof(double),6,fileOut);
			fclose(fileOut);
			iRet = 0;
		}
		return iRet;
	}
};
typedef opacity_profile_data OPACITY_PROFILE_DATA;
