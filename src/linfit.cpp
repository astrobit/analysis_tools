#include<cstdio>
#include<cstdlib>
#include <xstdlib.h>
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
	unsigned int uiFile_Idx = -1;
	unsigned int uiX_Col = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--xcol", -1);
	unsigned int uiY_Col = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ycol", -1);

	if (uiX_Col != -1 || uiY_Col != -1)
	{
		// figure out where the filename is
		//@@TODO
	}
	if (uiX_Col == -1)
	{
		if (uiY_Col != 0)
			uiX_Col = 0;
		else
			fprintf(stderr,"Error - X column not specified but Y column set to 0\n");
	}
	if (uiY_Col == -1)
	{
		if (uiX_Col != 1)
			uiY_Col = 1;
		else
			fprintf(stderr,"Error - Y column not specified but X column set to 1\n");
	}

	unsigned int uiTest_Idx = 1;
// OK, where the fuck did you put it?
	bool bFault = false;
	while (uiTest_Idx < i_iArg_Count && uiFile_Idx == -1 && !bFault)
	{
		if (strstr(i_lpszArg_Values[uiTest_Idx],"--xcol") != nullptr ||
			strstr(i_lpszArg_Values[uiTest_Idx],"--ycol") != nullptr)
		{
			if (i_lpszArg_Values[uiTest_Idx][6] == '=' && i_lpszArg_Values[uiTest_Idx][7] != 0)
				uiTest_Idx++; // normal parameter form: --xcol=1
			else if (i_lpszArg_Values[uiTest_Idx][6] == '=' && i_lpszArg_Values[uiTest_Idx][7] == 0)
			{
				uiTest_Idx += 2; // parameter form: --xcol= 1
			}
			else
			{
				uiTest_Idx++;
				if (i_lpszArg_Values[uiTest_Idx][0] == '=' && i_lpszArg_Values[uiTest_Idx][1] != 0)
					uiTest_Idx++; // parameter form: --xcol =1
				else if (i_lpszArg_Values[uiTest_Idx][0] == '=' && i_lpszArg_Values[uiTest_Idx][1] == 0)
					uiTest_Idx += 2; // parameter form: --xcol = 1
				else
					bFault = true;
			}
		}
		else
			uiFile_Idx = uiTest_Idx;
	}
		
	if (uiFile_Idx != -1)
	{	
		cData.ReadDataFile(i_lpszArg_Values[uiFile_Idx],false,false,',',1);
		xvector vA(2);
		double dX0 = cData.GetElement(uiX_Col,0);
		double dY0 = cData.GetElement(uiY_Col,0);
		double dXend = cData.GetElement(uiX_Col,cData.GetNumElements() - 1);
		double dYend = cData.GetElement(uiY_Col,cData.GetNumElements() - 1);
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
			vX.push_back(cData.GetElement(uiX_Col,uiI));
			vY.push_back(cData.GetElement(uiY_Col,uiI));
			vW.push_back(cData.GetElement(uiY_Col,uiI) * 0.00001);
		}
		xvector xvX(vX);
		xvector xvY(vY);
		xvector xvW(vW);

		XSQUARE_MATRIX cCovariance_Matrix(2);
		double dSmin;
		printf("Starting fit for %s (Xcol = %i,Ycol = %i)\n",i_lpszArg_Values[uiFile_Idx],uiX_Col,uiY_Col);

		if (GeneralFit(xvX, xvY,xvW, Linear_Function, vA, cCovariance_Matrix, dSmin, NULL))
		{
			printf("m = %.17e +/- %.3e\nb=%.17e +/- %.3e\nS=%.3e\n",vA[1],sqrt(cCovariance_Matrix.Get(1,1)),vA[0],sqrt(cCovariance_Matrix.Get(0,0)),dSmin);
		}
		else
			printf("Convergence failed\n");
	}
	else
	{
		fprintf(stderr,"Usage: %s <filepath> <options>\n",i_lpszArg_Values[0]);
		fprintf(stderr,"Performs a linear fit on the data in column 0 (x) and 1 (y) of the specified file. Assumes 1 header line. Filepath and options may appear in any order\n");
		fprintf(stderr,"Options:\n --xcol=<col>  - specifies the column to use as the x data. --ycol=<col>  - specifies the column to use as the y data.");
		

		fprintf(stderr,"\n%s",i_lpszArg_Values[0]);
		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			fprintf(stderr," %s",i_lpszArg_Values[uiI]);
		}
		fprintf(stderr,"\n");
	}


	return 0;
}
