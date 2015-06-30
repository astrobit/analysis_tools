#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>

template <typename T> void Output_Item(std::ofstream & osFile, const YAML::Node & cNode, const std::string &i_cFirst, const std::string & i_cSecond)
{
	T tData = cNode[i_cFirst][i_cSecond].as<T>();
	osFile << i_cFirst << ", " << i_cSecond << ", " << tData << std::endl;
}

void Output_Item_Bool(std::ofstream & osFile, const YAML::Node & cNode, const std::string &i_cFirst, const std::string & i_cSecond)
{
	bool tData = cNode[i_cFirst][i_cSecond].as<bool>();
	osFile << i_cFirst << ", " << i_cSecond << ", ";
	if (tData)
		osFile << "Yes";
	else
		osFile << "No";
	osFile << std::endl;
}

template <typename T> void Output_Setup_Item_Single(std::ofstream & osFile, const YAML::Node & cSetups_Node, const std::string &i_cFirst, unsigned int i_uiSetup_Num)
{

	osFile << "setups, " << i_uiSetup_Num << ", " << i_cFirst;
	unsigned int uiNum_Nodes = cSetups_Node[i_cFirst].size();
	T tData = cSetups_Node[i_cFirst].as<T>();
	osFile << ", " << tData;
	osFile << std::endl;
}

template <typename T> void Output_Setup_Items(std::ofstream & osFile, const YAML::Node & cSetups_Node, const std::string &i_cFirst, unsigned int i_uiSetup_Num)
{

	osFile << "setups, " << i_uiSetup_Num << ", " << i_cFirst;
	unsigned int uiNum_Nodes = cSetups_Node[i_cFirst].size();
	for(unsigned int uiI = 0; uiI < uiNum_Nodes; uiI++)
	{
		T tData = cSetups_Node[i_cFirst][uiI].as<T>();
		osFile << ", " << tData;
	}
	osFile << std::endl;
}

void Output_Setup_Items_Bool(std::ofstream & osFile, const YAML::Node & cSetups_Node, const std::string &i_cFirst, unsigned int i_uiSetup_Num)
{

	osFile << "setups, " << i_uiSetup_Num << ", " << i_cFirst;
	unsigned int uiNum_Nodes = cSetups_Node[i_cFirst].size();
	for(unsigned int uiI = 0; uiI < uiNum_Nodes; uiI++)
	{
		bool tData = cSetups_Node[i_cFirst][uiI].as<bool>();
		if (tData)
			osFile << ", Yes";
		else
			osFile << ", No";
	}
	osFile << std::endl;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	if (i_iArg_Count >= 2)
	{
		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			const char * lpszFilename = i_lpszArg_Values[uiI];
			std::string cszOutfilename = lpszFilename;
			cszOutfilename.append(".csv");
			
			YAML::Node				cSyn_File;
			cSyn_File = YAML::LoadFile(lpszFilename);

			std::ofstream osOutfile;
			osOutfile.open(cszOutfilename.c_str(),std::ofstream::out);
			if (osOutfile.is_open())
			{
				
				Output_Item<double>(osOutfile,cSyn_File,"output","min_wl");
				Output_Item<double>(osOutfile,cSyn_File,"output","max_wl");
				Output_Item<double>(osOutfile,cSyn_File,"output","wl_step");
				Output_Item<double>(osOutfile,cSyn_File,"grid","bin_width");
				Output_Item<double>(osOutfile,cSyn_File,"grid","v_size");
				Output_Item<double>(osOutfile,cSyn_File,"grid","v_outer_max");
				Output_Item<std::string>(osOutfile,cSyn_File,"opacity","form");
				Output_Item<double>(osOutfile,cSyn_File,"opacity","v_ref");
				Output_Item<double>(osOutfile,cSyn_File,"opacity","log_tau_min");
				Output_Item<int>(osOutfile,cSyn_File,"source","mu_size");
				Output_Item<int>(osOutfile,cSyn_File,"spectrum","p_size");
				Output_Item_Bool(osOutfile,cSyn_File,"spectrum","flatten");
				const YAML::Node & cSetups = cSyn_File["setups"];

				unsigned int uiSetup_Num = 0;
				for( YAML::const_iterator iterI = cSetups.begin(); iterI != cSetups.end(); ++ iterI )
				{
					const YAML::Node cIterNode = *iterI;
					Output_Setup_Item_Single<double>(osOutfile,cIterNode,"a0",uiSetup_Num);
					Output_Setup_Item_Single<double>(osOutfile,cIterNode,"a1",uiSetup_Num);
					Output_Setup_Item_Single<double>(osOutfile,cIterNode,"a2",uiSetup_Num);
					Output_Setup_Item_Single<double>(osOutfile,cIterNode,"v_phot",uiSetup_Num);
					Output_Setup_Item_Single<double>(osOutfile,cIterNode,"v_outer",uiSetup_Num);
					Output_Setup_Item_Single<double>(osOutfile,cIterNode,"t_phot",uiSetup_Num);
					Output_Setup_Items<int>(osOutfile,cIterNode,"ions",uiSetup_Num);
					Output_Setup_Items_Bool(osOutfile,cIterNode,"active",uiSetup_Num);
					Output_Setup_Items<double>(osOutfile,cIterNode,"log_tau",uiSetup_Num);
					Output_Setup_Items<double>(osOutfile,cIterNode,"v_min",uiSetup_Num);
					Output_Setup_Items<double>(osOutfile,cIterNode,"v_max",uiSetup_Num);
					Output_Setup_Items<double>(osOutfile,cIterNode,"aux",uiSetup_Num);
					Output_Setup_Items<double>(osOutfile,cIterNode,"temp",uiSetup_Num);

					uiSetup_Num++;
				}
				osOutfile.close();
			}
		}
	}
	return 0;
}


