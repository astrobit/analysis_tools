#include <statepop.h>

statepop::vector statepop::Get_Relaxational_Populations(size_t i_tNum_Iterations, const statepop::vector * i_lpStart_Condition)
{
	matrix mBZ = Get_Matrix_BZ();
	vector vRet;
	vector vDel;
	vector vDelI;
	vDel.Set_Size(mBZ.size());
	vDelI.Set_Size(mBZ.size());
	vRet.Set_Size(mBZ.size());
	vRet.Zero();
	vRet[0] = 1.0;
	if (i_lpStart_Condition != nullptr && i_lpStart_Condition->size() == mBZ.size())
	{
		floattype fSum = 0.0;
		for (size_t tI = 0; tI < vRet.size(); tI++)
		{
			vRet[tI] = std::fabs((*i_lpStart_Condition)[tI]);
			fSum += vRet[tI];
		}
		floattype fInv_Sum = 1.0L / fSum;
		for (size_t tI = 0; tI < vRet.size(); tI++)
		{
			vRet[tI] *= fInv_Sum;
		}
		
				

	}
	floattype fTime = 0.0;
	// quickly open then close grad.csv; just to create an empty file
	FILE * fileGrad = fopen("grad.csv","wt");
	if (fileGrad)
	{
		fprintf(fileGrad,"iteration, time (s), timestep (s) ");
		for (size_t tI = 0; tI < vLevel_Data.size(); tI++)
		{
			fprintf(fileGrad,", %.2Lf %s %.1Lf", vLevel_Data[tI].ldElement_Code,vLevel_Data[tI].szLabel.c_str(),vLevel_Data[tI].ldJ);
		}
		fprintf(fileGrad,"\n"); 
		fclose(fileGrad);
	}
/*	floattype fSum = 0.0;	
	for (size_t tI = 0; tI < mBZ.size(); tI++)
	{
		for (size_t tJ = 0; tJ < mBZ.size(); tJ++)
		{
			fSum += mBZ.Get(tI,tJ);
		}
	}
	floattype fInv_Sum = mBZ.size() / fSum;
	mBZ *= fInv_Sum;*/
	for (size_t tI = 0; tI < i_tNum_Iterations; tI++)
	{
		vDel.Zero();
		for (size_t tK = 0; tK < mBZ.size(); tK++)
		{
			for (size_t tJ = 0; tJ < mBZ.size(); tJ++)
			{
				long double ldDel = mBZ.Get(tK,tJ) * vRet[tJ];
				vDel[tK] += ldDel;
				vDel[tJ] -= ldDel;
			}
		}
		floattype fTimestep = LDBL_MAX;
		for (size_t tJ = 0; tJ < vDel.size(); tJ++)
		{
			if (vRet[tJ] != 0.0 && vDel[tJ] < 0.0)
			{
//				if (std::fabs(vDel[tJ]) > vRet[tJ])
//				{
//					floattype fTimestep_Test = std::fabs(vDel[tJ] / vRet[tJ]) * 0.1; // time that it takes to 
//					if (fTimestep_Test < fTimestep)
//						fTimestep = fTimestep_Test;
//				}
//				else
//				{
					floattype fTimestep_Test = std::fabs(vRet[tJ] / vDel[tJ]) * 0.1; // time that it takes to move 10% out
					if (fTimestep_Test < fTimestep)
						fTimestep = fTimestep_Test;
//				}
			}

		}
		vRet += (vDel * fTimestep);
		if (tI != 0)
			fTime += fTimestep; // don't increment time on the first step because it creates this really big number.
		//printf("%i -- %.3Le -- %.24Le -- %.3Le\n",tI,fTimestep,vRet[0],vDel[0]);
		if (tI % 10000 == 0 || tI < 100)
		{
			FILE * fileGrad = fopen("grad.csv","at");
			if (fileGrad)
			{
				fprintf(fileGrad,"%i, %.3Le, %.3Le",tI,fTime,fTimestep);
				for (size_t tJ = 0; tJ < mBZ.size(); tJ++)
				{
					fprintf(fileGrad,",%.24Le",vRet[tJ]);
				}
				fprintf(fileGrad,"\n");
				fclose(fileGrad);
			}
		}
	}
	printf("Finished simulation at time %.3Le\n",fTime);
			
	return vRet;
}

