#include<cstdio>
#include<cstdlib>
#include <iostream>
#include <xio.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	xdataset_improved	cA;
	xdataset_improved	cB;
	xdataset_improved	cOut;

	cA.Read_Data_File(i_lpszArg_Values[1], true, 0, 0);
	cB.Read_Data_File(i_lpszArg_Values[2], true, 0, 0);
	if (cA.Get_Num_Rows() == cB.Get_Num_Rows())
	{
		size_t tFault_Idx = -1;
		cOut.Allocate(cA.Get_Num_Rows(),2);
		for (size_t tI = 0; tI < cA.Get_Num_Rows() && tFault_Idx == -1; tI++)
		{
			if (std::fabs(cA.Get_Element_Double(tI,0) - cB.Get_Element_Double(tI,0)) < (1.0e-4 * cA.Get_Element_Double(tI,0)))
			{
				cOut.Set_Element(tI,0,cA.Get_Element_Double(tI,0));
				cOut.Set_Element(tI,1,cA.Get_Element_Double(tI,1) - cB.Get_Element_Double(tI,1));

			}
			else
				tFault_Idx = tI;
		}
		if (tFault_Idx != -1)
			std::cerr << "Wavelength fault at line " << tFault_Idx + 1 << std::endl;
		else
		{
			cOut.Save_Data_File_CSV("out.csv");
		}
	}
	else
		std::cerr << "Files have different sizes." << std::endl;



	return 0;
}
