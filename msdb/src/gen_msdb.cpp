#include <iostream>
#include <model_spectra_db.h>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	msdb::DATABASE cMSDB(true);
	if (cMSDB.Database_Exists())
		std::cout << "Database exists or has been created." << std::endl;

	return 0;
}
