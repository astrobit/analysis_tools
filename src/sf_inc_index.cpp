#include <specfit.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>

void specfit::Inc_Index(unsigned int * io_plIndices, unsigned int i_uiNum_Indices, unsigned int i_uiWrap_Value)
{
	if (i_uiNum_Indices != 0)
	{
		io_plIndices[0]++;
		if (io_plIndices[0] >= i_uiWrap_Value)
		{
			io_plIndices[0] -= i_uiWrap_Value;
			Inc_Index(&io_plIndices[1],i_uiNum_Indices - 1,i_uiWrap_Value);
		}
	}
}
