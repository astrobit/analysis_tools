#include <cstdio>
#include <cstdlib>
#include <xflash.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <eps_plot.h>
#include <cfloat>
#include <utility>
#include <cstring>
#include <map>
#include <sstream>

unsigned int  Find_Variable_Idx(const XFLASH_File & cFile, const char * i_lpszVar_Name)
{
	unsigned int uiVariable_Idx = 0;
	unsigned int uiLen = strlen(i_lpszVar_Name);

	while (uiVariable_Idx < cFile.m_uiNum_Vars && (strncmp(cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name,uiLen) != 0 || (strncmp(cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name,uiLen) == 0 && cFile.m_lpszVar_Names[uiVariable_Idx][uiLen] != ' ' && cFile.m_lpszVar_Names[uiVariable_Idx][uiLen] != 0)))
	{
//		printf("%s:%s\n",cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name);
		uiVariable_Idx++;
	}
	
	return uiVariable_Idx;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	if (i_iArg_Count < 3)
	{
		fprintf(stderr,"Usage: 1dfm <variable1> <variable2>\n");
		fprintf(stderr,
"\tWill read all explos_hdf5_chk_* checkpoint files and extract the time,\n\t<variable1>, and <variable2> data.\n");
		fprintf(stderr,"\tThese will be used to generate a movie showing the evolution of these over time.\n");

	}
	else
	{
		const char *lpszVariable_X  = i_lpszArg_Values[1];
		const char *lpszVariable_Y  = i_lpszArg_Values[2];
		char lpszVar_X_Fixed[5];
		char lpszVar_Y_Fixed[5];
		strcpy(lpszVar_X_Fixed,lpszVariable_X);
		char *  lpCursor = lpszVar_X_Fixed;
		unsigned int uiI = 0;
		while (lpCursor[uiI] != 0)
			uiI++;
		for (; uiI < 4; uiI++)
			lpCursor[uiI] = ' ';
		lpCursor[4] = 0;
		lpCursor = lpszVar_Y_Fixed;
		uiI = 0;
		while (lpCursor[uiI] != 0)
			uiI++;
		for (; uiI < 4; uiI++)
			lpCursor[uiI] = ' ';
		lpCursor[4] = 0;

		std::vector<std::string> szRet;
		DIR * dirList = opendir (".");
		if (dirList)
		{
			dirent64 * direntInfo = readdir64(dirList);
			while (direntInfo)
			{
				printf("%s\n",direntInfo->d_name);
				if (strstr(direntInfo->d_name,"explos_hdf5_chk") != NULL)
					szRet.push_back(direntInfo->d_name);
				direntInfo = readdir64(dirList);
			}
		}
		std::map<double,std::string > szPlot_List;

		double dX_min = DBL_MAX;
		double dX_max = -DBL_MAX;
		double	dY_min = DBL_MAX;
		double	dY_max = -DBL_MAX;
		// first identify x and y minima and maxima for all data
		for (std::vector<std::string>::iterator cI = szRet.begin(); cI != szRet.end(); cI++)
		{
			XFLASH_File	cFlash_File;
			XFLASH_Block * lpcBlock;
			XFLASH_Block * lpcHydBlock;
			XFLASH_Block * lpcHe3Block;
			//XFLASH_Block * lpcX_Block;
			XFLASH_Block * lpcY_Block;

			printf("openning %s\n",(*cI).c_str());
			cFlash_File.Open((*cI).c_str());
		
//			unsigned int uiX_Idx = Find_Variable_Idx(cFlash_File,lpszVariable_X);
			unsigned int uiY_Idx = Find_Variable_Idx(cFlash_File,lpszVariable_Y);
			//printf("%i %i\n",uiX_Idx,uiY_Idx);
			unsigned int uiHe3_Idx = Find_Variable_Idx(cFlash_File,"he3");
			unsigned int uiHyd_Idx = Find_Variable_Idx(cFlash_File,"hyd");
			for (unsigned int uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiJ;
				double ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
				ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];

				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					lpcHydBlock = cFlash_File.GetBlock(uiHyd_Idx,uiJ);
					lpcHe3Block = cFlash_File.GetBlock(uiHe3_Idx,uiJ);
//					lpcX_Block = cFlash_File.GetBlock(uiX_Idx,uiJ);
					lpcY_Block = cFlash_File.GetBlock(uiY_Idx,uiJ);
					for (unsigned int uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
					{
						double dX = ddx * (uiL + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0];
						if (lpcHydBlock->m_lpdData[uiL] < 0.01) // in ejecta
						{
							if (dX < dX_min)
								dX_min = dX;
							if (dX > dX_max)
								dX_max = dX;
							if (lpcY_Block->m_lpdData[uiL] < dY_min)
								dY_min = lpcY_Block->m_lpdData[uiL];
							if (lpcY_Block->m_lpdData[uiL] > dY_max)
								dY_max = lpcY_Block->m_lpdData[uiL];
						}
						else if (lpcHe3Block->m_lpdData[uiL] > 1e-4) // in shell
						{
							if (dX < dX_min)
								dX_min = dX;
							if (dX > dX_max)
								dX_max = dX;
							if (lpcY_Block->m_lpdData[uiL] < dY_min)
								dY_min = lpcY_Block->m_lpdData[uiL];
							if (lpcY_Block->m_lpdData[uiL] > dY_max)
								dY_max = lpcY_Block->m_lpdData[uiL];
						}
					}
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
					//cFlash_File.ReleaseBlock(lpcX_Block);
					cFlash_File.ReleaseBlock(lpcY_Block);
				}
			}
		}
		printf("%f %f %f %f\n",dX_min,dX_max,dY_min,dY_max);
		// get the file data and produce a plot for each file
		for (std::vector<std::string>::iterator cI = szRet.begin(); cI != szRet.end(); cI++)
		{
			XFLASH_File	cFlash_File;
			XFLASH_Block * lpcBlock;
			XFLASH_Block * lpcHydBlock;
			XFLASH_Block * lpcHe3Block;
			//XFLASH_Block * lpcX_Block;
			XFLASH_Block * lpcY_Block;

			cFlash_File.Open((*cI).c_str());
			printf("Opened file %s. Time for file is %.1f s\n",(*cI).c_str(),cFlash_File.m_dTime);
//			unsigned int uiX_Idx = Find_Variable_Idx(cFlash_File,lpszVariable_X);
			unsigned int uiY_Idx = Find_Variable_Idx(cFlash_File,lpszVariable_Y);
			unsigned int uiHe3_Idx = Find_Variable_Idx(cFlash_File,"he3");
			unsigned int uiHyd_Idx = Find_Variable_Idx(cFlash_File,"hyd");
			std::vector < epsplot::eps_pair> vEjecta_Data;
			std::vector < epsplot::eps_pair> vShell_Data;
			for (unsigned int uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiJ;
				double ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
				ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];

				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					lpcHydBlock = cFlash_File.GetBlock(uiHyd_Idx,uiJ);
					lpcHe3Block = cFlash_File.GetBlock(uiHe3_Idx,uiJ);
//					lpcX_Block = cFlash_File.GetBlock(uiX_Idx,uiJ);
					lpcY_Block = cFlash_File.GetBlock(uiY_Idx,uiJ);
					for (unsigned int uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
					{
						double dX = ddx * (uiL + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0];
						if (lpcHydBlock->m_lpdData[uiL] < 0.01) // in ejecta
						{
							vEjecta_Data.push_back(epsplot::eps_pair(dX,lpcY_Block->m_lpdData[uiL]));
						}
						else if (lpcHe3Block->m_lpdData[uiL] > 1e-4) // in shell
						{
							vShell_Data.push_back(epsplot::eps_pair(dX,lpcY_Block->m_lpdData[uiL]));
						}
					}
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
					//cFlash_File.ReleaseBlock(lpcX_Block);
					cFlash_File.ReleaseBlock(lpcY_Block);
				}
			}

			epsplot::DATA cPlot;
			epsplot::PAGE_PARAMETERS	cPlot_Parameters;
			char lpszPlotName[128];
			sprintf(lpszPlotName,"/tmp/%06i.eps",szPlot_List.size());
			epsplot::AXIS_PARAMETERS	cX_Axis;
			epsplot::AXIS_PARAMETERS	cY_Axis;
			cY_Axis.m_bLog = true;
			cY_Axis.Set_Title("Density [g/cm^3]");
			cX_Axis.Set_Title("Radius [cm]");
			cX_Axis.m_dLower_Limit = dX_min;
			cX_Axis.m_dUpper_Limit = dX_max;
			cY_Axis.m_dLower_Limit = dY_min;
			cY_Axis.m_dUpper_Limit = dY_max;

			szPlot_List.insert(std::pair<double,std::string>(cFlash_File.m_dTime,lpszPlotName));
			// plot data
			epsplot::LINE_PARAMETERS 	cLine_Parameters_Ej;
			epsplot::LINE_PARAMETERS 	cLine_Parameters_Sh;
			unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis);
			unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis);
			cPlot.Set_Plot_Filename(lpszPlotName);
			cPlot_Parameters.m_uiNum_Columns = 1;
			cPlot_Parameters.m_uiNum_Rows = 1;
			cPlot_Parameters.m_dWidth_Inches = 11.0;
			cPlot_Parameters.m_dHeight_Inches = 8.5;
			unsigned int uiPlot = cPlot.Set_Plot_Data(vEjecta_Data,cLine_Parameters_Ej,uiX_Axis,uiY_Axis);
			cLine_Parameters_Sh.m_eColor = epsplot::RED;
			uiPlot = cPlot.Set_Plot_Data(vShell_Data,cLine_Parameters_Sh,uiX_Axis,uiY_Axis);
			cPlot.Plot(cPlot_Parameters);

		}

		for (std::map<double,std::string>::iterator cI = szPlot_List.begin(); cI != szPlot_List.end(); cI++)
		{
			std::ostringstream szAnimate_Command;
			szAnimate_Command << "convert -background white -rotate 90 -flatten ";
			szAnimate_Command << cI->second;
			szAnimate_Command << " ";
			szAnimate_Command << cI->second;
			szAnimate_Command << ".png";
			system(szAnimate_Command.str().c_str());
		}

		std::ostringstream szAnimate_Command;
		szAnimate_Command << "convert -delay 8 ";
		for (std::map<double,std::string>::iterator cI = szPlot_List.begin(); cI != szPlot_List.end(); cI++)
		{
			szAnimate_Command << cI->second;
			szAnimate_Command << ".png ";
		}
		szAnimate_Command << " movie.mp4";
		system(szAnimate_Command.str().c_str());

		for (std::map<double,std::string>::iterator cI = szPlot_List.begin(); cI != szPlot_List.end(); cI++)
		{
			std::ostringstream szRemove_Command;
			std::ostringstream szRemove2_Command;
			szRemove_Command << "rm ";
			szRemove2_Command << "rm ";
			szRemove_Command << cI->second;
			szRemove2_Command << cI->second;
			szRemove2_Command << ".png";
			system(szRemove_Command.str().c_str());
			system(szRemove2_Command.str().c_str());
		}
	}

	return 0;
}
