#include <math.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <float.h>
#include <line_routines.h>
#include <stack>



void	SPECTRAL_DATABASE::Set_Base_Data(unsigned int i_uiIon, const char * i_lpszModel_Name)
{
	if (i_uiIon != 0 && i_uiIon != m_uiIon && ((!i_lpszModel_Name && m_lpszModel_Name && m_lpszModel_Name[0] != 0) || (!m_lpszModel_Name && i_lpszModel_Name && i_lpszModel_Name[0] != 0) || (m_lpszModel_Name && i_lpszModel_Name && strcmp(i_lpszModel_Name,m_lpszModel_Name) != 0)))
		Deallocate(); // there has been a change in the ion or model
	m_uiIon = i_uiIon;
	if (i_lpszModel_Name && i_lpszModel_Name[0] != 0)
	{
		m_lpszModel_Name = new char[strlen(i_lpszModel_Name) + 1];
		strcpy(m_lpszModel_Name,i_lpszModel_Name);
	}
	if (m_lpszModel_Name && m_lpszModel_Name[0] != 0)
	{
		unsigned int i_uiLength = 30 + strlen(m_lpszModel_Name);
		m_lpszDatabase_Filename = new char[i_uiLength];
		sprintf(m_lpszDatabase_Filename,".spectradata.%s.%i.database",m_lpszModel_Name,m_uiIon);
	}
}
void	SPECTRAL_DATABASE::Get_Base_Data(unsigned int &o_uiIon, char * o_lpszModel_Name, unsigned int i_uiMax_Model_Name_Length)
{
	o_uiIon = m_uiIon;
	if (o_lpszModel_Name && i_uiMax_Model_Name_Length > 0)
		o_lpszModel_Name[0] = 0;
	if (o_lpszModel_Name && m_lpszModel_Name && i_uiMax_Model_Name_Length > 0)
	{
		strncpy(o_lpszModel_Name,m_lpszModel_Name,i_uiMax_Model_Name_Length);
	}
}

unsigned int SPECTRAL_DATABASE::Read_Database(void)
{
	FILE * fileIn;
	unsigned int uiFault_Code = 0;
	unsigned int uiStrLen_Model_Name = 0;
	double			* lpdDouble_Buffer = nullptr;
	unsigned int	uiDouble_Buffer_Length = 0;
	char * 			lpszModel_Name_Buffer = nullptr;
	unsigned int	uiIon = 0;
	unsigned int	uiModel_Name_Length = 0;
	unsigned int	uiParameter_Vector_Length = 0;
	unsigned int	uiNum_Parameter_Vectors = 0;
	unsigned int	uiNum_Tuples = 0;
	if (!m_lpszModel_Name)
		uiFault_Code = 100; // no model name specified

	if (uiFault_Code == 0)
	{
		uiStrLen_Model_Name = strlen(m_lpszModel_Name) + 1;
		if (uiStrLen_Model_Name == 1)
			uiFault_Code = 100; // no model name specified
	}

	if (uiFault_Code == 0)
	{
		if (m_uiIon == 0 || m_uiIon > 9900)
			uiFault_Code = 200; // invalid ion
	}
	// make sure that there is no data in the database
	Deallocate_Non_Base_Data();
	Zero_Non_Base_Data();

	if (uiFault_Code == 0)
	{
		if (!m_lpszDatabase_Filename && m_lpszDatabase_Filename[0] == 0)
			uiFault_Code = 300; // invalid filename
	}
	if (uiFault_Code == 0)
	{
		fileIn = fopen(m_lpszDatabase_Filename,"rb");

		if (!fileIn)
			uiFault_Code = 305; // failed to open file
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiIon,sizeof(unsigned int),1,fileIn);

		if (iRead_Size != 1)
			uiFault_Code = 400; // failed to read ion.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiIon != m_uiIon)
			uiFault_Code = 405; // ion mismatch
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiModel_Name_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 500; // failed to read model name length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiModel_Name_Length != uiStrLen_Model_Name) // ensure that the current and saved model names are of the same length
			uiFault_Code = 505; // model name length mismatch
	}

	if (uiFault_Code == 0)
	{
		lpszModel_Name_Buffer = new char [uiModel_Name_Length];
		size_t iRead_Size = fread(lpszModel_Name_Buffer,sizeof(char),uiModel_Name_Length,fileIn);
		if (iRead_Size != uiModel_Name_Length)
			uiFault_Code = 510; // failed to read model name.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (strcmp(lpszModel_Name_Buffer,m_lpszModel_Name) != 0) // make sure the data in the file matches
			uiFault_Code = 515; // model name mismatch
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiParameter_Vector_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 600; // failed to read parameter vector length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiParameter_Vector_Length == 0 || uiParameter_Vector_Length > 128)//@@WARNING: hard coded limit of 128 parameters
			uiFault_Code = 605; // invalid parameter vector length
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiNum_Parameter_Vectors,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 700; // failed to read model name.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiNum_Parameter_Vectors == 0)
			uiFault_Code = 705; // invalid parameter vector number
	}
	if (uiFault_Code == 0)
	{
		m_lpvParameter_Data = new XVECTOR[uiNum_Parameter_Vectors];
		if (!m_lpvParameter_Data)
			uiFault_Code = 800; // failed to allocate parameter vector data
	}
	if (uiFault_Code == 0)
	{
		m_lplpcTuple_List_Heads = new SPECTRAL_DATABASE_TUPLE_NODE *[uiNum_Parameter_Vectors];
		if (!m_lplpcTuple_List_Heads)
			uiFault_Code = 900; // failed to allocate tuple list head array
	}

	if (uiFault_Code == 0)
	{
		m_lplpcTuple_List_Tails = new SPECTRAL_DATABASE_TUPLE_NODE *[uiNum_Parameter_Vectors];
		if (!m_lplpcTuple_List_Tails)
			uiFault_Code = 905; // failed to allocate tuple list tail array
	}

	if (uiFault_Code == 0)
	{
		lpdDouble_Buffer = new double[uiParameter_Vector_Length];
		uiDouble_Buffer_Length = uiParameter_Vector_Length;
		if (!lpdDouble_Buffer)
			uiFault_Code = 1000; // failed to allocate temporary buffer for double precision data
	}
	for(unsigned int uiI = 0; uiI < uiNum_Parameter_Vectors && uiFault_Code == 0; uiI++)
	{
		
		m_lpvParameter_Data[uiI].Set_Size(uiParameter_Vector_Length);
		m_lplpcTuple_List_Heads[uiI] = m_lplpcTuple_List_Tails[uiI] = nullptr;
		size_t iRead_Size = fread(lpdDouble_Buffer,sizeof(double),uiParameter_Vector_Length,fileIn);
		if (iRead_Size < uiParameter_Vector_Length)
			uiFault_Code = 100000000 + uiI; // failed to read parameter data for vector i. Unexpected end of file or file access error
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fread(&uiNum_Tuples,sizeof(unsigned int),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 200000000 + uiI; // failed to read number of tuples for vector i. Unexpected end of file or file access error
		}
		if (uiFault_Code == 0)
		{
			if (uiDouble_Buffer_Length < (uiNum_Tuples * 2))
			{
				if (lpdDouble_Buffer)
					delete [] lpdDouble_Buffer;
				uiDouble_Buffer_Length = uiNum_Tuples * 2;
				lpdDouble_Buffer = new double[uiDouble_Buffer_Length];
				if (!lpdDouble_Buffer)
					uiFault_Code = 300000000 + uiI; // failed to reallocate double buffer during vector i. 
			}
		}
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fread(lpdDouble_Buffer,sizeof(double),uiNum_Tuples,fileIn);
			if (iRead_Size != uiNum_Tuples)
				uiFault_Code = 400000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
		}
		if (uiFault_Code == 0)
		{
			for (unsigned int uiJ = 0; uiJ < uiNum_Tuples && uiFault_Code == 0; uiJ+=2)
			{
				unsigned int uiWL_Idx = uiJ * 2;
				unsigned int uiInt_Idx = uiWL_Idx + 1;
				SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = new SPECTRAL_DATABASE_TUPLE_NODE;
				if (!lpCurr)
					uiFault_Code = 500000000 + uiI; // failed to allocate a tuple node for vector i
				if (uiFault_Code == 0)
				{
					lpCurr->m_dWavelength = lpdDouble_Buffer[uiWL_Idx];
					lpCurr->m_dIntensity = lpdDouble_Buffer[uiInt_Idx];
					if (m_lplpcTuple_List_Heads[uiI] == nullptr)
					{
						m_lplpcTuple_List_Heads[uiI] = lpCurr;
						m_lplpcTuple_List_Tails[uiI] = lpCurr;
					}
					else
					{
						lpCurr->m_lpPrev = m_lplpcTuple_List_Tails[uiI];
						m_lplpcTuple_List_Tails[uiI] = lpCurr;
					}
				}
			}
		}
	}
	if (uiFault_Code != 0)
	{
		Deallocate();
	}
	// deallocate local temporary storage
	if (lpdDouble_Buffer)
		delete [] lpdDouble_Buffer;
	if (lpszModel_Name_Buffer)
		delete [] lpszModel_Name_Buffer;
	// return fault code
	return uiFault_Code;	
}
unsigned int SPECTRAL_DATABASE::Save_Database(void)
{
	FILE * fileIn = nullptr;
	unsigned int uiFault_Code = 0;
	unsigned int uiModel_Name_Length;
	unsigned int uiParameter_Vector_Length;
	
	if (!m_lpszModel_Name)
		uiFault_Code = 100; // no model name specified

	if (uiFault_Code == 0)
	{
		uiModel_Name_Length = strlen(m_lpszModel_Name) + 1;
		if (uiModel_Name_Length == 1)
			uiFault_Code = 100; // no model name specified
	}

	if (uiFault_Code == 0)
	{
		if (m_uiIon == 0 || m_uiIon > 9900)
			uiFault_Code = 200; // invalid ion
	}

	if (uiFault_Code == 0)
	{
		if (!m_lpszDatabase_Filename && m_lpszDatabase_Filename[0] == 0)
			uiFault_Code = 300; // invalid filename
	}
	if (uiFault_Code == 0)
	{
		fileIn = fopen(m_lpszDatabase_Filename,"wb");

		if (!fileIn)
			uiFault_Code = 305; // failed to open file
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&m_uiIon,sizeof(unsigned int),1,fileIn);

		if (iRead_Size != 1)
			uiFault_Code = 400; // failed to read ion.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&uiModel_Name_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 500; // failed to read model name length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(m_lpszModel_Name,sizeof(char),uiModel_Name_Length,fileIn);
		if (iRead_Size != uiModel_Name_Length)
			uiFault_Code = 510; // failed to read model name.  Unexpected EOF or file access error.
	}
	if (uiFault_Code == 0)
	{
		uiParameter_Vector_Length = m_lpvParameter_Data[0].Get_Size();
		if (uiParameter_Vector_Length == 0 || uiParameter_Vector_Length > 128)//@@WARNING: hard coded limit of 128 parameters
			uiFault_Code = 605; // invalid parameter vector length
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&uiParameter_Vector_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 600; // failed to read parameter vector length.  Unexpected EOF or file access error.
	}


	if (uiFault_Code == 0)
	{
		if (m_uiNum_Parameter_Vectors == 0)
			uiFault_Code = 705; // invalid parameter vector number
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&m_uiNum_Parameter_Vectors,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 700; // failed to read model name.  Unexpected EOF or file access error.
	}

	for(unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors && uiFault_Code == 0; uiI++)
	{
		for( unsigned int uiJ = 0; uiJ < uiParameter_Vector_Length && uiFault_Code == 0; uiJ++)
		{
			double dBuffer = m_lpvParameter_Data[uiI].Get(uiJ);
			size_t iRead_Size = fwrite(&dBuffer,sizeof(double),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 100000000 + uiI; // failed to read parameter data for vector i. Unexpected end of file or file access error
		}
		unsigned int uiNum_Tuples = 0;
		SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiI];
		// count tuples
		while (lpCurr != nullptr)
		{
			uiNum_Tuples++;
			lpCurr = lpCurr->m_lpNext;
		}
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fwrite(&uiNum_Tuples,sizeof(unsigned int),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 200000000 + uiI; // failed to read number of tuples for vector i. Unexpected end of file or file access error
		}
		// go through tuple list, writing each one to file
		lpCurr = m_lplpcTuple_List_Heads[uiI];
		while (lpCurr != nullptr && uiFault_Code == 0)
		{
			size_t iRead_Size = fwrite(&(lpCurr->m_dWavelength),sizeof(double),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 600000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
			if (uiFault_Code == 0)
			{
				size_t iRead_Size = fwrite(&(lpCurr->m_dIntensity),sizeof(double),1,fileIn);
				if (iRead_Size != 1)
					uiFault_Code = 700000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
			}

			lpCurr = lpCurr->m_lpNext;
		}
		
	}
	if (uiFault_Code != 0)
	{
		if (fileIn)
			fclose(fileIn);
		if (uiFault_Code > 305)
		{
			char lpszCommand[256];
			sprintf(lpszCommand,"rm %s",m_lpszDatabase_Filename);
			system(lpszCommand);
		}
	}
	// return fault code
	return uiFault_Code;	
}


unsigned int		SPECTRAL_DATABASE::Add_Parameter_Vector(const XVECTOR & i_vParameters)
{
	unsigned int uiRet = Find_Parameter_Vector(i_vParameters);
	if (uiRet == -1)
	{// not found, add
		m_uiNum_Parameter_Vectors++;
		XVECTOR			* lpNew_List = new XVECTOR[m_uiNum_Parameter_Vectors];
		SPECTRAL_DATABASE_TUPLE_NODE ** lplpcNew_Heads = new SPECTRAL_DATABASE_TUPLE_NODE *[m_uiNum_Parameter_Vectors];
		SPECTRAL_DATABASE_TUPLE_NODE ** lplpcNew_Tails = new SPECTRAL_DATABASE_TUPLE_NODE *[m_uiNum_Parameter_Vectors];
		m_uiNum_Parameter_Vectors--;
		if (lpNew_List && lplpcNew_Heads && lplpcNew_Tails)
		{
			for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors; uiI++)
			{
				lpNew_List[uiI] = m_lpvParameter_Data[uiI];
				lplpcNew_Heads[uiI] = m_lplpcTuple_List_Heads[uiI];
				lplpcNew_Tails[uiI] = m_lplpcTuple_List_Tails[uiI];
			}
			lpNew_List[m_uiNum_Parameter_Vectors] = i_vParameters;
			lplpcNew_Heads[m_uiNum_Parameter_Vectors] = nullptr;
			lplpcNew_Tails[m_uiNum_Parameter_Vectors] = nullptr;
			m_uiNum_Parameter_Vectors++;
			if (m_lpvParameter_Data)
				delete [] m_lpvParameter_Data;
			if (m_lplpcTuple_List_Heads)
				delete [] m_lplpcTuple_List_Heads;
			if (m_lplpcTuple_List_Tails)
				delete [] m_lplpcTuple_List_Tails;

			m_lpvParameter_Data = lpNew_List;
			m_lplpcTuple_List_Heads = lplpcNew_Heads;
			m_lplpcTuple_List_Tails = lplpcNew_Tails;
		}
	}
}
unsigned int		SPECTRAL_DATABASE::Find_Parameter_Vector(const XVECTOR & i_vParameters)
{
	unsigned int uiRet = -1;
	for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors && uiRet == -1; uiI++)
	{
		if (i_vParameters == m_lpvParameter_Data[uiI])
			uiRet = uiI;
	}
	return uiRet;
}

void 	SPECTRAL_DATABASE::Add_Spectrum(const XVECTOR & i_vParameters, const ES::Spectrum &i_cSpectrum)
{
	unsigned int uiIdx = Find_Parameter_Vector(i_vParameters);
	if (uiIdx == -1)
		uiIdx = Add_Parameter_Vector(i_vParameters);
	if (uiIdx != -1)
	{
		SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
		for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && uiIdx != -1; uiI++)
		{
			while (lpCurr && lpCurr->m_dWavelength <= i_cSpectrum.wl(uiI))
				lpCurr = lpCurr->m_lpNext;
			if (!lpCurr || lpCurr->m_dWavelength != i_cSpectrum.wl(uiI))
			{
				SPECTRAL_DATABASE_TUPLE_NODE * lpNew = new SPECTRAL_DATABASE_TUPLE_NODE;
				if (lpNew)
				{
					lpNew->m_dWavelength = i_cSpectrum.wl(uiI);
					lpNew->m_dIntensity = i_cSpectrum.flux(uiI);
					if (!m_lplpcTuple_List_Heads[uiI])
					{
						m_lplpcTuple_List_Heads[uiI] = lpNew;
						m_lplpcTuple_List_Tails[uiI] = lpNew;
					}
					else if (!lpCurr)
					{
						m_lplpcTuple_List_Tails[uiI]->m_lpNext = lpNew;
						lpNew->m_lpPrev = m_lplpcTuple_List_Tails[uiI];
						m_lplpcTuple_List_Tails[uiI] = lpNew;
					}
					else
					{
						lpNew->m_lpNext = lpCurr;
						lpNew->m_lpPrev = lpCurr->m_lpPrev;
						lpCurr->m_lpPrev = lpNew;
						if (lpNew->m_lpPrev)
							lpNew->m_lpPrev->m_lpNext = lpNew;
						if (lpCurr == m_lplpcTuple_List_Heads[uiI]) // if we are pointing at the head, update the head.
							m_lplpcTuple_List_Heads[uiI] = lpNew;
					}
					lpCurr = lpNew; // set the current pointer to the newly added node
				}
				else
					uiIdx = -1;
			}
		}
	}
	if (uiIdx != -1)
	{
		unsigned int uiError = Save_Database();
		if (uiError != 0)
			fprintf(stderr,"Error %i while saving spectral database.\n",uiError);
	}
}


bool 	SPECTRAL_DATABASE::Find_Spectrum(const XVECTOR & i_vParameters, ES::Spectrum & io_cOutput, bool i_bAllow_Interpolate, bool i_bGenerate_Wavelength_List) // allow interpolate flag will return f(wl) interpolated between known nearby wl if the exact wl value is 
{
	bool bSuccess = false;
	unsigned int uiIdx = Find_Parameter_Vector(i_vParameters);
	if (i_bGenerate_Wavelength_List)
		io_cOutput.zero_out();
	else
		io_cOutput.zero_flux();
	if (uiIdx != -1)
	{
		if (i_bGenerate_Wavelength_List)
		{
			SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			unsigned int uiCount = 0;
			while(lpCurr)
			{
				uiCount++;
				lpCurr = lpCurr->m_lpNext;
			}
			io_cOutput = ES::Spectrum::create_from_size(uiCount);

			lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			uiCount = 0;
			while(lpCurr)
			{
				io_cOutput.wl(uiCount) = lpCurr->m_dWavelength;
				io_cOutput.flux(uiCount) = lpCurr->m_dIntensity;
				uiCount++;
				lpCurr = lpCurr->m_lpNext;
			}
			bSuccess = true;
		}
		else
		{
			bSuccess = true;
			SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			for (unsigned int uiI = 0; uiI < io_cOutput.size() && bSuccess; uiI++)
			{
				while (lpCurr && lpCurr->m_dWavelength < io_cOutput.wl(uiI))
					lpCurr = lpCurr->m_lpNext;
				if (lpCurr->m_dWavelength == io_cOutput.wl(uiI))
					io_cOutput.flux(uiI) = lpCurr->m_dIntensity;
				else if (i_bAllow_Interpolate && lpCurr && lpCurr->m_lpPrev)
				{
					// @@TODO more interpolation techniques
					double dSlope = (lpCurr->m_dIntensity - lpCurr->m_lpPrev->m_dIntensity) / (lpCurr->m_dWavelength - lpCurr->m_lpPrev->m_dWavelength);
					double	dX = (io_cOutput.wl(uiI) - lpCurr->m_lpPrev->m_dWavelength);
					io_cOutput.flux(uiI) = dX * dSlope + lpCurr->m_lpPrev->m_dIntensity;
				}
				else
					bSuccess = false;
			}
		}
	}
	return bSuccess;
}
