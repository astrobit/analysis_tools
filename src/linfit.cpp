#include<cstdio>
#include<cstdlib>
#include <xio.h>
#include <xlinalg.h>
#include <xfit.h>
#include <vector>

xvector Linear_Function(const double &i_dX, const xvector & i_vA, void * io_lpvData)
{
	xvector vRet(3);
	vRet.Set(0,i_vA[0] + i_vA[1] * i_dX);
	vRet.Set(1,1.0);
	vRet.Set(2,i_dX);

	return vRet;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	xdataset	cData;
	if (i_iArg_Count == 2)
	{
		cData.ReadDataFile(i_lpszArg_Values[1],false,false,',',1);
		xvector vA(2);
		double dX0 = cData.GetElement(0,0);
		double dY0 = cData.GetElement(1,0);
		double dXend = cData.GetElement(0,cData.GetNumElements() - 1);
		double dYend = cData.GetElement(1,cData.GetNumElements() - 1);
		double dSlope = (dYend - dY0) / (dXend - dX0);
		double dYint = dY0 - dSlope * dX0;
		vA.Set(0,dYint);
		vA.Set(1,dSlope);

		printf("Initial paramters %.1e %.1e\n",vA[0],vA[1]);
		std::vector<double> vX;
		std::vector<double> vY;
		std::vector<double> vW;
		for (unsigned int uiI = 0; uiI < cData.GetNumElements(); uiI++)
		{
			vX.push_back(cData.GetElement(0,uiI));
			vY.push_back(cData.GetElement(1,uiI));
			vW.push_back(cData.GetElement(1,uiI) * 0.01);
		}
		xvector xvX(vX);
		xvector xvY(vY);
		xvector xvW(vW);

		XSQUARE_MATRIX cCovariance_Matrix(2);
		double dSmin;
		printf("Starting fit\n");

		if (GeneralFit(xvX, xvY,xvW, Linear_Function, vA, cCovariance_Matrix, dSmin, NULL))
		{
			printf("m = %.17e +/- %.3e\nb=%.17e +/- %.3e\nS=%.3e",vA[1],sqrt(cCovariance_Matrix.Get(0,0)),vA[0],sqrt(cCovariance_Matrix.Get(1,1)),dSmin);
		}
		else
			printf("Convergence failed\n");
	}
	else
	{
		fprintf(stderr,"Usage: %s <filepath>\n",i_lpszArg_Values[0]);
		fprintf(stderr,"Performs a linear fit on the data in column 0 (x) and 1 (y) of the specified file.\n");
	}


	return 0;
}
