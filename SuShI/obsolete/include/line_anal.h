#pragma once
#include <xastro.h>

class ATOMIC_IONIZATION_DATA
{
public:
	unsigned int m_uiZ;
	unsigned int m_uiIonization_Energies_Count;
	double * m_lpIonization_energies_erg;
private:
	void Copy(const ATOMIC_IONIZATION_DATA &i_cRHO)
	{
		if (m_lpIonization_energies_erg && m_uiIonization_Energies_Count < i_cRHO.m_uiIonization_Energies_Count)
		{
			delete [] m_lpIonization_energies_erg;
			m_lpIonization_energies_erg =  NULL;
		}
		if (m_lpIonization_energies_erg == NULL)
			m_lpIonization_energies_erg = new double[i_cRHO.m_uiIonization_Energies_Count];

		memcpy(m_lpIonization_energies_erg,i_cRHO.m_lpIonization_energies_erg,i_cRHO.m_uiIonization_Energies_Count * sizeof(double));
		m_uiIonization_Energies_Count = i_cRHO.m_uiIonization_Energies_Count;
		m_uiZ = i_cRHO.m_uiZ;
		
	}
public:
	ATOMIC_IONIZATION_DATA(void)
	{
		m_uiZ = 0;
		m_uiIonization_Energies_Count = 0;
		m_lpIonization_energies_erg = NULL;
	}
	~ATOMIC_IONIZATION_DATA(void)
	{
		m_uiZ = 0;
		m_uiIonization_Energies_Count = 0;
		if (m_lpIonization_energies_erg)
			delete [] m_lpIonization_energies_erg;
		m_lpIonization_energies_erg = NULL;
	}
	ATOMIC_IONIZATION_DATA(const ATOMIC_IONIZATION_DATA &i_cRHO)
	{
		m_uiZ = 0;
		m_uiIonization_Energies_Count = 0;
		m_lpIonization_energies_erg = NULL;
		Copy(i_cRHO);
	}
	ATOMIC_IONIZATION_DATA(unsigned int i_uiZ, unsigned int i_uiIonization_Energies_Count, ...)
	{
		va_list lpvaArg_Ptr;
		va_start(lpvaArg_Ptr,i_uiIonization_Energies_Count);
		m_uiZ = i_uiZ;
		m_uiIonization_Energies_Count = i_uiIonization_Energies_Count;
		m_lpIonization_energies_erg = new double[i_uiIonization_Energies_Count];
		for (unsigned int uiI = 0; uiI < i_uiIonization_Energies_Count; uiI++)
		{
			m_lpIonization_energies_erg[uiI] = va_arg(lpvaArg_Ptr,double);
		}
		va_end(lpvaArg_Ptr);
	}
	ATOMIC_IONIZATION_DATA & operator =(const ATOMIC_IONIZATION_DATA i_cRHO)
	{
		Copy(i_cRHO);
		return *this;
	}
};

class ATOMIC_IONIZATION_DATA_LI
{
public:
	ATOMIC_IONIZATION_DATA m_cData;
	ATOMIC_IONIZATION_DATA_LI * m_lpNext;
	ATOMIC_IONIZATION_DATA_LI * m_lpPrev;

	ATOMIC_IONIZATION_DATA_LI(void) : m_cData() {m_lpNext = m_lpPrev = NULL;}
	ATOMIC_IONIZATION_DATA_LI(const ATOMIC_IONIZATION_DATA &i_cRHO) : m_cData(i_cRHO) {m_lpNext = m_lpPrev = NULL;}
	ATOMIC_IONIZATION_DATA_LI(unsigned int i_uiZ, unsigned int i_uiIonization_Energies_Count, ...) : m_cData()
	{
		unsigned int uiI;
		va_list lpvaArg_Ptr;
		va_start(lpvaArg_Ptr,i_uiIonization_Energies_Count);
		m_cData.m_uiZ = i_uiZ;
		m_cData.m_uiIonization_Energies_Count = i_uiIonization_Energies_Count;
		m_cData.m_lpIonization_energies_erg = new double[m_cData.m_uiIonization_Energies_Count];
		for (uiI = 0; uiI < i_uiIonization_Energies_Count; uiI++)
		{
			m_cData.m_lpIonization_energies_erg[uiI] = va_arg(lpvaArg_Ptr,double);
		}
		va_end(lpvaArg_Ptr);

		m_lpNext = m_lpPrev = NULL;
	}
	ATOMIC_IONIZATION_DATA_LI & operator =(const ATOMIC_IONIZATION_DATA_LI i_cRHO)
	{
		m_cData = i_cRHO.m_cData;
		return *this;
	}
};

class ATOMIC_IONIZATION_DATA_LIST
{
private:
	unsigned int				m_uiNum_List_Items;
	unsigned int *				m_uiQuick_Find_Z;
	ATOMIC_IONIZATION_DATA **	m_lpcQuick_Find_Data;

	
	ATOMIC_IONIZATION_DATA_LI * m_lpHead;
	ATOMIC_IONIZATION_DATA_LI * m_lpTail;
	
public:
	ATOMIC_IONIZATION_DATA_LIST(void)
	{
		m_lpHead = NULL;
		m_lpTail = NULL;
		m_uiNum_List_Items = 0;
		m_uiQuick_Find_Z = NULL;
		m_lpcQuick_Find_Data = NULL;
	}
	void Initialize(void)
	{
		m_uiNum_List_Items = 0;
		FILE * fileIn = fopen("ionization_data.dat","rt");
		if (fileIn)
		{
			ATOMIC_IONIZATION_DATA_LI * lpCurr;
			unsigned int uiEntry_Index = 0;
			char *lpszBuffer = new char[1024];
			double lpdIonization_Energies[30];
			char *lpszCursor;
			lpszCursor = fgets(lpszBuffer,1024,fileIn); // read header line
			while(fgets(lpszBuffer,1024,fileIn))
			{
				lpszCursor = lpszBuffer;
				if (lpszCursor[0])
				{
					if (xIsWhitespace(lpszCursor))
						lpszCursor = xPassWhitespace(lpszCursor);
					// Entry index
					lpszCursor = xPassNumber(lpszCursor);
					lpszCursor = xPassWhitespace(lpszCursor);
					// Z
					unsigned int uiZ = atoi(lpszCursor);
					lpszCursor = xPassNumber(lpszCursor);
					lpszCursor = xPassWhitespace(lpszCursor);
					// Element name
					while (!xIsWhitespace(lpszCursor))
						lpszCursor++;
					lpszCursor = xPassWhitespace(lpszCursor);
					// Element symbol
					while (!xIsWhitespace(lpszCursor))
						lpszCursor++;
					lpszCursor = xPassWhitespace(lpszCursor);
					uiEntry_Index = 0;
					while (lpszCursor && lpszCursor[0] != 0 && lpszCursor[0] != 10) // 10 = end of line
					{
						// ionization data
						lpszCursor = Read_Table_Entry(lpdIonization_Energies[uiEntry_Index],lpszCursor);
						lpdIonization_Energies[uiEntry_Index] *= g_XASTRO.k_derg_eV;
						uiEntry_Index++;					
					}
				
					// add entry to list
					lpCurr = new ATOMIC_IONIZATION_DATA_LI;
					lpCurr->m_cData.m_uiZ = uiZ;
					lpCurr->m_cData.m_uiIonization_Energies_Count = uiEntry_Index;
					lpCurr->m_cData.m_lpIonization_energies_erg = new double[uiEntry_Index];
					memcpy(lpCurr->m_cData.m_lpIonization_energies_erg,lpdIonization_Energies,sizeof(double) * uiEntry_Index);
					m_uiNum_List_Items++;
					// insert into list; make sure list remains sorted by Z
					if (!m_lpHead)
					{
						m_lpHead = m_lpTail = lpCurr;
					}
					else
					{
						if (uiZ > m_lpTail->m_cData.m_uiZ) // end of list
						{
							m_lpTail->m_lpNext = lpCurr;
							lpCurr->m_lpPrev = m_lpTail;
							m_lpTail = lpCurr;
						}
						else if (uiZ < m_lpHead->m_cData.m_uiZ) // beginning of list
						{
							m_lpHead->m_lpPrev = lpCurr;
							lpCurr->m_lpNext = m_lpHead;
							m_lpHead = lpCurr;
						}
						else // somewhere in the middle
						{
							ATOMIC_IONIZATION_DATA_LI * lpPrev = m_lpTail;
							while (lpPrev && lpPrev->m_cData.m_uiZ > uiZ)
								lpPrev = lpPrev->m_lpPrev;
							if (lpPrev) // we'd better get here!
							{
								ATOMIC_IONIZATION_DATA_LI * lpNext = lpPrev->m_lpNext;
								lpPrev->m_lpNext = lpCurr;
								lpCurr->m_lpPrev = lpPrev;
								lpCurr->m_lpNext = lpNext;
								if (lpNext)
									lpNext->m_lpPrev = lpCurr;
							}
						}
					}
				}


			}
			fclose(fileIn);
			fileIn = NULL;
			// generate quick find lists
			m_uiQuick_Find_Z = new unsigned int[m_uiNum_List_Items];
			m_lpcQuick_Find_Data = new ATOMIC_IONIZATION_DATA *[m_uiNum_List_Items];
			uiEntry_Index = 0;
			lpCurr = m_lpHead;
			while (lpCurr)
			{
				m_uiQuick_Find_Z[uiEntry_Index] = lpCurr->m_cData.m_uiZ;
				m_lpcQuick_Find_Data[uiEntry_Index] = &(lpCurr->m_cData);
				uiEntry_Index++;
				lpCurr = lpCurr->m_lpNext;
			}
		}
	}
	unsigned int Find_Z_Index(unsigned int i_uiZ)
	{
		unsigned int uiIndex = (unsigned int)(-1);
		unsigned int uiHigh_Index = m_uiNum_List_Items - 1;
		unsigned int uiLow_Index = 0;

		while(uiHigh_Index != uiLow_Index && uiIndex == (unsigned int)(-1))
		{
			unsigned int uiTest_Idx = (uiHigh_Index + uiLow_Index) >> 1;		
			if (m_uiQuick_Find_Z[uiTest_Idx] == i_uiZ)
				uiIndex = uiTest_Idx;
			else if (m_uiQuick_Find_Z[uiTest_Idx] > i_uiZ)
				uiHigh_Index = uiTest_Idx;
			else if (uiTest_Idx == uiLow_Index)
				uiLow_Index = uiTest_Idx + 1;
			else
				uiLow_Index = uiTest_Idx;
				
		}
		if (uiIndex == (unsigned int)(-1) && m_uiQuick_Find_Z[uiHigh_Index] == i_uiZ)
			uiIndex = uiHigh_Index;
		return uiIndex;
	}
	ATOMIC_IONIZATION_DATA	Get_Ionization_Data(unsigned int i_uiZ)
	{
		ATOMIC_IONIZATION_DATA cData;
		unsigned int uiZ_Index = Find_Z_Index(i_uiZ);
		if (uiZ_Index != (unsigned int)(-1))
			cData = m_lpcQuick_Find_Data[uiZ_Index][0];
		return cData;
	}
};
extern ATOMIC_IONIZATION_DATA_LIST	g_xIonization_Energy_Data;


class ATOMIC_PARTITION_FUNCTION_DATA
{
	// This class is used for estimating the partition function for ionized atoms
	// method is based on Cardona, Matrinez-Arroyo & Lopez-Castillo, ApJ 711:239-245 (2010)
public:
	unsigned int 	m_uiZ;
	unsigned int	m_uiNum_Entries;
	unsigned int *	m_lpuiJ;
	double *		m_lpdE;
	unsigned int *	m_lpuiG;
	unsigned int *	m_lpuiM;
	double * 		m_lpdChi;
	double 			m_dE1;
	unsigned int	m_uiG1;

	void	Copy(const ATOMIC_PARTITION_FUNCTION_DATA & i_cRHO)
	{
		m_uiZ = i_cRHO.m_uiZ;
		if (m_uiNum_Entries > 0 && m_uiNum_Entries < i_cRHO.m_uiNum_Entries)
		{
			delete [] m_lpuiJ;	m_lpuiJ = NULL;
			delete [] m_lpdE;	m_lpdE = NULL;
			delete [] m_lpuiG;	m_lpuiG = NULL;
			delete [] m_lpuiM;	m_lpuiM = NULL;
			delete [] m_lpdChi;	m_lpdChi = NULL;
			m_uiNum_Entries = 0;
		}
		if (m_uiNum_Entries == 0)
		{
			m_uiNum_Entries = i_cRHO.m_uiNum_Entries;
			m_lpuiJ = new unsigned int[m_uiNum_Entries];
			m_lpdE = new double[m_uiNum_Entries];
			m_lpuiG = new unsigned int[m_uiNum_Entries];
			m_lpuiM = new unsigned int[m_uiNum_Entries];
			m_lpdChi = new double[m_uiNum_Entries];
		}
		memcpy(m_lpuiJ,i_cRHO.m_lpuiJ,sizeof(unsigned int) * m_uiNum_Entries);
		memcpy(m_lpdE,i_cRHO.m_lpdE,sizeof(double) * m_uiNum_Entries);
		memcpy(m_lpuiG,i_cRHO.m_lpuiG,sizeof(unsigned int) * m_uiNum_Entries);
		memcpy(m_lpuiM,i_cRHO.m_lpuiM,sizeof(unsigned int) * m_uiNum_Entries);
		memcpy(m_lpdChi,i_cRHO.m_lpdChi,sizeof(double) * m_uiNum_Entries);

		m_dE1 = i_cRHO.m_dE1;
		m_uiG1 = i_cRHO.m_uiG1;
	}

	ATOMIC_PARTITION_FUNCTION_DATA(void)
	{
		m_uiZ = 0;
		m_uiNum_Entries = 0;
		m_lpuiJ = m_lpuiG = NULL;
		m_lpdE = NULL;
		m_dE1 = 0.0;
		m_uiG1 = 0;
	}
	~ATOMIC_PARTITION_FUNCTION_DATA(void)
	{
		m_uiZ = 0;
		m_uiNum_Entries = 0;
		if (m_lpuiJ)
			delete [] m_lpuiJ;
		m_lpuiJ = NULL;
		if (m_lpuiG)
			delete [] m_lpuiG;
		m_lpuiG = NULL;
		if (m_lpdE)
			delete [] m_lpdE;
		m_lpdE = NULL;
		m_dE1 = 0.0;
		m_uiG1 = 0;
	}
	ATOMIC_PARTITION_FUNCTION_DATA(const ATOMIC_PARTITION_FUNCTION_DATA & i_cRHO)
	{
		m_uiZ = 0;
		m_uiNum_Entries = 0;
		m_lpuiJ = m_lpuiG = NULL;
		m_lpdE = NULL;
		m_dE1 = 0.0;
		m_uiG1 = 0;
		Copy(i_cRHO);
	}
	ATOMIC_PARTITION_FUNCTION_DATA & operator =(const ATOMIC_PARTITION_FUNCTION_DATA & i_cRHO)
	{
		Copy(i_cRHO);
		return *this;
	}
	
		

	double	Get_Partition_Function(unsigned int i_uiIonization_State, const  double &i_dTemperature, const double &i_dNumber_Density) const
	{
		unsigned int uiIndex = (unsigned int)(-1);
		double	dU = 1.0;
		if (i_uiIonization_State < m_uiNum_Entries && i_uiIonization_State == m_lpuiJ[i_uiIonization_State])
			uiIndex = i_uiIonization_State;
		else if (i_uiIonization_State < m_uiZ) // make sure this state makes sense
		{
			unsigned int uiLow = 0;
			unsigned int uiHigh = m_uiNum_Entries;
			unsigned int uiCurr;
			while (uiLow != uiHigh && uiIndex == (unsigned int)(-1))
			{
				uiCurr = (uiHigh + uiLow) >> 1;
				if (m_lpuiJ[uiCurr] == i_uiIonization_State)
					uiIndex = uiCurr;
				else if (m_lpuiJ[uiCurr] > i_uiIonization_State)
					uiHigh = uiCurr;
				else if (uiCurr == uiLow)
					uiLow = uiCurr + 1;
				else
					uiLow = uiCurr;
			}
		}
		if (uiIndex != (unsigned int)(-1))
		{
			double	d1_kt = 1.0 / (g_XASTRO.k_dKb * i_dTemperature);
			double	dZeff = (i_uiIonization_State + 1);
			double dq = sqrt(dZeff / (g_cConstants.dPi * 2.0 * g_XASTRO.k_da0)) * pow(i_dNumber_Density,-1.0 / 6.0) * 0.5;
			double dn_star = dq * (1.0 + sqrt(1.0 + 2.0 / dq));
			int iN_star = (int)(dn_star + 0.5);
			double	dE_nstar = m_lpdChi[uiIndex] - dZeff * dZeff / (iN_star * iN_star) * g_XASTRO.k_dRy;

			double	dA = m_uiG1 * exp(-m_dE1 * d1_kt);
			double	dB = m_lpuiG[uiIndex] * exp(-m_lpdE[uiIndex] * d1_kt);
			double	dC;
			if (iN_star > 7) // n' was chosen to be 7, so if n* < 7, drop these terms
				dC = m_lpuiM[uiIndex] / 3.0 * (iN_star * iN_star * iN_star - 343) * exp(-dE_nstar * d1_kt);
			else
				dC = 0.0;

			dU = dA + dB + dC;
		}
		return dU;
	}
};


class ATOMIC_PARTITION_FUNCTION_DATA_LI
{
public:
	ATOMIC_PARTITION_FUNCTION_DATA m_cData;
	ATOMIC_PARTITION_FUNCTION_DATA_LI * m_lpNext;
	ATOMIC_PARTITION_FUNCTION_DATA_LI * m_lpPrev;

	ATOMIC_PARTITION_FUNCTION_DATA_LI(void) : m_cData() {m_lpNext = m_lpPrev = NULL;}
	ATOMIC_PARTITION_FUNCTION_DATA_LI(const ATOMIC_PARTITION_FUNCTION_DATA &i_cRHO) : m_cData(i_cRHO) {m_lpNext = m_lpPrev = NULL;}
	ATOMIC_PARTITION_FUNCTION_DATA_LI & operator =(const ATOMIC_PARTITION_FUNCTION_DATA_LI i_cRHO)
	{
		m_cData = i_cRHO.m_cData;
		return *this;
	}
};

class ATOMIC_PARTITION_FUNCTION_DATA_LIST
{
private:
	unsigned int				m_uiNum_List_Items;
	unsigned int *				m_uiQuick_Find_Z;
	ATOMIC_PARTITION_FUNCTION_DATA **	m_lpcQuick_Find_Data;

	
	ATOMIC_PARTITION_FUNCTION_DATA_LI * m_lpHead;
	ATOMIC_PARTITION_FUNCTION_DATA_LI * m_lpTail;
public:
	ATOMIC_PARTITION_FUNCTION_DATA_LIST(void)
	{
		m_lpHead = NULL;
		m_lpTail = NULL;
		m_uiNum_List_Items = 0;
		m_uiQuick_Find_Z = NULL;
		m_lpcQuick_Find_Data = NULL;
	}
	void Initialize(void)
	{
		m_uiNum_List_Items = 0;
		FILE * fileIn = fopen("pf_data.dat","rt");
		if (fileIn)
		{
			ATOMIC_PARTITION_FUNCTION_DATA_LI * lpCurr;
			unsigned int uiEntry_Index = 0;
			char *lpszBuffer = new char[1024];
			unsigned int uiZ = 0, uiZnew;
			unsigned int lpuiJ[30];
			double lpdEnergy[30];
			unsigned int lpuiG[30];
			unsigned int lpuiM[30];
			char *lpszCursor;
			lpszCursor = fgets(lpszBuffer,1024,fileIn); // read header line
			while(fgets(lpszBuffer,1024,fileIn))
			{
				lpszCursor = lpszBuffer;
				if (xIsWhitespace(lpszCursor))
					lpszCursor = xPassWhitespace(lpszCursor);
				// Z
				uiZnew = atoi(lpszCursor);
				lpszCursor = xPassNumber(lpszCursor);
				lpszCursor = xPassWhitespace(lpszCursor);
				if (uiZnew != uiZ && uiZ != 0)
				{ 		// add entry to list
					lpCurr = new ATOMIC_PARTITION_FUNCTION_DATA_LI;
					lpCurr->m_cData.m_uiZ = uiZ;
					lpCurr->m_cData.m_uiNum_Entries = uiEntry_Index;
					lpCurr->m_cData.m_lpuiJ = new unsigned int[uiEntry_Index];
					lpCurr->m_cData.m_lpdE = new double[uiEntry_Index];
					lpCurr->m_cData.m_lpuiG = new unsigned int [uiEntry_Index];
					lpCurr->m_cData.m_lpuiM = new unsigned int [uiEntry_Index];
					lpCurr->m_cData.m_lpdChi = new double [uiEntry_Index];

					ATOMIC_IONIZATION_DATA cIon_Data = g_xIonization_Energy_Data.Get_Ionization_Data(uiZ);

					memcpy(lpCurr->m_cData.m_lpuiJ,lpuiJ,sizeof(unsigned int) * uiEntry_Index);
					memcpy(lpCurr->m_cData.m_lpdE,lpdEnergy,sizeof(double) * uiEntry_Index);
					memcpy(lpCurr->m_cData.m_lpuiG,lpuiG,sizeof(unsigned int) * uiEntry_Index);
					memcpy(lpCurr->m_cData.m_lpuiM,lpuiM,sizeof(unsigned int) * uiEntry_Index);
					for (unsigned int uiI = 0; uiI < uiEntry_Index; uiI++)
					{
						lpCurr->m_cData.m_lpdChi[uiI] = cIon_Data.m_lpIonization_energies_erg[lpuiJ[uiI]];
					}

					lpCurr->m_cData.m_dE1 = 0.0;//cIon_Data.m_lpIonization_energies_erg[0];
					lpCurr->m_cData.m_uiG1 = 2; // @@TODO

					m_uiNum_List_Items++;
					// insert into list; make sure list remains sorted by Z
					if (!m_lpHead)
					{
						m_lpHead = m_lpTail = lpCurr;
					}
					else
					{
						if (uiZ > m_lpTail->m_cData.m_uiZ) // end of list
						{
							m_lpTail->m_lpNext = lpCurr;
							lpCurr->m_lpPrev = m_lpTail;
							m_lpTail = lpCurr;
						}
						else if (uiZ < m_lpHead->m_cData.m_uiZ) // beginning of list
						{
							m_lpHead->m_lpPrev = lpCurr;
							lpCurr->m_lpNext = m_lpHead;
							m_lpHead = lpCurr;
						}
						else // somewhere in the middle
						{
							ATOMIC_PARTITION_FUNCTION_DATA_LI * lpPrev = m_lpTail;
							while (lpPrev && lpPrev->m_cData.m_uiZ > uiZ)
								lpPrev = lpPrev->m_lpPrev;
							if (lpPrev) // we'd better get here!
							{
								ATOMIC_PARTITION_FUNCTION_DATA_LI * lpNext = lpPrev->m_lpNext;
								lpPrev->m_lpNext = lpCurr;
								lpCurr->m_lpPrev = lpPrev;
								lpCurr->m_lpNext = lpNext;
								if (lpNext)
									lpNext->m_lpPrev = lpCurr;
							}
						}
					}

					uiEntry_Index = 0;
				}
				uiZ = uiZnew;

				lpuiJ[uiEntry_Index] = atoi(lpszCursor);
				lpszCursor = xPassNumber(lpszCursor);
				lpszCursor = xPassWhitespace(lpszCursor);
				if (lpszCursor[0] != '*') // no data
				{
					lpdEnergy[uiEntry_Index] = atof(lpszCursor);
					lpszCursor = xPassNumber(lpszCursor);
					lpszCursor = xPassWhitespace(lpszCursor);

					lpuiG[uiEntry_Index] = atoi(lpszCursor);
					lpszCursor = xPassNumber(lpszCursor);
					lpszCursor = xPassWhitespace(lpszCursor);
				
					lpuiM[uiEntry_Index] = atoi(lpszCursor);
					lpszCursor = xPassNumber(lpszCursor);
					lpszCursor = xPassWhitespace(lpszCursor);
					uiEntry_Index++;
				}
			}
			lpCurr = new ATOMIC_PARTITION_FUNCTION_DATA_LI;
			lpCurr->m_cData.m_uiZ = uiZ;
			lpCurr->m_cData.m_uiNum_Entries = uiEntry_Index;
			lpCurr->m_cData.m_lpuiJ = new unsigned int[uiEntry_Index];
			lpCurr->m_cData.m_lpdE = new double[uiEntry_Index];
			lpCurr->m_cData.m_lpuiG = new unsigned int [uiEntry_Index];
			lpCurr->m_cData.m_lpuiM = new unsigned int [uiEntry_Index];
			lpCurr->m_cData.m_lpdChi = new double [uiEntry_Index];

			ATOMIC_IONIZATION_DATA cIon_Data = g_xIonization_Energy_Data.Get_Ionization_Data(uiZ);

			memcpy(lpCurr->m_cData.m_lpuiJ,lpuiJ,sizeof(unsigned int) * uiEntry_Index);
			memcpy(lpCurr->m_cData.m_lpdE,lpdEnergy,sizeof(double) * uiEntry_Index);
			memcpy(lpCurr->m_cData.m_lpuiG,lpuiG,sizeof(unsigned int) * uiEntry_Index);
			memcpy(lpCurr->m_cData.m_lpuiM,lpuiM,sizeof(unsigned int) * uiEntry_Index);
			for (unsigned int uiI = 0; uiI < uiEntry_Index; uiI++)
			{
				lpCurr->m_cData.m_lpdChi[uiI] = cIon_Data.m_lpIonization_energies_erg[lpuiJ[uiI]];
			}

			lpCurr->m_cData.m_dE1 = cIon_Data.m_lpIonization_energies_erg[0];
			lpCurr->m_cData.m_uiG1 = 2; // @@TODO

			m_uiNum_List_Items++;
			// insert into list; make sure list remains sorted by Z
			if (!m_lpHead)
			{
				m_lpHead = m_lpTail = lpCurr;
			}
			else
			{
				if (uiZ > m_lpTail->m_cData.m_uiZ) // end of list
				{
					m_lpTail->m_lpNext = lpCurr;
					lpCurr->m_lpPrev = m_lpTail;
					m_lpTail = lpCurr;
				}
				else if (uiZ < m_lpHead->m_cData.m_uiZ) // beginning of list
				{
					m_lpHead->m_lpPrev = lpCurr;
					lpCurr->m_lpNext = m_lpHead;
					m_lpHead = lpCurr;
				}
				else // somewhere in the middle
				{
					ATOMIC_PARTITION_FUNCTION_DATA_LI * lpPrev = m_lpTail;
					while (lpPrev && lpPrev->m_cData.m_uiZ > uiZ)
						lpPrev = lpPrev->m_lpPrev;
					if (lpPrev) // we'd better get here!
					{
						ATOMIC_PARTITION_FUNCTION_DATA_LI * lpNext = lpPrev->m_lpNext;
						lpPrev->m_lpNext = lpCurr;
						lpCurr->m_lpPrev = lpPrev;
						lpCurr->m_lpNext = lpNext;
						if (lpNext)
							lpNext->m_lpPrev = lpCurr;
					}
				}
			}

			fclose(fileIn);
			fileIn = NULL;
			// generate quick find lists
			m_uiQuick_Find_Z = new unsigned int[m_uiNum_List_Items];
			m_lpcQuick_Find_Data = new ATOMIC_PARTITION_FUNCTION_DATA *[m_uiNum_List_Items];
			uiEntry_Index = 0;
			lpCurr = m_lpHead;
			while (lpCurr)
			{
				m_uiQuick_Find_Z[uiEntry_Index] = lpCurr->m_cData.m_uiZ;
				m_lpcQuick_Find_Data[uiEntry_Index] = &(lpCurr->m_cData);
				uiEntry_Index++;
				lpCurr = lpCurr->m_lpNext;
			}
		}
	}
	unsigned int Find_Z_Index(unsigned int i_uiZ)
	{
		unsigned int uiIndex = (unsigned int)(-1);
		unsigned int uiHigh_Index = m_uiNum_List_Items - 1;
		unsigned int uiLow_Index = 0;

		while(uiHigh_Index != uiLow_Index && uiIndex == (unsigned int)(-1))
		{
			unsigned int uiTest_Idx = (uiHigh_Index + uiLow_Index) >> 1;		
			if (m_uiQuick_Find_Z[uiTest_Idx] == i_uiZ)
				uiIndex = uiTest_Idx;
			else if (m_uiQuick_Find_Z[uiTest_Idx] > i_uiZ)
				uiHigh_Index = uiTest_Idx;
			else if (uiTest_Idx == uiLow_Index)
				uiLow_Index = uiTest_Idx + 1;
			else
				uiLow_Index = uiTest_Idx;
		}
		if (uiIndex == (unsigned int)(-1) && m_uiQuick_Find_Z[uiHigh_Index] == i_uiZ)
			uiIndex = uiHigh_Index;
		return uiIndex;
	}
	ATOMIC_PARTITION_FUNCTION_DATA	Get_PF_Data(unsigned int i_uiZ)
	{
		ATOMIC_PARTITION_FUNCTION_DATA cData;
		unsigned int uiZ_Index = Find_Z_Index(i_uiZ);
		if (uiZ_Index != (unsigned int)(-1))
			cData = m_lpcQuick_Find_Data[uiZ_Index][0];
		return cData;
	}
};
extern ATOMIC_PARTITION_FUNCTION_DATA_LIST	g_xPartition_Fn_Data;

double Optical_Depth(const double & i_dMass_Density_Ref, const LINE_DATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double &i_dTime_Ref);

