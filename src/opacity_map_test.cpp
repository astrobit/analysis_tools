#include <xio.h>
#include <opacity_profile_data.h>

inline void Swap(double & io_dA, double & io_dB)
{
	double	dTemp = io_dA;
	io_dA = io_dB;
	io_dB = dTemp;
}

void Fill_Opacity_Map(double * o_lpdOpacity_Map, const XDATASET i_cData, unsigned int i_uiVel_Grid_Data_Points,
	unsigned int i_uiVelocity_Idx, unsigned int i_uiV_Lower_Face_Idx, unsigned int i_uiV_Upper_Face_Idx, 
	unsigned int * i_lpuiElement_Group_Idx, unsigned int i_uiNum_Element_Group, unsigned int i_uiDensity_Idx, const double i_lpdVelocity_Range[2], const double & i_dDelta_Vel_Bin, const double & i_dReference_Time,
	OPACITY_PROFILE_DATA & o_cOP_Data, OPACITY_PROFILE_DATA::GROUP i_eGroup)
{
	unsigned int uiI;
	unsigned int uiAbd_Max_Bin = 0;
	double	dAbd_Density;
	double	dAbd_Dens_Max = 0.0;
	double	dAbd_Max;
	double	dAbd_Dens;
	memset(o_lpdOpacity_Map,0,sizeof(double) * i_uiVel_Grid_Data_Points); // clear all data

//	uiI = 0;
//	while (uiI < i_cData.GetNumElements())
	for (uiI = 0; uiI < i_cData.GetNumElements(); uiI++)
	{
		double dVel = i_cData.GetElement(i_uiVelocity_Idx,uiI);
//			double dV_upper = i_cData.GetElement(i_uiVelocity_Idx,uiI);
		double	dVlower = i_cData.GetElement(i_uiV_Lower_Face_Idx,uiI);
		double	dVupper = i_cData.GetElement(i_uiV_Upper_Face_Idx,uiI);
		double	dAbd = 1.0;
		if (i_uiNum_Element_Group > 0)
		{
			dAbd = 0.0;
			for (unsigned int uiJ = 0; uiJ < i_uiNum_Element_Group; uiJ++)
			{
//					printf("%i - %i: %.2e\n",uiJ,i_lpuiElement_Group_Idx[uiJ],i_cData.GetElement(i_lpuiElement_Group_Idx[uiJ],uiI));
				double 	dPotential_Abd = i_cData.GetElement(i_lpuiElement_Group_Idx[uiJ],uiI);
				if (dPotential_Abd > 1.01e-10)
					dAbd += dPotential_Abd;
			}
//				printf("%i: %.2e\n",i_uiNum_Element_Group,dAbd);
		}


//			double	dVupper = dVel + 0.5 * dDelta_Vel;
		double	dMult = 1.0;
		if (dVlower < 0.0)
			dVlower = 0.0;
		
		if (dVlower > dVupper)
			Swap(dVlower,dVupper);
//			printf("%.2e %.2e %.2e %.2e\n",dVlower,dVupper,i_lpdVelocity_Range[0],i_dDelta_Vel_Bin);
		unsigned int uiBin_Lower = (dVlower - (i_lpdVelocity_Range[0] - i_dDelta_Vel_Bin * 0.5)) / i_dDelta_Vel_Bin;
		unsigned int uiBin_Upper = (dVupper - (i_lpdVelocity_Range[0] - i_dDelta_Vel_Bin * 0.5)) / i_dDelta_Vel_Bin;
		if (dVlower < i_lpdVelocity_Range[0] || dVupper < i_lpdVelocity_Range[0])
		{
			uiBin_Lower = 0; // this can happen because range values are at bin center
		}
		if (uiBin_Lower > uiBin_Upper)
		{
			unsigned int uiTemp = uiBin_Upper;
			uiBin_Upper = uiBin_Lower;
			uiBin_Lower = uiTemp;
		}
		if (uiBin_Upper > (i_uiVel_Grid_Data_Points - 1))
			uiBin_Upper = i_uiVel_Grid_Data_Points - 1;
//			printf("%i %i\n",uiBin_Lower,uiBin_Upper);
		double dDens = i_cData.GetElement(i_uiDensity_Idx,uiI);
		for (int uiBin = uiBin_Lower; uiBin <= uiBin_Upper && uiBin < (i_uiVel_Grid_Data_Points - 1); uiBin++)
		{
			double dBin_Lower = (uiBin - 0.5) * i_dDelta_Vel_Bin + i_lpdVelocity_Range[0];
			double dBin_Upper = (uiBin + 0.5) * i_dDelta_Vel_Bin + i_lpdVelocity_Range[0];
			if (dBin_Lower <= dVlower && dVlower <= dBin_Upper && dBin_Lower <= dVupper && dVupper <= dBin_Upper)
			{
				dMult = fabs(dVupper - dVlower) / i_dDelta_Vel_Bin;
			}
			else if (dBin_Lower <= dVlower && dVlower <= dBin_Upper)
			{
				dMult = 1.0 - (dVlower - dBin_Lower) / i_dDelta_Vel_Bin;
			}
			else if (dBin_Lower <= dVupper && dVupper <= dBin_Upper)
			{
				dMult = (dVupper - dBin_Lower) / i_dDelta_Vel_Bin;
			}
			else
				dMult = 1.0;

			//printf("%i [%i - %i] (%f,%f):(%f,%f):%f\n",uiBin, uiBin_Lower, uiBin_Upper, dVlower,dVupper,dBin_Lower,dBin_Upper,dMult,dDens*dAbd*dMult);
			o_lpdOpacity_Map[uiBin] += dMult * dAbd * dDens;
		}
		dAbd_Dens = dAbd * dDens;

		if (dAbd_Dens > dAbd_Dens_Max)
		{
			uiAbd_Max_Bin = (uiBin_Upper + uiBin_Lower) >> 1; // put it in the middle
			dAbd_Dens_Max = dAbd_Dens;
			dAbd_Max = dAbd;
			dAbd_Density = dDens;
		}

	}
	//printf("First while\n");
	unsigned int uiBin_Ref = uiAbd_Max_Bin;//(i_dV_Ref - i_lpdVelocity_Range[0]) / i_dDelta_Vel_Bin;
	//printf("bin ref  = %.2e %.2e %.2e %i (%i)\n",i_dV_Ref,i_lpdVelocity_Range[0],i_dDelta_Vel_Bin,uiBin_Ref,i_uiVel_Grid_Data_Points);
	if (uiBin_Ref < i_uiVel_Grid_Data_Points)// < i_dV_Ref);
	{
		//printf("Dest bin != 0\n");
		double dRef_Mult = o_lpdOpacity_Map[uiBin_Ref];

		o_cOP_Data.Set_Velocity(i_eGroup,i_lpdVelocity_Range[0] + i_dDelta_Vel_Bin * uiAbd_Max_Bin);
		o_cOP_Data.Set_Scalar(i_eGroup,dRef_Mult);
		o_cOP_Data.Set_Abundance(i_eGroup,dAbd_Max);
		o_cOP_Data.Set_Density(i_eGroup,dAbd_Density);
		o_cOP_Data.Set_Normalization_Time(i_eGroup,i_dReference_Time);
		double dRef = 1.0 / dRef_Mult;
		for (uiI = 0; uiI < i_uiVel_Grid_Data_Points; uiI++)
		{
//				double dVel = (uiI * i_dDelta_Vel_Bin + i_lpdVelocity_Range[0]) * 1e-8;
				// normalize to reference time
				o_lpdOpacity_Map[uiI] *= dRef;
//				if (dVel < i_lpdV_ps[uiT])
//					o_lpdOpacity_Map[uiT][uiI] = 0.0;
		}
	}
	else // 
	{
		o_cOP_Data.Set_Velocity(i_eGroup,0.0);
		o_cOP_Data.Set_Scalar(i_eGroup,0.0);
		o_cOP_Data.Set_Abundance(i_eGroup,0.0);
		o_cOP_Data.Set_Density(i_eGroup,0.0);
		o_cOP_Data.Set_Normalization_Time(i_eGroup,0.0);
		memset(o_lpdOpacity_Map,0,sizeof(double) * i_uiVel_Grid_Data_Points); // clear all data
	}
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	unsigned int uiSource_Num = 4096;
	unsigned int uiOut_Num = 2048;
	XDATASET	cSource_Data;
	double	* lpdOpacity = new double[uiOut_Num];
	double	dVrange = 150.0;
	double dDelta_Vel = dVrange / (uiOut_Num - 1);
	double dVel_Bound[2] = {dDelta_Vel * 0.5,dVrange - dDelta_Vel * 0.5};
	unsigned int lpuiElem_Data[1] = {4};
	OPACITY_PROFILE_DATA cOP_Data;

	printf("Flat opacity: t(v) = 1\n");
	cSource_Data.Allocate(5,uiSource_Num);
	for (unsigned int uiI = 0; uiI < uiSource_Num; uiI++)
	{
		double diN = 1.0 / (uiSource_Num - 1);
		double dfl = uiI * diN;
		double dfc = (uiI + 0.5) * diN;
		double dfr = (uiI + 1) * diN;
		cSource_Data.SetElement(0,uiI,dfl * dVrange); // velocity at left face
		cSource_Data.SetElement(1,uiI,dfc * dVrange); // velocity at center
		cSource_Data.SetElement(2,uiI,dfr * dVrange); // velocity at right face
		cSource_Data.SetElement(3,uiI,1.0); // density
		cSource_Data.SetElement(4,uiI,1.0); // abundance
	}		
	Fill_Opacity_Map(lpdOpacity, cSource_Data, uiOut_Num, 0, 1, 2, lpuiElem_Data, 1, 3, dVel_Bound, dDelta_Vel, 50.0, cOP_Data, OPACITY_PROFILE_DATA::SILICON);
	for (unsigned int uiI = 0; uiI < uiOut_Num; uiI++)
	{
		if (uiI != (uiOut_Num - 1) && fabs(lpdOpacity[uiI]-1.0) > 0.01)
			printf("Error at %f [%i] (%f)\n",dVel_Bound[0] + dDelta_Vel * uiI, uiI, lpdOpacity[uiI]);
	}

	printf("Exponential opacity: t(v) = t0 e^(-v/vc)\n");
	for (unsigned int uiI = 0; uiI < uiSource_Num; uiI++)
	{
		double diN = 1.0 / (uiSource_Num - 1);
//		double dfl = uiI * diN;
		double dfc = (uiI + 0.5) * diN;
//		double dfr = (uiI + 1) * diN;
		double dv = dfc * dVrange;
		double dX = dv / 50.0;
		double dDens = exp(- dX);
//		cSource_Data.SetElement(0,uiI,dfl * dVrange); // velocity at left face
//		cSource_Data.SetElement(1,uiI,dfc * dVrange); // velocity at center
//		cSource_Data.SetElement(2,uiI,dfr * dVrange); // velocity at right face
		cSource_Data.SetElement(3,uiI,dDens); // density
		cSource_Data.SetElement(4,uiI,1.0); // abundance
	}		
	Fill_Opacity_Map(lpdOpacity, cSource_Data, uiOut_Num, 0, 1, 2, lpuiElem_Data, 1, 3, dVel_Bound, dDelta_Vel, 50.0, cOP_Data, OPACITY_PROFILE_DATA::SILICON);
	for (unsigned int uiI = 0; uiI < uiOut_Num; uiI++)
	{
		double dv = dVel_Bound[0] + dDelta_Vel * uiI;
		double tv = exp(-dv/50.0);
		if (uiI != (uiOut_Num - 1) && fabs(lpdOpacity[uiI]-tv) > 0.01)
			printf("Error at %f [%i]: %f | expected %f\n",dVel_Bound[0] + dDelta_Vel * uiI, uiI, lpdOpacity[uiI],tv);
	}

	printf("Gaussian opacity: t(v) = t0 e^(-(v/vc)^2)\n");
	for (unsigned int uiI = 0; uiI < uiSource_Num; uiI++)
	{
		double diN = 1.0 / (uiSource_Num - 1);
//		double dfl = uiI * diN;
		double dfc = (uiI + 0.5) * diN;
//		double dfr = (uiI + 1) * diN;
		double dv = dfc * dVrange;
		double dX = dv / 50.0;
		double dDens = exp(- dX * dX);
//		cSource_Data.SetElement(0,uiI,dfl * dVrange); // velocity at left face
//		cSource_Data.SetElement(1,uiI,dfc * dVrange); // velocity at center
//		cSource_Data.SetElement(2,uiI,dfr * dVrange); // velocity at right face
		cSource_Data.SetElement(3,uiI,dDens); // density
		cSource_Data.SetElement(4,uiI,1.0); // abundance
	}		
	Fill_Opacity_Map(lpdOpacity, cSource_Data, uiOut_Num, 0, 1, 2, lpuiElem_Data, 1, 3, dVel_Bound, dDelta_Vel, 50.0, cOP_Data, OPACITY_PROFILE_DATA::SILICON);
	for (unsigned int uiI = 0; uiI < uiOut_Num; uiI++)
	{
		double dv = dVel_Bound[0] + dDelta_Vel * uiI;
		double dX = dv / 50.0;
		double tv = exp(- dX * dX);
		if (uiI != (uiOut_Num - 1) && fabs(lpdOpacity[uiI]-tv) > 0.01)
			printf("Error at %f [%i]: %f | expected %f\n",dVel_Bound[0] + dDelta_Vel * uiI, uiI, lpdOpacity[uiI],tv);
	}

	FILE * fileOut = fopen("opac_test.csv","wt");
	fprintf(fileOut,"Vel, opacity\n");
	for (unsigned int uiI = 0; uiI < uiOut_Num; uiI++)
	{
		fprintf(fileOut,"%f,%f\n",(dVel_Bound[0] + dDelta_Vel * uiI),lpdOpacity[uiI]);
	}
	fclose(fileOut);
	fileOut = fopen("opac_test_src.csv","wt");
	fprintf(fileOut,"Vel, density\n");
	for (unsigned int uiI = 0; uiI < uiSource_Num; uiI++)
	{
		fprintf(fileOut,"%f,%f\n",cSource_Data.GetElement(1,uiI),cSource_Data.GetElement(3,uiI)*cSource_Data.GetElement(4,uiI));
	}
	fclose(fileOut);
	return 0;
}
