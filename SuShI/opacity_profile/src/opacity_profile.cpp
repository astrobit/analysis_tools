#include <opacity_profile_data.h>

void opacity_profile_data::Load(size_t i_tModel)
{
	char lpszFilename[256] = {0};
	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
	// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
	const char lpszData_Dir[] = {DATADIR};
	sprintf(lpszFilename,"%s/%i/opacity_map_scalars.opdata",lpszData_Dir,i_tModel);
#endif
	if (lpszLA_Data_Path != nullptr)
	{
		sprintf(lpszFilename,"%s/%i/opacity_map_scalars.opdata",lpszLA_Data_Path,i_tModel);
	}
	if (lpszFilename[0] != 0)
	{
		Load(lpszFilename);
	}
	
}

