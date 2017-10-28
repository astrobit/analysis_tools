#include <xio.h>
#include <xroots.h>
#include <cmath>

inline double Opacity(const double & i_dTime_Factor, const double & i_dDensity)
{
	return i_dDensity * 0.2 * i_dTime_Factor * i_dTime_Factor * i_dTime_Factor;
}

double	Diffusion_Time(XDATASET & i_cData, const double & i_dElapsed_Time, const double & i_dTime_Factor)
{
	unsigned int uiDensity_Idx = 8;
	unsigned int uiDelta_Rad_Idx = 1;
	unsigned int uiRad_Idx = 0;
	unsigned int uiVel_Inner_Idx = 26;
	unsigned int uiVel_Outer_Idx = 27;

	double	dMFP_Time = 1.0 / (2.99792458e10 * Opacity(i_dTime_Factor,i_cData.GetElement(uiDensity_Idx,0))); // approximate time to travel 1 MFP for innermost layer

	double	dTau = 0;
	for (unsigned int uiI = 0; uiI < i_cData.GetNumElements(); uiI++)
	{
		double	dDelta_R = i_cData.GetElement(uiDelta_Rad_Idx,uiI);
		double	dR = i_cData.GetElement(uiRad_Idx,uiI);
		double	dR_inner = dR - 0.5 * dDelta_R + i_cData.GetElement(uiVel_Inner_Idx,uiI) * i_dElapsed_Time;
		double	dR_outer = dR + 0.5 * dDelta_R + i_cData.GetElement(uiVel_Outer_Idx,uiI) * i_dElapsed_Time;
		dDelta_R = dR_outer - dR_inner;
		if (dDelta_R < 0.0)
			dDelta_R = fabs(dDelta_R); // hydrodynamics will prevent this, but it makes the calculation a little simpler.
//			dDelta_R = 0.0; // hydrodynamics will prevent this, but it makes the calculation a little simpler.

		dTau += Opacity(i_dTime_Factor,i_cData.GetElement(uiDensity_Idx,uiI)) * dDelta_R;
	}
	return dTau * dTau * dMFP_Time;
}
class DATA
{
public:
	XDATASET * m_lpDataset;
	double		m_dRef_Time;
	double		m_dT0;
};

double	Diffusion_Time(const DATA & i_cData, const double & i_dTime)
{
	double	dTime_Factor = i_cData.m_dT0 / i_dTime;
	double	dElapsed_Time = i_dTime - i_cData.m_dT0;
	return Diffusion_Time(i_cData.m_lpDataset[0],dElapsed_Time,dTime_Factor);
}
double	Function(const double & i_dTime, const void * i_vData)
{
	DATA * lpData = (DATA *)i_vData;
	double	dElapsed_Time = i_dTime - lpData->m_dT0;
	return (lpData->m_dRef_Time - dElapsed_Time - Diffusion_Time(lpData[0],i_dTime));
}
	

int main(int i_iArg_C, char * i_lpszArg_V[] )
{
	DATA cData;
	char lpszFilename[64];
	cData.m_lpDataset = new XDATASET();
	if (i_iArg_C == 2)
		sprintf(lpszFilename,"shell%04i.xdataset",atoi(i_lpszArg_V[1]));
	else
		printf(lpszFilename,"shell0050.xdataset");
	printf("Reading %s\n",lpszFilename);
	cData.m_lpDataset->ReadDataFileBin(lpszFilename);
	cData.m_dT0 = 5.00005422222170992e+01;

	double	dT = 2.5;	
	printf("%.4e\t%.4e\n",Diffusion_Time(cData,dT * 3600.0),Function(dT * 3600.0,&cData));

	double	dTime;	
	for (double dT_ref = 4.0; dT_ref <= 32.0; dT_ref += 1.0)
	{
		cData.m_dRef_Time = dT_ref * 3600.0; // 4h after explosion
		XROOT_Secant(Function,dTime,cData.m_dRef_Time * 0.5, &cData);
		printf("For t=%.0fh, t_d = %.4e (%.1f)\n",dT_ref,dTime,dTime/3600.0);
	}
	printf("---------------------------------------------------------------------------\n");
	for (double dT_ref = 1.0; dT_ref <= 32.0; dT_ref += 1.0)
	{
		dTime = Diffusion_Time(cData,dT_ref * 3600.0);
		printf("For t=%.0fh, t_d = %.4e (%.1f)\n",dT_ref,dTime,dTime/3600.0);
	}
	return 0;
}
