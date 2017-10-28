#include <math.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <float.h>
#include <line_routines.h>
#include <stack>

void	ABUNDANCE_LIST::Read_Table(ABUNDANCE_TYPE i_eAbundance_Type)
{
	char * lpszData_Path = getenv("LINE_ANALYSIS_DATA_PATH");
	if (lpszData_Path)
	{
		char lpszFilename[256] = {0};
		switch (i_eAbundance_Type)
		{
		case Solar:
			sprintf(lpszFilename,"%s/Solar_Abundance.csv",lpszData_Path);
			break;
		case CO_Rich:
			sprintf(lpszFilename,"%s/CO_Rich_Abundance.csv",lpszData_Path);
			break;
		case Seitenzahl_N100_2013:
			sprintf(lpszFilename,"%s/Seitenzahl_N100_2013.csv",lpszData_Path);
			break;
		case Seitenzahl_Ca_Rich:
			sprintf(lpszFilename,"%s/Seitenzahl_N100_2013_Ca_Rich.csv",lpszData_Path);
			break;
		}
		if (lpszFilename[0] != 0)
			Read_Table(lpszFilename);
	}
	else
		fprintf(stderr,"LINE_ANALYSIS_DATA_PATH is not set.  Ensure this variable is set before \nusing this version of ABUNDANCE_LIST::Read_Table.\n");
}
void	ABUNDANCE_LIST::Read_Table(const char * i_lpszFilename)
{
	XDATASET_ADVANCED	cAbundance_File;
	XDATASET_ADVANCED::DATATYPE	lpeRecord_Descriptor[] = {XDATASET_ADVANCED::XDT_STRING, XDATASET_ADVANCED::XDT_UINT, XDATASET_ADVANCED::XDT_DOUBLE, XDATASET_ADVANCED::XDT_DOUBLE, XDATASET_ADVANCED::XDT_STRING};
	cAbundance_File.DescribeRecord(lpeRecord_Descriptor,5);
	cAbundance_File.ReadDataFile(i_lpszFilename,false,',','\"');
	memset(m_dAbundances,0,sizeof(m_dAbundances));
	if (cAbundance_File.GetNumRows() > 0)
	{
		double	dAbd_Sum = 0.0;
		for (unsigned int uiI = 0; uiI < cAbundance_File.GetNumRows(); uiI++)
		{
			double dZ = Get_Element_Number(cAbundance_File.GetElementString(0,uiI));
			unsigned int uiZ = (unsigned int)(dZ);
//			printf("Load %i %f\n",uiZ,cAbundance_File.GetElementDbl(2,uiI));
			if (uiZ <= 118)
			{
				double	dAbd_Curr = pow(10.0,cAbundance_File.GetElementDbl(2,uiI));
				m_dAbundances[uiZ] += dAbd_Curr;
				m_dUncertainties[uiZ] += pow(10.0,cAbundance_File.GetElementDbl(3,uiI));
				dAbd_Sum += dAbd_Curr;
			}
			else
				fprintf(stderr,"Could not find atomic number for element %s in file %s.\n",cAbundance_File.GetElementString(0,uiI), i_lpszFilename);
		}
		double	dInv_Abd_Total = 1.0 / dAbd_Sum;
		for (unsigned int uiZ = 0; uiZ < 128; uiZ++)
		{
			m_dAbundances[uiZ] *= dInv_Abd_Total;
			m_dUncertainties[uiZ] *= dInv_Abd_Total;
		}
	}
	else
		fprintf(stderr,"Could not open abundance file %s.\n",i_lpszFilename);
}

// Gamezo et al abundance information is group abundance.  Normalize the abundances for the individual groups instead of whoel abundances
void	ABUNDANCE_LIST::Normalize_Groups(void)
{
	// Mg group: F to Al
	double	dSum = 0.0;
	for (unsigned int uiZ = 9; uiZ < 14; uiZ++)
		dSum += m_dAbundances[uiZ];
	double	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 9; uiZ < 14; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}

	// Si group: Si to Mn
	dSum = 0.0;
	for (unsigned int uiZ = 14; uiZ < 21; uiZ++)
		dSum += m_dAbundances[uiZ];
	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 14; uiZ < 21; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}

	// Fe group: Fe to Uuo (really more like Fe to Zn or Ge)
	dSum = 0.0;
	for (unsigned int uiZ = 21; uiZ < 119; uiZ++)
		dSum += m_dAbundances[uiZ];
	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 21; uiZ < 119; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}
}
